"""Mini Redis 시연: 미션 요구사항을 명령 하나씩 실행하고, 그때마다 Redis 내부 상태를 보여 준다.

python3 scripts/demo.py          한 화면을 갱신하며 엔터로 한 단계씩 (a: 끝까지 자동, Ctrl+C: 중단)
python3 scripts/demo.py --auto   끝까지 자동
터미널이 아닌 곳(| tee build/demo.log)으로 출력하면 화면 갱신 없이 한 줄씩 로그를 남긴다.

명령은 python/main.py의 tokenize·execute로 실행한다. 사용자가 CLI에 입력했을 때와 같은 파싱·출력이고,
같은 프로세스라서 Store 내부(LRU 순서, TTL, 메모리)를 직접 읽어 화면에 보여 줄 수 있다.
"""

import os
import subprocess
import sys
import time
import unicodedata

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "python"))

from main import ERR_QUOTES, execute, now_ms, tokenize  # noqa: E402
from store import Store                      # noqa: E402

TTY = sys.stdout.isatty()
OOM = "(error) OOM command not allowed when used_memory > 'maxmemory'"
NOT_INT = "(error) ERR value is not an integer or out of range"


def info(used, maxmemory, evicted):
    return "used_memory:%d\nmaxmemory:%d\nevicted_keys:%d" % (used, maxmemory, evicted)


def keys(*names):
    """KEYS 결과를 순서와 무관하게 비교한다."""
    def f(out):
        lines = out.splitlines()
        if not all(line.startswith("%d. " % (i + 1)) for i, line in enumerate(lines)):
            return False  # 번호가 1부터 차례로 붙어야 한다 (미션 출력 형식)
        return sorted(line.split(". ", 1)[1] for line in lines) == sorted('"%s"' % n for n in names)
    f.desc = "키 %s (순서 무관)" % ", ".join(names)
    f.names = names
    return f


def between(lo, hi):
    """시간에 따라 달라지는 TTL 결과를 범위로 비교한다."""
    f = lambda out: out.startswith("(integer) ") and lo <= int(out.split()[1]) <= hi
    f.desc = "(integer) %d ~ %d" % (lo, hi)
    return f


# (제목, [(명령, 설명, 기대값)])
# 기대값: 문자열이면 그대로 비교, 함수면 함수로 판정. 명령이 숫자면 그 초만큼 대기.
SCENARIOS = [
    ("String 명령과 Redis 스타일 출력", [
        ('SET user:1 "Alice"', "큰따옴표 값 저장", "OK"),
        ("GET user:1", "값은 큰따옴표로 출력", '"Alice"'),
        ("SET user:2 Bob", "따옴표 없는 값", "OK"),
        ("EXISTS user:2", "있는 키", "(integer) 1"),
        ("DBSIZE", "키 개수", "(integer) 2"),
        ("KEYS", "전체 키 (순서 무관)", keys("user:1", "user:2")),
        ("DEL user:1", "삭제 성공", "(integer) 1"),
        ("DEL user:1", "없는 키", "(integer) 0"),
        ("GET user:1", "없는 키", "(nil)"),
    ]),
    ("메모리 제한 + LRU 제거 (미션 8절 예시)", [
        ("CONFIG SET maxmemory 30", "최대 30바이트", "OK"),
        ('SET user:1 "Alice"', "6 + 5 = 11", "OK"),
        ('SET user:2 "Bob"', "6 + 3 = 9 → 합계 20", "OK"),
        ('SET user:3 "Charlie"', "6 + 7 = 13 → 33 > 30, 가장 오래된 user:1 제거", "OK"),
        ("GET user:1", "LRU로 제거됨", "(nil)"),
        ("INFO memory", "33 - 11 = 22, evicted 1", info(22, 30, 1)),
        ("GET user:2", "user:2를 사용 → 이제 가장 오래된 키는 user:3", '"Bob"'),
        ('SET 이름 "철수"', "UTF-8: 6 + 6 = 12 → 34 > 30, user:3 제거", "OK"),
        ("KEYS", "GET한 user:2는 남음", keys("user:2", "이름")),
        ("SET huge 12345678901234567890123456789", "단일 엔트리 4 + 29 = 33 > 30 → OOM", OOM),
        ("EXISTS huge", "저장되지 않음", "(integer) 0"),
        ("INFO memory", "OOM은 아무것도 바꾸지 않음: 9 + 12 = 21", info(21, 30, 2)),
    ]),
    ("TTL (힙 기반 만료)", [
        ('SET user:2 "Bob"', "키 저장", "OK"),
        ("TTL user:2", "만료 없음", "(integer) -1"),
        ("TTL none", "없는 키", "(integer) -2"),
        ("EXPIRE none 10", "없는 키에 EXPIRE", "(integer) 0"),
        ("EXPIRE user:2 100", "100초 뒤 만료", "(integer) 1"),
        ("TTL user:2", "남은 초 (내림). 엔터가 늦어도 100초 안이면 통과", between(0, 100)),
        ("EXPIRE user:2 3", "3초로 다시 설정. 이전 100초 기록은 힙에 남음(lazy deletion)", "(integer) 1"),
        (3.2, "3초 대기. 시간이 지나도 키는 다음 명령이 올 때 정리된다", None),
        ("GET user:2", "명령 전 만료 정리 → 삭제 후 (nil)", "(nil)"),
        ("TTL user:2", "만료되어 없는 키", "(integer) -2"),
        ("SET a 1", "키 저장", "OK"),
        ("EXPIRE a 100", "100초 뒤 만료", "(integer) 1"),
        ("SET a 2", "SET 덮어쓰기 → TTL 없음. 힙의 옛 항목은 남음(lazy deletion)", "OK"),
        ("TTL a", "덮어쓰기는 TTL 초기화", "(integer) -1"),
        ("EXPIRE a 0", "0 이하는 즉시 만료", "(integer) 1"),
        ("EXISTS a", "삭제됨", "(integer) 0"),
    ]),
    ("에러 처리와 종료", [
        ("HELLO", "없는 명령", "(error) ERR unknown command 'HELLO'"),
        ("GET", "인자 개수 오류", "(error) ERR wrong number of arguments for 'GET' command"),
        ("CONFIG SET maxmemory abc", "정수 파싱 실패", NOT_INT),
        ("quit", "실제 CLI(main.py)에 quit을 보내 정상 종료(exit code 0) 확인", None),
    ]),
]

RED, BOLD, RESET = (c if TTY else "" for c in ("\033[31m", "\033[1m", "\033[0m"))
CLEAR = "\033[2J\033[H"
RULE = "─" * 72


# ---------- 실행 ----------

def run(store, cmd, expected):
    """명령 하나를 실행해 (출력, 통과 여부)를 돌려준다."""
    if not isinstance(cmd, str):
        time.sleep(cmd)
        return "%.1f초 지남" % cmd, True
    if cmd == "quit":  # 종료는 REPL 루프가 처리하므로 실제 CLI를 띄워 확인
        main_py = os.path.join(ROOT, "python", "main.py")
        code = subprocess.run([sys.executable, main_py], input=b"quit\n",
                              stdout=subprocess.DEVNULL).returncode
        return "(프로그램 종료, exit code %d)" % code, code == 0
    tokens = tokenize(cmd)
    out = ERR_QUOTES if tokens is None else execute(store, tokens, now_ms())
    return out, expected(out) if callable(expected) else out == expected


# ---------- 화면 ----------

def width(text):
    """터미널에서 차지하는 칸 수. 한글은 2칸."""
    return sum(2 if unicodedata.east_asian_width(ch) in "WF" else 1 for ch in text)


def pad(text, n):
    return text + " " * max(0, n - width(text))


def state_lines(store):
    """LRU 순서대로 키·값·크기·TTL과 메모리 상태를 보여 준다. 만료 정리는 하지 않고 있는 그대로 읽는다."""
    now = now_ms()
    lines = ["Redis 상태   used_memory %d / maxmemory %s   evicted_keys %d   힙 항목 %d" % (
        store.used_memory, store.maxmemory or "0(무제한)", store.evicted_keys, store.heap.size())]
    node = store.lru.HEAD
    if node is None:
        return lines + ["  (비어 있음)"]
    lines.append("  LRU 순서: 최근 → 오래됨")
    rank = 1
    while node:
        d = node.data
        size = len(d.key.encode("utf-8")) + len(d.value.encode("utf-8"))
        if d.expire_at == 0:
            ttl = "TTL 없음"
        elif d.expire_at <= now:
            ttl = "만료됨 (다음 명령 때 정리)"
        else:
            ttl = "TTL %d초" % ((d.expire_at - now) // 1000)
        tail = "   ← 다음 제거 대상" if node is store.lru.TAIL and store.maxmemory > 0 else ""
        lines.append("  %d. %s %s %3dB   %s%s" % (rank, pad(d.key, 8), pad('"%s"' % d.value, 10), size, ttl, tail))
        node, rank = node.next, rank + 1
    return lines


def label(cmd):
    return cmd if isinstance(cmd, str) else "대기 %.1f초" % cmd


def output_lines(tasks):
    """출력 칸 높이: 기대 출력 중 가장 긴 줄 수(INFO memory = 3줄) + 실패 표시 1줄."""
    def n(expected):
        if isinstance(expected, str):
            return expected.count("\n") + 1
        return len(getattr(expected, "names", ())) or 1
    return max(n(t[3]) for t in tasks) + 1


def render(task, no, store, result, new_scen, height):
    """한 화면: TASK와 명령 → 출력 칸(높이 고정) → Redis 상태. result가 None이면 실행 전."""
    scen, cmd, note, expected = task
    print(CLEAR, end="")
    print("[%s]%s" % (scen, "  빈 Redis에서 시작" if new_scen else ""))
    print("%sTASK-%03d%s  %s" % (BOLD, no, RESET, note))
    print("  mini-redis> %s%s%s" % (BOLD, cmd, RESET) if isinstance(cmd, str)
          else "  %s%s%s" % (BOLD, label(cmd), RESET))

    lines = []
    if result is not None:
        out, ok = result
        lines = ["  " + line for line in (out.splitlines() or [""])]
        if not ok:  # 기대와 다를 때만 표시
            exp = "exit code 0" if cmd == "quit" else getattr(expected, "desc", expected)
            lines.append("%s  ✗ 기대: %s%s" % (RED, str(exp).replace("\n", " / "), RESET))
    for line in (lines + [""] * height)[:max(height, len(lines))]:
        print(line)

    print(RULE)
    print("(실행 전)" if result is None else "(실행 후)")
    for line in state_lines(store):
        print(line)
    print(RULE)


def log_line(no, task, result):
    """터미널이 아닐 때(파일 로그) 한 줄 기록."""
    scen, cmd, note, expected = task
    out, ok = result
    print("%s TASK-%03d [%s] %s → %s" % ("✓" if ok else "✗", no, scen, label(cmd), out.replace("\n", " / ")))
    if not ok:
        print("    기대: %s" % str(getattr(expected, "desc", expected)).replace("\n", " / "))


# ---------- 진행 ----------

def wait(auto):
    """엔터를 기다린다. a를 누르거나 입력이 없으면 자동 모드로 바뀐다. 반환: auto."""
    if auto:
        time.sleep(0.5)
        return True
    try:
        return input().strip().lower() == "a"  # 안내 문구 없이 입력만 받는다
    except EOFError:  # 입력이 없으면(< /dev/null 등) 끝까지 자동
        return True


def main():
    auto = "--auto" in sys.argv
    tasks = [(title, cmd, note, exp) for title, steps in SCENARIOS for cmd, note, exp in steps]
    height = output_lines(tasks)
    results = []
    store = None

    for idx, task in enumerate(tasks):
        new_scen = idx == 0 or tasks[idx - 1][0] != task[0]
        if new_scen:                          # 시나리오마다 빈 Redis로 시작
            store = Store()
        if TTY:                               # 엔터 1: 이 TASK 실행
            render(task, idx + 1, store, None, new_scen, height)
            auto = wait(auto)

        result = run(store, task[1], task[3])
        results.append(result)

        if TTY:                               # 엔터 2: 결과를 본 뒤 다음 TASK로
            render(task, idx + 1, store, result, new_scen, height)
            auto = wait(auto)
        else:
            log_line(idx + 1, task, result)

    if TTY:
        print(CLEAR, end="")
    failed = [i for i, (_, ok) in enumerate(results, 1) if not ok]
    print("결과: %d / %d 통과" % (len(results) - len(failed), len(results)))
    if failed:
        print("실패: " + ", ".join("TASK-%03d" % n for n in failed))
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n중단")
        sys.exit(130)  # 성공(0)과 구분. 셸 관례: 128 + SIGINT(2)