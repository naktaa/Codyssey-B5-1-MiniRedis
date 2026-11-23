# Mini Redis

자료구조를 직접 구현하며 Redis의 LRU·TTL·메모리 제한 동작을 배우는 CLI 프로젝트입니다. C 실습을 먼저 진행하고, 직접 만든 알고리즘을 Python으로 포팅해 최종 프로그램을 완성합니다. C 실습(연결 리스트·체이닝 해시맵·최소 힙·LRU/TTL/메모리 제한 결합)을 완료했고, 자료구조를 Python으로 포팅하는 PY-1 단계입니다.

원문은 [MISSION.md](docs/harness/MISSION.md), 필수·보너스 완료 기준은 [REQUIREMENTS.md](docs/harness/REQUIREMENTS.md), 현재 단계는 [STATUS.md](docs/harness/STATUS.md)에 있습니다. [AGENTS.md](AGENTS.md)에 따라 사용자가 설계·구현·테스트·디버깅하고 PM은 같은 세션에서 질문과 학습을 돕습니다. 자동 위임·자동 리뷰·별도 평가는 하지 않습니다.

## 기본 구조와 환경

```text
.
├── AGENTS.md
├── README.md
├── .editorconfig
├── .gitignore
├── c-practice/          # LinkedList.h/.c, HashMap.c, MinHeap.h/.c 실습
├── python/             # Python 최종 Mini Redis (현재 .gitkeep만 존재)
├── tests/              # 사용자 작성 테스트 (현재 .gitkeep만 존재)
└── docs/harness/        # 기존 지침·원문·요구사항·상태·이력·템플릿
```

- 최종 환경: Python 3.8 이상. C 실습에는 C 컴파일러가 필요하며 C++는 사용하지 않습니다.
- 각 자료구조는 사용자가 독립 모듈/파일로 분리합니다. 세부 파일명·C 표준·빌드 명령·테스트 방식은 아직 결정하지 않았습니다.
- 외부 dependency·패키징·CI·자동 실행 설정은 추가하지 않았습니다. 컴파일·프로그램 실행·테스트·도구 설치는 수행하지 않았습니다.
- 생성물은 루트 `build/`에 두는 기본 정리 방식을 설정했습니다. C 실습은 [LinkedList.h](c-practice/LinkedList.h)·[LinkedList.c](c-practice/LinkedList.c)·[HashMap.c](c-practice/HashMap.c)·[MinHeap.h](c-practice/MinHeap.h)·[MinHeap.c](c-practice/MinHeap.c)에 있습니다. 해시맵은 레포 루트에서 `gcc -Wall -Wextra -g -fsanitize=address,undefined -o build/hashmap c-practice/HashMap.c c-practice/LinkedList.c`로 빌드합니다.

## 단계별 학습 계획

학습의 중심은 C 단계(C-1~C-4)에서 자료구조를 직접 설계·구현하는 것이며, 요구사항 확인 → 사용자 설계 → 사용자 구현 → 사용자 테스트 → 요청 시 리뷰 순서입니다. PY 단계는 새로 설계하지 않고 사용자가 작성한 C 코드를 구조·이름·로직 그대로 Python으로 포팅해 미션 최종 결과물을 만듭니다. 언어 차이로 꼭 필요한 부분만 바꾸고, C에 없던 CLI(명령 파싱·출력)만 새로 작성합니다. 완료 결과와 이해한 내용을 공유한 뒤 사용자의 의사에 따라 다음 단계로 진행합니다. 아래는 목표와 관찰할 기준이며 세부 설계·구현 해법을 정하지 않습니다.

| 단계 | 위치 | 목표·완료 기준 | 상태 |
| --- | --- | --- | --- |
| C-1 | [c-practice](c-practice/) | 이중 연결 리스트: 원문의 6개 연산, 삽입/삭제/이동 O(1), 빈/단일/복수 노드에서 연결·수명·해제를 사용자 테스트로 확인·설명 | 완료 (remove_front·remove_back 단독 호출 미확인) |
| C-2 | [HashMap.c](c-practice/HashMap.c) | 직접 해시 함수와 체이닝 해시맵: 원문의 6개 연산, 충돌 처리, 로드 팩터 0.75 초과 시 버킷 2배 확장 후 기존 항목 보존 확인·설명 | 완료 (사용자 실행 확인) |
| C-3 | [MinHeap.h](c-practice/MinHeap.h)·[MinHeap.c](c-practice/MinHeap.c) | 최소 힙: push/pop/peek/size와 상·하향 정렬, 만료 시간·키 요소 지원 및 최소값 순서 확인·설명 | 완료 (사용자 실행 확인) |
| C-4 | [c-practice](c-practice/) | 기존 자료구조에 LRU·TTL·메모리 제한 결합: 갱신·삭제·만료·제거 시 일관성, 사용량·제거 횟수와 메모리 해제 확인·설명. C 전체 CLI는 요구하지 않음 | 완료 (사용자 실행 확인) |
| PY-1 | [python](python/), [tests](tests/) | C의 연결 리스트·체이닝 해시맵·최소 힙 코드를 그대로 독립 모듈(`linked_list.py`·`hash_map.py`·`min_heap.py`)로 포팅하고 기존 성질·연산·제약을 `tests/`의 unittest로 확인 | 진행 중 |
| PY-2 | [python](python/) | `store.py`(Store.c 그대로 포팅)와 `main.py`(새로 작성하는 CLI). String 6개·메모리 2개·TTL 2개 명령을 LRU/TTL/메모리 관리와 결합해 REQ-004~007 동작 확인 | 대기 |
| PY-3 | [python](python/), [tests](tests/) | REPL·오류·값 파싱과 통합 테스트: 모든 필수 기준·경계 동작을 실제 입력/출력으로 확인하고 원문의 4개 학습 목표를 자기 코드로 설명 | 대기 |

동적 배열 등 보너스 5개는 필수 완료 후 사용자 선택으로만 진행하며 이 계획의 선행 조건으로 넣지 않습니다.

## 사용자가 정할 주요 항목

- C 노드·문자열의 소유권과 수명, 할당 실패 처리, API·파일명·빌드/테스트 방식.
- 해시 함수·초기 버킷 수·체이닝 표현, 최소 힙 저장소·확장 방식. 동적 배열 직접 구현 보너스의 채택 여부는 별개입니다.
- 자료구조 간 참조와 삭제 책임, LRU 갱신 범위, TTL 갱신·초기화·삭제 후 힙 엔트리 처리 방식.
- 시간 측정·남은 초의 표현, DBSIZE/KEYS 등 전체 조회 시 만료 정리 범위, 메모리 제한 변경 시 초과분 처리 시점과 OOM 시 기존 값 보존 정책. 원문에 명시된 동작을 우선하고 미명시 사항은 결정 근거를 남깁니다.
- Python 배열 수준 저장소의 허용 범위와 값 파싱 방식(원문의 최소 기준 중 선택). 핵심 자료구조를 dict/set/collections/heapq로 대체하지 않습니다.

C-2는 [LinkedList.h](c-practice/LinkedList.h)·[LinkedList.c](c-practice/LinkedList.c)의 범용 리스트를 재사용해 완료했습니다. C-3 최소 힙은 밀리초 `long long` 만료 시각, 힙이 복사해 소유하는 key, 2배 동적 확장으로 설계했으며 결정 근거는 [WORKLOG](docs/harness/WORKLOG.md)에 있습니다. 남은 작업은 [STATUS](docs/harness/STATUS.md)에 기록합니다.
