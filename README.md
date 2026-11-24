# Mini Redis

자료구조를 직접 구현하며 Redis의 LRU·TTL·메모리 제한 동작을 배우는 CLI 프로젝트입니다. C 실습에서 연결 리스트·체이닝 해시맵·최소 힙·LRU/TTL/메모리 제한 결합을 직접 구현했고, 그 코드를 Python으로 포팅해 최종 CLI 프로그램을 완성했습니다.

원문은 [MISSION.md](docs/harness/MISSION.md), 필수·보너스 완료 기준은 [REQUIREMENTS.md](docs/harness/REQUIREMENTS.md), 현재 단계는 [STATUS.md](docs/harness/STATUS.md)에 있습니다. 구현 설명은 학습 노트 [study-note.md](docs/study-note.md)에 C 코드 기준으로 정리했고, 기능 시연 흐름은 [demo-scenario.md](docs/demo-scenario.md)에 있습니다. 진행 방식은 [CLAUDE.md](CLAUDE.md)를 따르며, [AGENTS.md](AGENTS.md)와 `docs/harness/`의 나머지 절차 문서는 Codex용 기록입니다.

## 기본 구조와 환경

```text
.
├── CLAUDE.md            # Claude Code 진행 지침
├── AGENTS.md            # Codex용 기록
├── README.md
├── c-practice/          # C 실습: LinkedList, HashMap, MinHeap, Store (.h/.c), 확인용 hashmap_test.c, store_test.c
├── python/              # Python 최종 Mini Redis: linked_list.py, hash_map.py, min_heap.py, store.py, main.py
├── tests/               # unittest: test_linked_list.py, test_hash_map.py, test_min_heap.py, test_store.py, test_main.py
├── scripts/demo.py      # 엔터 단계형 시연 (41 TASK, 내부 상태 표시)
└── docs/
    ├── study-note.md    # 학습 노트 (C 코드 기준)
    ├── demo-scenario.md # 시연 시나리오 흐름과 화면 읽는 법
    └── harness/         # 원문·요구사항·상태·이력·템플릿
```

- 최종 환경: Python 3.8 이상, 외부 dependency 없음. C 실습에는 C 컴파일러가 필요하며 C++는 사용하지 않습니다.
- 빌드 생성물은 루트 `build/`에 둡니다(git 추적 안 함).

## 실행과 테스트

레포 루트에서 실행합니다.

```bash
# Python CLI
python3 python/main.py

# Python 테스트 (30개)
python3 -m unittest discover -s tests -v

# 시연: 엔터로 한 단계씩 (a: 끝까지 자동, Ctrl+C: 중단) / --auto: 끝까지 자동
python3 scripts/demo.py

# C 확인용 main (sanitizer)
mkdir -p build
gcc -Wall -Wextra -g -fsanitize=address,undefined -o build/hashmap c-practice/hashmap_test.c c-practice/HashMap.c c-practice/LinkedList.c
gcc -Wall -Wextra -g -fsanitize=address,undefined -o build/store c-practice/store_test.c c-practice/Store.c c-practice/HashMap.c c-practice/LinkedList.c c-practice/MinHeap.c
./build/store

# C 누수 확인 (macOS는 LeakSanitizer 미지원이라 sanitizer 없이 빌드)
gcc -Wall -Wextra -g -o build/store c-practice/store_test.c c-practice/Store.c c-practice/HashMap.c c-practice/LinkedList.c c-practice/MinHeap.c
leaks --atExit -- ./build/store
```

지원 명령: `SET` `GET` `DEL` `EXISTS` `DBSIZE` `KEYS` · `CONFIG SET maxmemory` `INFO memory` · `EXPIRE` `TTL` · `exit`/`quit`. 출력과 에러 문구는 실제 Redis가 아니라 미션 원문 기준입니다(예: `KEYS` 출력은 `1. "key"`).

```text
mini-redis> CONFIG SET maxmemory 30
OK
mini-redis> SET user:1 "Alice"
OK
mini-redis> SET user:2 "Bob"
OK
mini-redis> SET user:3 "Charlie"
OK
mini-redis> GET user:1
(nil)
mini-redis> INFO memory
used_memory:22
maxmemory:30
evicted_keys:1
```

## 단계별 학습 계획

C 단계(C-1~C-4)에서 자료구조를 직접 설계·구현했고, PY 단계는 그 C 코드를 구조·로직 그대로 Python으로 옮겼습니다. 언어 차이로 꼭 필요한 부분과 C에 없던 CLI(명령 파싱·출력)만 새로 작성했습니다.

| 단계 | 위치 | 목표·완료 기준 | 상태 |
| --- | --- | --- | --- |
| C-1 | [LinkedList.c](c-practice/LinkedList.c) | 이중 연결 리스트: 원문의 6개 연산, 삽입/삭제/이동 O(1), 연결·수명·해제 | 완료 (remove_front·remove_back 단독 호출은 Python 테스트로 확인) |
| C-2 | [HashMap.c](c-practice/HashMap.c) | 직접 해시 함수와 체이닝 해시맵: 원문의 6개 연산, 충돌 처리, 로드 팩터 0.75 초과 시 2배 확장 | 완료 (사용자 실행 확인) |
| C-3 | [MinHeap.c](c-practice/MinHeap.c) | 최소 힙: push/pop/peek/size와 상·하향 정렬, (expire_at, key) 요소 | 완료 (사용자 실행 확인) |
| C-4 | [Store.c](c-practice/Store.c) | LRU·TTL·메모리 제한 결합: 갱신·삭제·만료·제거 시 일관성, used_memory·evicted_keys, 메모리 해제 | 완료 (사용자 실행 확인, leaks 0) |
| PY-1 | [python](python/), [tests](tests/) | 연결 리스트·해시맵·최소 힙을 독립 모듈로 포팅, 원문 이름의 클래스 메서드, unittest 확인 | 완료 (Claude 실행 확인, 사용자 미검증) |
| PY-2 | [store.py](python/store.py), [main.py](python/main.py) | Store.c 포팅(used_memory는 원문 UTF-8 공식)과 CLI. 명령 10개, 에러 4종, 큰따옴표 값 | 완료 (Claude 실행 확인, 사용자 REPL 일부 실행) |
| PY-3 | [python](python/), [tests](tests/) | 사용자가 전체 테스트·REPL 시나리오를 직접 확인하고 원문의 4개 학습 목표를 자기 코드로 설명 | 대기 |

보너스 5개는 선택하지 않았습니다.

## 주요 설계 결정

- **소유권**: Node는 리스트가, Data와 key 문자열은 해시맵이 해제합니다. 힙은 key를 복사해 소유하고, pop한 key는 호출한 쪽이 해제합니다.
- **LRU**: Data가 LRU 노드 주소(lru_node)를 직접 가져 GET·SET 갱신과 TAIL 제거가 O(1)입니다. 해시맵은 LRU를 모르고 Store가 갱신합니다.
- **TTL**: 밀리초 만료 시각을 최소 힙에 넣고, 꺼낼 때 해시맵의 현재 만료 시각과 같을 때만 지웁니다(lazy deletion). 모든 명령 전에 만료를 정리합니다.
- **메모리 제한**: 단일 엔트리가 maxmemory를 넘으면 저장하지 않고 OOM을 돌려줍니다. 저장 후 초과하면 TAIL부터 제거하고 evicted_keys를 셉니다. 만료로 지운 키는 세지 않습니다.
- **Python 제약**: dict·set·collections·heapq·내장 hash()를 쓰지 않고, list는 고정 길이 배열과 인덱스 접근으로만 씁니다.

설계 근거는 [WORKLOG](docs/harness/WORKLOG.md), 남은 작업은 [STATUS](docs/harness/STATUS.md)에 있습니다.
