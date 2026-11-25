# Mini Redis 시연 시나리오

[demo.py](/scripts/demo.py)는 미리 정한 명령을 엔터를 칠 때마다 하나씩 실행하고, 실행 전후의 Redis 내부 상태(LRU 순서, TTL, 메모리, 힙 항목 수, 구독 채널)를 한 화면에 보여 줍니다. 명령 1개가 TASK 1개이고, 시나리오 5개에 TASK는 모두 52개입니다. 결과는 기대값과 비교해 판정합니다.

명령은 [main.py](/python/main.py)의 `tokenize`·`execute`로 실행합니다. CLI에 직접 입력했을 때와 파싱·출력이 같고, 같은 프로세스 안이라 Store 내부를 바로 읽어 화면에 그릴 수 있습니다. REPL 루프가 처리하는 종료(`quit`)만 실제 `main.py`를 띄워 exit code로 확인합니다.

## 실행

```bash
python3 scripts/demo.py                               # 엔터로 한 단계씩
python3 scripts/demo.py --auto                        # 끝까지 자동
python3 scripts/demo.py --auto | tee build/demo.log   # 한 줄 로그로 저장
```

| 입력 | 동작 |
| --- | --- |
| Enter | 다음 화면으로 (실행 전 → 실행 후 → 다음 TASK) |
| `a` + Enter | 나머지를 끝까지 자동 실행 (화면마다 0.5초) |
| Ctrl+C | 그 자리에서 중단 (exit code 130) |

- TASK 하나에 엔터를 두 번 칩니다. 첫 엔터는 실행, 두 번째 엔터는 결과를 본 뒤 다음 TASK로 넘어갑니다.
- 모두 통과하면 exit code 0, 하나라도 틀리면 1입니다.
- 입력이 없는 환경(`< /dev/null`)에서는 자동 모드로 넘어갑니다.
- 출력이 터미널이 아니면(파이프·파일) 화면을 지우지 않고, 엔터도 기다리지 않고, TASK마다 한 줄씩 기록합니다.
- 자동 모드 전체 시간은 터미널에서 약 45초(화면 82개 × 0.5초 + 3초 대기), 파이프에서 약 4초입니다.

## 화면 읽는 법

TASK-027 `TTL user:2`를 실행한 뒤의 화면입니다.

```text
[TTL (힙 기반 만료)]
TASK-027  남은 초 (내림). 엔터가 늦어도 100초 안이면 통과
  mini-redis> TTL user:2
  (integer) 97



────────────────────────────────────────────────────────────────────────
(실행 후)
Redis 상태   used_memory 9 / maxmemory 0(무제한)   evicted_keys 0   힙 항목 1
  LRU 순서: 최근 → 오래됨
  1. user:2   "Bob"        9B   TTL 97초
────────────────────────────────────────────────────────────────────────
```

- **첫 줄**: 시나리오 제목입니다. 시나리오의 첫 TASK에는 `빈 Redis에서 시작`이 붙습니다. 시나리오마다 새 Store로 시작합니다.
- **TASK 줄**: 이 명령으로 확인할 점
- **`mini-redis>` 줄과 출력 칸**: 실행한 명령과 프로그램이 돌려준 출력입니다. 칸 높이는 가장 긴 출력(PUBLISH 결과 + 받은 메시지 4줄)에 맞춰 고정되어 있어, 화면이 바뀌어도 아래 상태 칸이 흔들리지 않습니다.
- **판정**: 기대값과 같으면 아무것도 표시하지 않고, 다를 때만 빨간 `✗ 기대: ...` 줄이 나옵니다.
- **상태 칸**: `(실행 전)`·`(실행 후)` 시점의 Store 내부입니다.
  - 키는 LRU 순서(최근 → 오래됨)로 나옵니다. maxmemory가 있으면 TAIL에 `← 다음 제거 대상`이 붙습니다.
  - 만료 시각이 지났지만 아직 정리되지 않은 키는 `만료됨 (다음 명령 때 정리)`로 보입니다.
- **마지막 화면**: `결과: 52 / 52 통과`와 실패한 TASK 번호

파이프로 저장한 로그는 TASK마다 한 줄입니다. 실패한 TASK에만 `기대:` 줄이 붙습니다.

```text
✓ TASK-013 [메모리 제한 + LRU 제거 (미션 8절 예시)] SET user:3 "Charlie" → OK
```

## 천천히 진행해도 결과가 같은 이유

| 상황 | 방식 |
| --- | --- |
| 남은 초 읽기 (TASK-026·027) | EXPIRE를 100초로 걸고 TTL이 0~100이면 통과. 설명하느라 엔터가 늦어도 100초 안이면 결과가 같음 |
| 실제 만료 보기 (TASK-028~030) | EXPIRE를 3초로 다시 건 뒤 대기 TASK가 3.2초를 실제로 기다림. 엔터를 늦게 칠수록 더 확실히 만료됨 |
| 그 밖의 명령 | 시간과 무관해 언제 쳐도 같음 |

TASK-025~028 화면마다 엔터를 5초씩 늦게 친 경우에도 통과를 확인했습니다(당시 41개 기준. TTL 시나리오 TASK 번호는 그대로).

## 시나리오 흐름

| # | TASK | 시나리오 | 보여 주는 것 | 미션 요구 |
| --- | --- | --- | --- | --- |
| 1 | 001-009 | String 명령과 Redis 스타일 출력 | SET·GET, 큰따옴표 값과 따옴표 없는 값, EXISTS, DBSIZE, KEYS, DEL 1/0, 삭제 후 (nil) | 4-2 String 명령, 4-5 값 파싱 |
| 2 | 010-021 | 메모리 제한 + LRU 제거 (미션 8절 예시) | 33 > 30이라 user:1 제거 → INFO 22/30/1, GET으로 user:2 갱신 → 한글 키 저장 시 user:3 제거, 단일 엔트리 OOM | 4-3 메모리 관리, 4-2 GET 시 LRU 갱신, 8 결과 예시 |
| 3 | 022-037 | TTL (힙 기반 만료) | TTL -1·-2, 없는 키 EXPIRE 0, TTL 남은 초, EXPIRE 재설정과 힙에 남는 옛 기록, 만료 후 GET (nil)·TTL -2, SET 덮어쓰기 시 TTL 초기화, EXPIRE 0 즉시 삭제 | 4-4 TTL 관리와 엣지 케이스 |
| 4 | 038-048 | Pub/Sub (보너스 5) | 구독 전 발행 0, SUBSCRIBE 채널 수(중복 구독은 그대로), 구독한 채널 발행 시 1 + 받은 메시지 3줄, 구독 안 한 채널 0, 메시지는 DBSIZE·used_memory에 안 들어감, 인자 개수 오류 | 5-5 보너스 Pub/Sub |
| 5 | 049-052 | 에러 처리와 종료 | unknown command, 인자 개수, 정수 파싱 실패, quit 후 exit code 0 | 4-5 에러 출력·CLI 종료 |

미션 요구 칸의 번호는 [MISSION.md](/docs/harness/MISSION.md)의 "4. 기능 요구 사항" 소절 번호입니다(예: 4-3 = 3. 메모리 관리 + LRU 자동 제거). 5-5는 "5. 보너스 과제"의 5번입니다.

## 시나리오별 핵심 TASK

**[2] 메모리 제한 + LRU**: 미션 예시 숫자에 이어, GET 하나로 제거 대상이 바뀌는 것까지 보여 줍니다. 상태 칸의 `← 다음 제거 대상` 표시가 GET 뒤에 user:3으로 옮겨 갑니다.

```text
SET user:1 "Alice"     11   used 11   lru: HEAD -> [user:1] <- TAIL
SET user:2 "Bob"        9   used 20   lru: HEAD -> [user:2] <-> [user:1] <- TAIL
SET user:3 "Charlie"   13   used 33 > 30 → TAIL(user:1) 제거 → used 22, evicted 1
                                      lru: HEAD -> [user:3] <-> [user:2] <- TAIL
GET user:2                            lru: HEAD -> [user:2] <-> [user:3] <- TAIL
SET 이름 "철수"         12   used 34 > 30 → TAIL(user:3) 제거 → used 21, evicted 2
SET huge (29자리)       33   단일 엔트리 > 30 → OOM, 아무것도 바뀌지 않음
```

**[3] TTL**: 상태 칸의 `힙 항목` 수로 lazy deletion을 볼 수 있습니다.

| TASK | 명령 | 힙 항목 | 보이는 것 |
| --- | --- | --- | --- |
| 026 | EXPIRE user:2 100 | 1 | 100초 기록 추가 |
| 028 | EXPIRE user:2 3 | 2 | 옛 100초 기록은 지우지 않고 3초 기록을 더함 |
| 029 | 대기 3.2초 | 2 | 키가 `만료됨 (다음 명령 때 정리)`로 보이지만 아직 남아 있음 |
| 030 | GET user:2 | 1 | 명령 전 정리로 3초 기록을 꺼내 키 삭제. 100초 기록은 아직 때가 안 되어 남음 |
| 033-034 | EXPIRE a 100 → SET a 2 | 2 → 2 | SET 덮어쓰기로 TTL은 사라져도 힙 기록은 남음 |

**[4] Pub/Sub**: REPL 자신이 유일한 구독자입니다. 서버나 터미널 2개 없이, 구독한 채널로 발행하면 받은 메시지를 발행 결과 바로 뒤에 보여 줍니다. 상태 칸에는 `구독 채널 N개: ...`가 나옵니다.

```text
PUBLISH news "hello"     구독 전 → (integer) 0, 메시지는 버려짐
SUBSCRIBE news           channels(HashMap)에 news 추가 → 구독 채널 수 1
SUBSCRIBE news           이미 있는 키 → put이 count를 늘리지 않음 → 그대로 1
PUBLISH news "hello"     contains(news) → inbox(LinkedList) 뒤에 넣음 → (integer) 1
                         → inbox 앞에서 꺼내 1) "message" 2) "news" 3) "hello" 출력
DBSIZE / INFO memory     메시지는 Store 밖에 있어 0, used_memory 0
```

## 코드 위치

| 내용 | 코드 |
| --- | --- |
| KEYS 판정(번호 형식 + 순서 무관 비교) | [L33-42](/scripts/demo.py#L33) |
| SUBSCRIBE·PUBLISH 기대 출력 | [L45-52](/scripts/demo.py#L45) |
| TTL 범위 판정 | [L55-59](/scripts/demo.py#L55) |
| 시나리오 목록(명령·설명·기대값) | [L64-127](/scripts/demo.py#L64) |
| 명령 실행·대기·quit 확인 | [L136-148](/scripts/demo.py#L136) |
| 상태 칸(LRU 순서·TTL·메모리·구독 채널) | [L162-186](/scripts/demo.py#L162) |
| 한 화면 그리기(출력 칸 높이 고정) | [L202-225](/scripts/demo.py#L202) |
| 엔터·a·EOF 처리 | [L239-247](/scripts/demo.py#L239) |
| TASK 진행·결과 요약 | [L250-281](/scripts/demo.py#L250) |

시나리오를 더하려면 SCENARIOS에 `(명령, 설명, 기대값)` 한 줄을 추가합니다.
- 기대값이 문자열이면 출력과 그대로 비교하고, `keys(...)`나 `between(lo, hi)`면 그 함수로 판정합니다.
- 대기는 명령 자리에 초를 숫자로 씁니다. 예: `(3.2, "3초 대기", None)`
