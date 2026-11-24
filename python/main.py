"""Mini Redis CLI(REPL). C 실습에 없던 부분이라 새로 작성했다.

입력을 토큰으로 나누고(tokenize), 명령을 실행해 Redis 스타일 문자열을 만든 뒤(execute) 출력한다.
실행: python3 python/main.py
"""

import time

from store import Store, STORE_ERR_OOM

ERR_SYNTAX = "(error) ERR syntax error"
ERR_NOT_INTEGER = "(error) ERR value is not an integer or out of range"
ERR_OOM = "(error) OOM command not allowed when used_memory > 'maxmemory'"
ERR_QUOTES = "(error) ERR unbalanced quotes in request"


def now_ms():
    """현재 시각(ms). Store의 TTL 계산에 넣는다."""
    return int(time.time() * 1000)


def tokenize(line):
    """공백으로 나누되 큰따옴표로 감싼 부분은 공백을 포함해 한 토큰으로 본다.

    반환: 토큰 배열. 따옴표가 닫히지 않으면 None.
    예: SET user:1 "Alice Kim" → ["SET", "user:1", "Alice Kim"]
    """
    tokens = []
    current = ""
    in_quote = False
    has_token = False  # "" 처럼 빈 문자열 토큰도 토큰으로 남기기 위한 표시

    for ch in line:
        if ch == '"':
            in_quote = not in_quote
            has_token = True
        elif ch.isspace() and not in_quote:
            if has_token:
                tokens.append(current)
                current = ""
                has_token = False
        else:
            current += ch
            has_token = True

    if in_quote:
        return None
    if has_token:
        tokens.append(current)
    return tokens


def parse_int(text):
    """10진수 정수 문자열이면 int, 아니면 None. 부호는 앞의 '-'만 허용한다."""
    digits = text[1:] if text.startswith("-") else text
    if digits == "" or any(ch not in "0123456789" for ch in digits):
        return None
    return int(text)


def wrong_args(cmd):
    return "(error) ERR wrong number of arguments for '%s' command" % cmd


def execute(store, tokens, now):
    """토큰 배열의 명령을 실행하고 출력할 문자열을 돌려준다. 명령 이름은 대소문자를 구분하지 않는다."""
    cmd = tokens[0]
    name = cmd.upper()
    args = tokens[1:]

    if name == "SET":
        if len(args) != 2:
            return wrong_args(cmd)
        if store.set(args[0], args[1], now) == STORE_ERR_OOM:
            return ERR_OOM
        return "OK"

    if name == "GET":
        if len(args) != 1:
            return wrong_args(cmd)
        value = store.get(args[0], now)
        if value is None:
            return "(nil)"
        return '"%s"' % value

    if name == "DEL":
        if len(args) != 1:
            return wrong_args(cmd)
        return "(integer) %d" % store.delete(args[0], now)

    if name == "EXISTS":
        if len(args) != 1:
            return wrong_args(cmd)
        return "(integer) %d" % store.exists(args[0], now)

    if name == "DBSIZE":
        if len(args) != 0:
            return wrong_args(cmd)
        return "(integer) %d" % store.dbsize(now)

    if name == "KEYS":
        if len(args) != 0:
            return wrong_args(cmd)
        keys = store.keys(now)
        if len(keys) == 0:
            return "(empty array)"
        lines = []
        for i in range(len(keys)):
            lines.append('%d. "%s"' % (i + 1, keys[i]))
        return "\n".join(lines)

    if name == "EXPIRE":
        if len(args) != 2:
            return wrong_args(cmd)
        seconds = parse_int(args[1])
        if seconds is None:
            return ERR_NOT_INTEGER
        return "(integer) %d" % store.expire(args[0], seconds, now)

    if name == "TTL":
        if len(args) != 1:
            return wrong_args(cmd)
        return "(integer) %d" % store.ttl(args[0], now)

    if name == "CONFIG":
        # 지원: CONFIG SET maxmemory <bytes>
        if len(args) != 3:
            return wrong_args(cmd)
        if args[0].upper() != "SET" or args[1].lower() != "maxmemory":
            return ERR_SYNTAX
        maxmemory = parse_int(args[2])
        if maxmemory is None or maxmemory < 0:
            return ERR_NOT_INTEGER
        # 미션 규칙상 LRU 제거는 SET 이후에만 한다. 여기서는 제한 값만 바꾼다.
        store.maxmemory = maxmemory
        return "OK"

    if name == "INFO":
        # 지원: INFO memory
        if len(args) != 1:
            return wrong_args(cmd)
        if args[0].lower() != "memory":
            return ERR_SYNTAX
        used, maxmemory, evicted = store.info_memory(now)
        return "used_memory:%d\nmaxmemory:%d\nevicted_keys:%d" % (used, maxmemory, evicted)

    return "(error) ERR unknown command '%s'" % cmd


def main():
    """REPL: 프롬프트 출력 → 입력 → 실행 → 출력을 반복한다. exit/quit 또는 EOF로 종료."""
    store = Store()

    while True:
        try:
            line = input("mini-redis> ")
        except (EOFError, KeyboardInterrupt):
            print()
            break

        tokens = tokenize(line)
        if tokens is None:
            print(ERR_QUOTES)
            continue
        if len(tokens) == 0:
            continue
        if tokens[0].lower() in ("exit", "quit"):
            break

        print(execute(store, tokens, now_ms()))


if __name__ == "__main__":
    main()
