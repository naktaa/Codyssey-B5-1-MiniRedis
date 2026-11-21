# Project Status

## Current Phase

2026-11-21: C-2 해시맵 완료(사용자 실행 확인). C-3 최소 힙 설계를 결정했고 [MinHeap.h](../../c-practice/MinHeap.h)·[MinHeap.c](../../c-practice/MinHeap.c)에 구조체·원형·TODO를 준비했습니다. 함수 본문은 사용자가 구현합니다.

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

## In Progress

C-3 최소 힙: 설계 결정·선언 템플릿 준비 완료, 함수 본문 구현 대기. 결정 근거는 [WORKLOG](WORKLOG.md)의 "C-3 최소 힙 설계 결정"에 있습니다.

## Next

사용자가 [MinHeap.c](../../c-practice/MinHeap.c)에 init/free → _heapify_up/push → peek/size → _heapify_down/pop 순서로 구현합니다. 이후 Claude가 확인용 main(무작위 push 후 전부 pop해 오름차순 확인)을 작성하고 사용자가 실행합니다.

## Open Issues

- C-1 연결 리스트는 단독 테스트 없이 C-2 해시맵 실행으로 insert_back·remove_node 경로만 확인됐습니다. insert_front·remove_back·move_to_front는 C-4 LRU에서 확인합니다.
- HashMap.c 21~22행 주석에 예전 Entry** 설명이 남아 있습니다(현재 구조는 LinkedList* buckets).
- C-4 메모: lazy deletion을 위해 해시맵 Data에 expire_at 필드 추가 예정.
- Python 최종 기능 미구현. 보너스 5개 모두 미선택.

## Recent Decisions

같은 PM 단일 세션, 에이전트 위임 없음, 사용자 직접 설계·구현·테스트·디버깅, 요청 시에만 같은 세션 정적 리뷰. C 선행 실습과 Python 최종 결과물을 분리하고 C++는 사용하지 않습니다.

## Current Task

- 현재 단계: C-3 최소 힙 구현 대기(설계·선언 템플릿 준비 완료). 별도 TASK 파일 없이 [학습 계획](../../README.md)의 단계 ID를 사용합니다. 아래 항목은 C-2 진행 당시 기록입니다.
- 이번 승인: 문자열 키·정수 값, 항목/관리 구조체, 해시/필수 여섯 연산의 원형·역할 설명·TODO 제공 계획에 사용자가 '그럼 한번 해줘'라고 승인했습니다. 기존 HashMap.c를 사용하며 별도 파일·함수 본문·정답 알고리즘·테스트 코드는 생성하지 않습니다. 자동 구현 위임·실행·Git 쓰기 승인으로 확대하지 않습니다.
- 후속 선택: 기존 연결 리스트를 가져다 쓰는 재사용 방향으로 진행합니다. 이는 설계 방향 선택이며 에이전트의 소스 리팩터링·새 헤더 생성·기존 함수 수정 계획 승인은 아닙니다. 사용자가 직접 설계·구현합니다.
- 후속 설계 요청: 사용자가 범용 데이터 표현을 정해달라고 요청해 void* data와 Node/실제 데이터의 소유권 분리를 선택했습니다. callback·매크로 기반 타입 생성 없이 진행하며 에이전트 소스 수정 승인은 아닙니다. 근거는 [WORKLOG](WORKLOG.md)의 C-2 범용 데이터 표현 결정에 기록했습니다.
- 후속 문서 요청: 미션 원문 형식 복원 및 제공된 최신 Harness 템플릿의 필요한 변경 반영 완료. 새 구현·자동화·모델 설정 변경은 포함하지 않습니다.
- 변경 기준점: 이번 기록 요청 직전 README·STATUS·WORKLOG에 기존 변경이 있었고 HashMap.c는 미추적이었습니다. 사용자 소스는 그대로 두고 기존 문서의 현재 상태만 갱신했습니다.
- Review Decision: **SKIP** — 이번 요청은 중간 진행 기록입니다. 파일을 정적으로 확인했으며 사용자 코드의 완료 판정·자동 리뷰·별도 평가·위임은 하지 않았습니다.
- 확인 범위: 파일/diff·원형·주석·문서 링크의 정적 확인. 컴파일·실행·테스트는 하지 않았습니다.
- 실제 런타임 모델·Reasoning·속도: 미확인·설정 변경 없음. 문서 기본값과 실제 설정을 동일시하지 않습니다.
- Git 운영: 사용자 후속 지침에 따라 스테이징·커밋은 항상 사용자 직접 수행입니다. PM은 커밋 범위·메시지만 추천하며 실행·권한 요청을 하지 않습니다. 이전 Git 시도 실패 이력은 [WORKLOG.md](WORKLOG.md)에 보존했습니다.
