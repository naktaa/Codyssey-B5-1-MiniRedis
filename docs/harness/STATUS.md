# Project Status

## Current Phase

2026-11-24: PY-1·PY-2 Python 포팅 완료(Claude unittest 30개·REPL 실행 확인, 사용자 REPL 일부 실행). C 코드 기준 학습 노트 작성.

## Completed

- 첨부 원문의 문구·순서를 [MISSION.md](MISSION.md)에 보존하고, 추가 제공 화면을 기준으로 제목·번호/중첩 목록·인라인 코드·예시 코드 블록 형식을 복원했습니다.
- 기존 Harness를 유지하며 단일 PM 코칭·사용자 직접 구현·요청 시 리뷰로 조정했습니다.
- 제공된 최신 템플릿의 새 미션 시작·배포/병합 파일 생성 규칙을 [WORKFLOW.md](WORKFLOW.md)에 선별 반영했습니다. 학습형 운영과 기존 원문·기록은 유지했습니다.
- [REQUIREMENTS.md](REQUIREMENTS.md)에 필수·보너스·사용자 추가 조건을 분리하고 [README.md](../../README.md)에 C → Python 학습 계획을 작성했습니다.
- C 실습·Python 최종·테스트 디렉터리와 최소 설정을 준비했습니다. Python 최종 기능 완료 항목은 없습니다.
- C-1의 [LinkedList.c](../../c-practice/LinkedList.c) 여섯 연산을 사용자가 구현했고, 포인터 연결·삭제 시 해제·이동 시 미해제·O(1) 처리를 정적으로 확인했습니다. 실행 검증 완료를 뜻하지 않습니다.
- 사용자 명시 요청·구체적 계획 승인에 따라 기존 [HashMap.c](../../c-practice/HashMap.c)에 Entry/HashMap 구조체·해시/필수 여섯 연산의 원형·역할 설명·TODO만 추가했습니다. 알고리즘 함수 본문·테스트는 구현하지 않았습니다.
- 이후 사용자 구현: [LinkedList.h](../../c-practice/LinkedList.h)로 선언 분리, Node의 void* data·리스트별 HEAD/TAIL 도입, HashMap의 LinkedList* buckets·키/값 Data 분리. 해시 함수의 바이트 변환·31배 누적, put의 순회·동일 키 갱신·신규 Data/키/Node 생성·insert_back 호출을 작성했습니다(실행 미검증).
- 2026-11-21 C-2 완료: 해시맵 6개 연산·로드 팩터 0.75 초과 시 2배 확장 구현. 사용자가 sanitizer 빌드로 확인용 main을 실행해 모든 기대값 일치를 확인했습니다.
- 2026-11-21 C-3 준비: 설계 6개 항목 결정, MinHeap.h/.c에 구조체·원형·역할 주석·TODO 작성(본문·테스트 없음).
- 2026-11-22 C-3 완료: 사용자가 MinHeap 6개 연산(_heapify_up/down 포함)을 구현. Claude 작성 확인용 main([build/tmp/minheap_test.c](../../build/tmp/minheap_test.c), 미추적)을 사용자가 sanitizer 빌드로 실행해 기대값 일치 확인. macOS에서 누수 검사는 미확인.
- 2026-11-22 C-4 LRU 연결: HashMap.h 분리, Data에 lru_node 추가, 해시맵은 LRU를 모르고 [Store.c](../../c-practice/Store.c)(Store { map, lru })가 set/get/del/free에서 LRU를 갱신. 사용자가 hashmap_get_data/free와 Store 4개 함수를 구현, Claude 작성 [store_test.c](../../c-practice/store_test.c)를 사용자가 sanitizer 빌드로 실행해 기대값 일치 확인.
- 2026-11-22 C-4 메모리 제한: Store에 used_memory·maxmemory·evicted_keys와 STORE_OK/ERR_ALLOC/ERR_OOM 추가. 사용자가 entry_size·delete_data·evict_if_needed와 store_set의 OOM 거부·used_memory 갱신·LRU 제거를 구현. store_test.c 메모리 구간(LRU 제거 순서, 갱신 크기 증감, 새 키/덮어쓰기 OOM, maxmemory 경계, del)을 사용자가 sanitizer 빌드로 실행해 기대값 일치 확인. 설계 근거는 WORKLOG "C-4 메모리 제한 설계 결정".
- 2026-11-23 C-4 TTL: STORE_ERR_ALLOC을 -2로, hashmap_put의 새 Data를 calloc으로 변경(사용자 요청으로 Claude 수정). expire_if_needed(힙 peek/pop + 해시맵 expire_at 일치 시만 삭제)·store_expire·store_ttl은 Claude가 보여 준 코드를 사용자가 작성. set/get/del 명령 전 만료 정리, SET 덮어쓰기 TTL 초기화, store_free 힙 해제는 사용자 요청으로 Claude 구현. store_test.c TTL 구간(내림, 오래된 힙 항목, DEL 후 재SET, 덮어쓰기, EXPIRE 0, 만료 GET의 LRU 미갱신, evicted 미집계)을 Claude가 sanitizer 빌드로 실행해 기대값 일치, leaks 0 확인. 사용자도 실행 확인. 설계 근거는 WORKLOG "C-4 TTL 설계 결정".
- 2026-11-24 PY-1: 사용자 요청으로 Claude가 C 연결 리스트·해시맵·최소 힙을 [linked_list.py](../../python/linked_list.py)·[hash_map.py](../../python/hash_map.py)·[min_heap.py](../../python/min_heap.py)로 로직 그대로 포팅. 사용자 결정으로 C식 모듈 함수 대신 원문 이름의 클래스 메서드(put/get/remove/contains/keys/size, push/pop/peek/size, _heapify_up/down)로 작성. 언어 차이: free·할당 실패 분기 제거, out 파라미터→반환값(None), keys()는 출력 대신 count 길이 배열 반환, 힙 개수 필드는 size() 메서드와 겹쳐 count. 해시는 UTF-8 바이트·64비트 마스킹으로 C hashmap_hash와 같은 값임을 Claude가 C 실행 결과와 비교 확인. remove_front·remove_back 단독 호출은 test_linked_list.py로 확인.
- 2026-11-24 PY-2: 사용자 요청으로 Claude가 Store.c를 [store.py](../../python/store.py)로 포팅(entry_size만 미션 공식 len(utf8(key))+len(utf8(value))로 변경, STORE_ERR_ALLOC 제거, del→delete). CLI 명령용으로 C에 없던 exists·dbsize·keys·info_memory 추가(모두 만료 정리 후 조회). [main.py](../../python/main.py) REPL 새로 작성: 명령 10개, 미션 원문 에러 문구 4종, 큰따옴표 값, KEYS는 원문 예시의 `1. "key"` 형식, 출력은 실제 Redis가 아닌 미션 원문 기준. tests/ unittest 30개(C hashmap_test·store_test 시나리오, 미션 8절 예시·에러 예시 포함)를 Claude가 실행해 모두 통과, 미션 예시 REPL 출력 일치 확인. 사용자는 REPL에서 SET 2개 실행 확인(전체 테스트는 미실행).
- 2026-11-24 학습 노트: 사용자 요청으로 Claude가 [study-note.md](../study-note.md)를 사용자가 작성한 C 코드 기준으로 작성(자료구조 3개·Store 결합·Q1~Q8·C→Python 포팅 표·부록). 그림 1(images/store-structure.png)은 자리와 메모만 두었고 이미지는 사용자가 넣음. 코드 링크는 레포 루트 절대 경로 + 시작 줄 앵커(`#L138`) 형식으로 정하고, 미리보기 클릭 시 새 탭·IntelliSense 꺼짐이 없음을 사용자가 확인. 이 규칙을 [CLAUDE.md](../../CLAUDE.md) "문서의 코드 링크"와 [LEARNING_TEMPLATE.md](learning/LEARNING_TEMPLATE.md)에 반영.

## In Progress

없음. (PY-1·PY-2 완료. 다음 단계는 커밋 후 작성)

## Next

커밋 후 작성.

## Open Issues

- 사용자의 전체 unittest·REPL 시나리오 직접 실행은 미확인.
- 보너스 5개 모두 미선택.
- C-3 확인용 build/tmp/minheap_test.c가 지금 없어 C 최소 힙 테스트는 다시 실행할 수 없음(Python test_min_heap.py로만 확인 가능). 학습 노트 그림 1 이미지 미작성.

## Recent Decisions

같은 PM 단일 세션, 에이전트 위임 없음, 사용자 직접 설계·구현·테스트·디버깅, 요청 시에만 같은 세션 정적 리뷰. C 선행 실습과 Python 최종 결과물을 분리하고 C++는 사용하지 않습니다.

## Current Task

- 현재 단계: C-4 2단계 메모리 제한 완료. 별도 TASK 파일 없이 [학습 계획](../../README.md)의 단계 ID를 사용합니다. 아래 항목은 C-2 진행 당시 기록입니다.
- 이번 승인: 문자열 키·정수 값, 항목/관리 구조체, 해시/필수 여섯 연산의 원형·역할 설명·TODO 제공 계획에 사용자가 '그럼 한번 해줘'라고 승인했습니다. 기존 HashMap.c를 사용하며 별도 파일·함수 본문·정답 알고리즘·테스트 코드는 생성하지 않습니다. 자동 구현 위임·실행·Git 쓰기 승인으로 확대하지 않습니다.
- 후속 선택: 기존 연결 리스트를 가져다 쓰는 재사용 방향으로 진행합니다. 이는 설계 방향 선택이며 에이전트의 소스 리팩터링·새 헤더 생성·기존 함수 수정 계획 승인은 아닙니다. 사용자가 직접 설계·구현합니다.
- 후속 설계 요청: 사용자가 범용 데이터 표현을 정해달라고 요청해 void* data와 Node/실제 데이터의 소유권 분리를 선택했습니다. callback·매크로 기반 타입 생성 없이 진행하며 에이전트 소스 수정 승인은 아닙니다. 근거는 [WORKLOG](WORKLOG.md)의 C-2 범용 데이터 표현 결정에 기록했습니다.
- 후속 문서 요청: 미션 원문 형식 복원 및 제공된 최신 Harness 템플릿의 필요한 변경 반영 완료. 새 구현·자동화·모델 설정 변경은 포함하지 않습니다.
- 변경 기준점: 이번 기록 요청 직전 README·STATUS·WORKLOG에 기존 변경이 있었고 HashMap.c는 미추적이었습니다. 사용자 소스는 그대로 두고 기존 문서의 현재 상태만 갱신했습니다.
- Review Decision: **SKIP** — 이번 요청은 중간 진행 기록입니다. 파일을 정적으로 확인했으며 사용자 코드의 완료 판정·자동 리뷰·별도 평가·위임은 하지 않았습니다.
- 확인 범위: 파일/diff·원형·주석·문서 링크의 정적 확인. 컴파일·실행·테스트는 하지 않았습니다.
- 실제 런타임 모델·Reasoning·속도: 미확인·설정 변경 없음. 문서 기본값과 실제 설정을 동일시하지 않습니다.
- Git 운영: 사용자 후속 지침에 따라 스테이징·커밋은 항상 사용자 직접 수행입니다. PM은 커밋 범위·메시지만 추천하며 실행·권한 요청을 하지 않습니다. 이전 Git 시도 실패 이력은 [WORKLOG.md](WORKLOG.md)에 보존했습니다.
