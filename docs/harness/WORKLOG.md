# Worklog

실제 중요 작업·결정만 기록합니다. 정적 확인은 실행·테스트 성공을 뜻하지 않습니다.

## 2026-10-30

### 초기 chore — 학습형 Mini Redis 준비

작업:
- 사용자 요청 범위인 원문 보존·기존 지침 확인·학습형 운영 전환·완료 기준·C → Python 계획·상태 초기화·빈 디렉터리/최소 설정만 반영했습니다.
- [MISSION.md](MISSION.md)에 첨부 텍스트를 그대로 복사했습니다. 해석은 [REQUIREMENTS.md](REQUIREMENTS.md), 학습 순서는 [README.md](../../README.md), 현재 상태는 [STATUS.md](STATUS.md)에 분리했습니다.
- [AGENTS.md](../../AGENTS.md)·[PM 지침](prompts/PM.md)·[Reviewer 지침](prompts/REVIEWER.md)·[WORKFLOW.md](WORKFLOW.md)를 학습형으로 조정하고 [Implementer 지침](prompts/IMPLEMENTER.md)은 비활성 표시 후 보존했습니다. [모델 정책](MODEL_POLICY.md)도 실제 런타임 설정과 문서 기본값을 구분했습니다.
- [c-practice](../../c-practice/)·[python](../../python/)·[tests](../../tests/)에는 .gitkeep만 두었습니다. [.editorconfig](../../.editorconfig)·[.gitignore](../../.gitignore)를 추가했습니다. 핵심 소스·스켈레톤·테스트 코드·실행 자동화는 만들지 않았습니다.

실제 확인된 결과:
- 초기 복사 당시 정적 확인: 원문과 보존본 바이트 동일. 이후 아래 형식 복원 작업에서 Markdown 표기를 추가했습니다.
- 정적 확인: 작성 문서의 상대 링크 대상 존재, C/Python/테스트 디렉터리에 .gitkeep만 존재.
- 작업 전 AGENTS.md·docs/는 미추적 템플릿이었고 최초 커밋도 없었습니다. 기존 이력·사용자 소스·실행 설정을 삭제하거나 초기화하지 않았습니다.
- 컴파일·프로그램 실행·테스트·환경 설치·에이전트 위임·Git 쓰기를 수행하지 않았습니다. 사용자 실행 결과는 없습니다.

Review:
- Review Decision: **SKIP**. 문서·최소 초기 설정만 변경했고 사용자 요청에 따라 자동 리뷰·별도 평가를 하지 않았습니다. 리뷰 결과 파일 없음.

### Decision

- Decision: 기존 Harness 경로·템플릿을 유지하며 단일 PM 학습 코칭으로 운영합니다. 원문은 기존 MISSION 한 곳, 학습 계획은 README 한 곳에 둡니다.
- Reason: 사용자 직접 설계·구현·테스트·디버깅과 불필요한 문서/자동화 방지가 이번 요청의 목적입니다.
- Alternative: 자동 Implementer/Reviewer 흐름, 별도 Roadmap·단계별 TASK·학습 답안·빌드/테스트 자동화 선생성.
- Why not: 사용자 요청 범위를 넘고 세부 설계·학습 선택을 선점합니다. C 실습은 별도 위치에 두고 최종 Python 요구를 대체하지 않습니다.

남은 문제·다음:
- 기능·실행 환경은 미검증입니다. 보너스는 미선택이며 설계 미정 항목은 REQUIREMENTS/README에 남겼습니다.
- 사용자가 C-1부터 직접 시작합니다. 다음 단계·리뷰는 사용자 의사에 따릅니다.
- 커밋 권장 범위: 이번 초기 Harness·문서·최소 설정 일괄. 제안 메시지: `Chore: Mini Redis 학습형 초기 환경 구성`. 실제 커밋하지 않았습니다.

### 미션 원문 형식 복원

- 사용자 추가 화면을 기준으로 [MISSION.md](MISSION.md)의 1~8절 제목, 번호/중첩 목록, 명령·메서드의 인라인 코드, 예시 코드 블록과 구분선을 복원했습니다.
- 원문 문구·순서를 유지했으며 Markdown 표기를 제외한 전체 비어 있지 않은 줄의 동일성을 정적으로 확인했습니다. 형식 표기 추가로 초기 첨부 파일과의 바이트 동일성은 더 이상 현재 상태 기준이 아닙니다.
- [STATUS.md](STATUS.md)의 원문 보존·확인 범위 설명을 현재 형식에 맞췄습니다. 기능 소스·학습 단계·요구사항은 변경하지 않았습니다.
- Review Decision: **SKIP** — 원문 서식·관련 상태 기록만 수정. 자동 리뷰·테스트·위임 없음.

### 최신 Harness 템플릿 선별 반영

- 사용자가 제공한 최신 템플릿 13개 문서를 최초 적용 템플릿과 비교했습니다. 새 차이는 WORKFLOW의 '새 미션과 세션 복구' 절뿐이었으며 다른 12개 문서는 동일했습니다.
- [WORKFLOW.md](WORKFLOW.md)에 새 프로젝트용 기본 배포 구성·빈 기록 시작·프로젝트별 최소 제약 추가·명시 요청 시에만 병합 안내/패치 파일 생성 규칙을 반영했습니다. 진행 중인 Mini Redis 기록을 초기화하지 않는 경계를 명시했습니다.
- [STATUS.md](STATUS.md)에 반영 완료를 기록했습니다. 미션 원문·요구사항·학습형 운영·비활성 역할·소스/설정은 보존하고, 원본 Downloads 템플릿은 수정하지 않았습니다.
- 정적 확인: 최신 템플릿 차이와 반영 범위 대조. 컴파일·프로그램 실행·테스트·위임·Git 쓰기는 수행하지 않았습니다.
- Review Decision: **SKIP** — 운영 문서·상태 기록만 수정했습니다. 커밋은 초기 chore의 Harness·문서 변경과 함께 묶는 것을 권장합니다.

## 2026-10-31

### C-1 구현 정리·요청 검토와 C-2 설계 안내

- 사용자 요청: 연결 리스트 마무리 가능 여부를 검토하고 괜찮으면 관련 변경을 커밋한 뒤 다음 작업으로 진행.
- 검토 대상: [Linked_list.c](../../c-practice/Linked_list.c)의 Node, insert_front/insert_back/remove_front/remove_back/remove_node/move_to_front와 caeaf11 이후 diff. 기존 합의대로 임시 main과 NULL 입력 검사는 제외했습니다.
- 정적 결과: 첫 삽입·양 끝·중간 노드의 연결, 마지막 노드 이동 시 TAIL 갱신, 삭제 시 한 번 해제·이동 시 미해제, 여섯 연산의 O(1) 처리를 확인했습니다. 삽입은 연결되지 않은 동적 할당 노드, 삭제·이동은 현재 리스트 소속의 유효 노드라는 전제입니다. 추가 오류는 발견하지 못했으며 PM은 소스를 수정하지 않았습니다.
- Review Decision: **REQUIRED** — 사용자 요청한 커밋 전 마무리 정적 검토 완료. 같은 PM 세션에서 수행했으며 독립 에이전트 리뷰·실행 검증은 아닙니다.
- 사용자 확인: 아직 실행 검증하지 않았다고 답했습니다. C-1은 구현·정적 검토 완료 / 사용자 검증 대기로 기록하고 단계 완료 확정·테스트 성공으로 처리하지 않았습니다. PM도 컴파일·실행·테스트하지 않았습니다.
- 기록 갱신: [README](../../README.md)·[REQUIREMENTS](REQUIREMENTS.md)·[STATUS](STATUS.md)를 실제 구현 상태에 맞췄습니다. Python 최종 요구 충족은 아직 미완료입니다.
- 커밋 범위: 사용자가 작성한 Linked_list.c와 위 진행 문서·이 기록. 실행 미검증을 커밋 메시지에 명시합니다. 커밋 요청은 받았으며 push는 요청받지 않았습니다.
- 실제 Git 결과: git add는 .git/index.lock 생성 권한 오류로 실패했고, 추가 권한 요청도 사용자 거부로 종료됐습니다. 스테이징·커밋은 미실행 상태이며 변경 파일은 보존했습니다. 커밋 보류 — 로컬 Git 쓰기 권한 미확보. 제안 메시지: `Feat: 이중 연결 리스트 연산 구현 정리 (실행 검증 대기)`.
- 다음: 사용자 요청에 따라 C-2 체이닝 해시맵의 요구사항과 직접 설계할 항목만 안내합니다. 기존 전역 리스트의 버킷별 관리·키/값 표현·해시 함수·확장 방식은 사용자가 결정하며 해시맵 코드·완성 설계는 만들지 않습니다.

### 커밋 운영 규칙 명확화

- 사용자 지침: 커밋은 항상 사용자가 직접 수행하고 PM은 필요 시 커밋 메시지만 추천합니다.
- 기존 공통 지침은 명시 요청 없는 Git 쓰기 금지였고, PM은 직전 요청을 직접 커밋 요청으로 해석했습니다. 이번 지침에 따라 스테이징·커밋 실행과 해당 권한 요청 없이 권장 범위·메시지만 안내하도록 [AGENTS.md](../../AGENTS.md)·[PM 지침](prompts/PM.md)·[WORKFLOW](WORKFLOW.md)·[STATUS](STATUS.md)를 정정했습니다. 이전 시도 이력은 보존했습니다.
- Review Decision: **SKIP** — 운영 문서만 수정했으며 소스·Git 인덱스·커밋은 변경하지 않았습니다.

### C-2 승인된 해시맵 학습 템플릿

- 승인: 사용자가 문자열 키·정수 값, 항목/해시맵 구조체, 해시 함수와 필수 여섯 연산의 원형·설명·TODO만 제공하는 계획에 '그럼 한번 해줘'라고 승인했습니다. 함수 본문·정답 알고리즘·테스트 구현은 포함하지 않습니다.
- 작업 전 [HashMap.c](../../c-practice/HashMap.c)는 미추적 파일로 include 두 줄과 비어 있는 main만 있었습니다. 제안한 별도 신규 파일 대신 이 파일의 기존 틀을 유지하고 Entry/HashMap 선언과 함수 원형·학습 주석을 추가했습니다. [Linked_list.c](../../c-practice/Linked_list.c)는 수정하지 않았습니다.
- 학습 선택: 버킷마다 첫 Entry 주소를 관리하고, C 표준 remove와 충돌을 피하기 위해 함수 이름에 hashmap_ 접두사를 사용했습니다. get은 성공 여부와 정수 값을 구분하며, keys는 C 실습용 전체 키 출력 원형입니다. 초기 버킷 수·키 소유권·생성/정리·해시 함수·확장 구현은 사용자에게 남겼습니다.
- [README](../../README.md)·[STATUS](STATUS.md)에 템플릿 준비/사용자 구현 대기를 기록했습니다. 필수 해시맵 기능이나 Python 최종 요구가 완료된 것은 아닙니다.
- Review Decision: **SKIP** — 선언·주석·진행 기록만 준비. 정적 확인만 수행하며 컴파일·실행·테스트·에이전트 위임·스테이징·커밋은 하지 않습니다.
- 다음: 준비/정리와 해시 함수의 역할을 이해한 뒤 사용자가 put/get부터 직접 구현합니다. 커밋 권장 — 템플릿·관련 진행 기록: `Chore: C 해시맵 학습 템플릿 준비`.

### C-2 연결 리스트 모듈 재사용 방향 선택

- 사용자 선택: 해시맵 안에서 연결 처리를 다시 구현하기보다 기존 연결 리스트를 가져다 쓰며 재사용성을 갖추기로 했습니다. 원문에서 권장하는 체이닝 재사용 방향을 채택한 것이며 미션 필수·보너스를 변경하지 않습니다.
- 현재 파일 확인: Linked_list.c는 전역 HEAD/TAIL과 int data, HashMap.c는 prev/next가 있는 독립 Entry 체인과 작성 중인 해시 함수·put을 사용합니다. PM은 두 소스를 수정하지 않았습니다.
- 다음 설계 순서: 리스트별 head/tail 상태와 대상 리스트를 전달받는 인터페이스 → 키/값 데이터 표현·노드/항목/키의 해제 책임 → 헤더/구현 분리와 버킷 연결. 세부 구현은 사용자가 결정하며 작성 중인 코드는 보존합니다.
- [STATUS](STATUS.md)·[README](../../README.md)에 선택한 방향만 반영했습니다. 방향 선택을 에이전트의 소스 리팩터링·헤더 생성 승인으로 해석하지 않았습니다.
- Review Decision: **SKIP** — 설계 코칭과 진행 기록만 수행. 소스 수정·컴파일·실행·테스트·위임·Git 쓰기 없음. 커밋 보류 — 재사용 인터페이스 설계·직접 구현 후 관련 변경을 함께 정리합니다.

### C-2 범용 데이터 표현 결정

- Decision: 사용자의 '이 부분은 정해줘' 요청에 따라 Node는 prev/next/void* data를 갖고, 각 사용 모듈은 자기 데이터 형식을 정의합니다. 해시맵의 Entry는 문자열 키·정수 값만 담으며 Node의 data가 Entry 주소를 가리킵니다. 버킷은 이 연결 리스트를 사용합니다.
- Reason: 리스트의 연결 알고리즘을 키/값 형식과 분리해 해시맵 외의 용도에서도 같은 함수를 사용하려는 사용자 목표를 충족합니다. 원문의 prev/next/data와 O(1) 연산 제약을 유지합니다.
- Alternative / Why not: Node에 key/value를 직접 넣으면 해시맵 항목에 종속됩니다. 현재 학습 범위에서는 callback이나 매크로 기반 타입 생성까지 추가할 필요가 없습니다.
- 수명 규칙: 호출자가 동적 할당한 Node를 삽입하면 리스트가 소유하고, 기존 삭제 연산은 Node만 해제합니다. data는 빌린 주소이며 리스트가 해제하지 않습니다. 실제 데이터의 소유자는 사용하는 모듈이고, 해시맵에서는 Entry와 키 문자열의 수명을 책임집니다. 삭제 전에 필요한 data 주소를 보관하고 연결·관련 참조를 정리한 뒤 데이터를 해제합니다. 이동에서는 어떤 객체도 해제하지 않습니다.
- [STATUS](STATUS.md)에 결정과 다음 사용자 작업을 반영했습니다. 현재 소스의 int data·독립 Entry 체인은 PM이 수정하지 않았습니다. 헤더 분리·함수 구현·실행 검증은 사용자가 진행합니다.
- Review Decision: **SKIP** — 요청받은 설계 결정·기록만 수행. 소스 수정·컴파일·실행·테스트·위임·Git 쓰기 없음. 커밋 보류 — 데이터 표현 반영과 헤더 분리 뒤 관련 변경을 함께 정리합니다.

### C-2 중간 진행 기록

- 사용자 구현: [LinkedList.h](../../c-practice/LinkedList.h)·[LinkedList.c](../../c-practice/LinkedList.c) 분리, void* data와 리스트별 상태 도입. [HashMap.c](../../c-practice/HashMap.c)는 LinkedList* buckets와 키/값 Data를 사용합니다.
- 진행: 해시의 unsigned char·31배 누적, put의 버킷 순회·같은 키 값 갱신·신규 Data/키/Node 할당과 리스트 삽입을 작성했습니다. 최신 put은 비교 분기를 바로잡았고 strlen(key)+1 키 저장 공간도 추가했습니다. 구현 완료·실행 성공을 뜻하지 않습니다.
- 우선 수정: 키 할당 실패 시 NewData 해제, NewNode 할당 실패 확인·기존 할당 정리, 신규 삽입 성공 시 count 증가, 고정 CAP 대신 실제 capacity 기반 버킷 계산. NewNode의 prev/next 설정은 insert_back과 중복되어 선택적으로 정리할 수 있습니다.
- 남은 구현: 맵/버킷 초기화·전체 메모리 정리, get/remove/contains/keys/size, 로드 팩터 0.75 초과 시 버킷 2배 확장. 오래된 Entry 관련 주석도 실제 Data/LinkedList 구조와 맞춥니다.
- 기록: [STATUS](STATUS.md)·[README](../../README.md)를 현재 단계·파일명에 맞췄습니다. 정적 확인만 수행했고 소스 수정·컴파일·실행·테스트·Git 쓰기는 하지 않았습니다. Review Decision: **SKIP** — 중간 기록 요청. 커밋 보류 — put의 실패 경로·count 보완 후 관련 변경을 묶는 것을 권장합니다.

## 2026-11-21

### C-2 해시맵 완료

- 사용자 구현: put 실패 경로 정리·count 갱신·capacity 기반 버킷 계산·로드 팩터 0.75 초과 시 2배 확장, get/remove/contains/keys/size.
- Claude 구현(사용자 요청): 탐색 공통 헬퍼 `hashmap_find_node`로 put/get/remove/contains 리팩토링, 확인용 `main`.
- 사용자 확인: 루트에서 `-fsanitize=address,undefined` 빌드 후 `./build/hashmap` 실행. 모든 항목이 기대값과 일치하고 sanitizer 오류 없음.

### C-3 최소 힙 설계 결정

- Decision:
  1. 파일은 [MinHeap.h](../../c-practice/MinHeap.h)·[MinHeap.c](../../c-practice/MinHeap.c). LinkedList와 같은 이름 규칙이며 구조체 정의는 헤더에 둔다.
  2. `expire_at`은 밀리초 단위 `long long`. C-3 테스트에서는 실제 시간 없이 숫자만 넣는다.
  3. key는 힙이 `strlen + 1`로 `malloc`해 복사·소유한다. `minheap_free`는 남은 항목의 key를 해제한다.
  4. 배열은 동적 확장하며 가득 차면 2배. `realloc` 결과는 임시 포인터로 받아 실패 시 원본을 보존한다.
  5. pop/peek은 `hashmap_get`과 같은 패턴(out 매개변수, 성공 1 / 빈 힙 0). 인덱스는 `size_t`, `_heapify_up`은 `while (i > 0)` 조건.
  6. pop의 key 소유권: `out_item->key`를 호출한 쪽으로 넘기고 힙은 해제하지 않는다. 호출한 쪽이 다 쓴 뒤 `free`한다. peek의 key는 빌린 주소이며 그 항목이 pop되기 전까지만 유효하다.
- Reason:
  - 초 단위로 저장하면 EXPIRE 호출 시점에 따라 최대 1초 오차가 생긴다.
  - C-4에서 lazy deletion을 쓰면 힙 항목이 해시맵 항목보다 오래 남으므로, 해시맵 key 주소를 빌리면 해제된 주소가 남는다. 그래서 최종 설계도 복사로 간다.
  - pop이 key를 해제하면 out 매개변수로 넘긴 key가 곧바로 해제된 주소가 된다(3·5번 충돌). 소유권을 넘기면 C-4에서 pop 한 번으로 해시맵 확인 후 해제할 수 있다.
- Alternative / Why not:
  - 초 단위 `long`: 1초 오차.
  - 해시맵 key 주소 공유: lazy deletion과 함께 쓰면 해제 후 사용 위험.
  - peek으로 확인 후 pop이 key 해제: 호출할 때마다 peek → pop 두 번이 필요하다.
- C-4 메모(C-3에서는 구현하지 않음): lazy deletion. EXPIRE 재호출·SET 덮어쓰기·DEL 때 힙의 옛 항목은 지우지 않고 남긴다. pop 시 해시맵에 key가 없거나 해시맵의 `expire_at`과 다르면 버린다. 이를 위해 C-4에서 해시맵 `Data`에 `expire_at` 필드가 추가된다.
- 반영: 사용자 요청에 따라 Claude가 구조체·함수 원형·역할 주석·TODO만 작성했다. 함수 본문·테스트 코드는 없다. `_heapify_up/_heapify_down`은 `MinHeap.c`의 `static` 원형이다.

## 2026-11-22

### C-4 메모리 제한 설계 결정

- Decision:
  1. 덮어쓰기 SET의 단일 항목이 maxmemory를 넘으면 OOM으로 명령을 거부하고 기존 값을 유지한다(Redis처럼 거부). OOM 검사를 put보다 앞에 둔다.
  2. 삭제는 `static void delete_data(Store*, Data*)` 하나로 모으고 `store_del`·`evict_if_needed`가 함께 쓴다. `used_memory` 감소는 이 함수에서만 한다. `hashmap_remove(map, data->key)`는 마지막에 호출하고 그 뒤 data·key를 읽지 않는다.
  3. 제거 루프 조건: `maxmemory > 0 && used_memory > maxmemory && lru.TAIL != NULL`.
  4. `store_set` 반환값은 `STORE_OK 1` / `STORE_ERR_ALLOC 0` / `STORE_ERR_OOM -1` 매크로. get·del의 1/0은 그대로.
  5. C의 value 길이는 int의 10진수 자릿수(`snprintf(NULL, 0, "%d", v)`)로 대체한다. Python은 value 문자열의 UTF-8 바이트 수.
  6. 파일 범위 static 이름은 예약 식별자를 피해 `entry_size`, `evict_if_needed`로 한다. MinHeap의 `_heapify_up/_heapify_down`은 미션 지정 이름이라 유지한다.
- Reason:
  - 거부 후 기존 값 유지가 "저장하지 않고 OOM" 규칙과 맞고, 검사를 put 앞에 두면 되돌릴 것이 없다.
  - evict가 `store_del(store, TAIL의 key)`를 부르면 넘긴 key가 삭제 과정에서 해제되는 같은 문자열이라, 이후 코드가 key를 쓰면 해제 후 사용이 된다. Data를 직접 넘기면 해시 조회도 한 번 줄어든다.
  - TAIL 검사는 used_memory 계산 버그 시 NULL 접근·무한 루프를 막는다.
  - CLI가 OOM 에러 문구를 따로 출력하려면 할당 실패와 구분해야 한다.
- Alternative / Why not:
  - evict에서 `store_del` 재사용: 위 key 해제 문제.
  - C value를 `char*`로 변경: 해시맵·Store·테스트 전부 수정이 필요하다.
- 반영: 사용자 승인에 따라 Claude가 Store.h 필드·매크로, Store.c 원형·TODO, store_test.c 필드 초기화만 작성했다. 함수 본문은 사용자가 작성한다.

### C-4 TTL 설계 결정

- Decision:
  1. `Data`에 `long long expire_at`(ms) 추가. 만료 없음은 `NO_EXPIRE`(Store.h, 값 0). 해시맵은 이 필드를 읽지 않고 Store가 put 직후 채운다.
  2. `Store`에 `MinHeap heap` 추가(map·lru와 같은 값 멤버).
  3. 현재 시각은 Store 함수의 `long long now` 인자로 받는다. 테스트는 숫자를 넣고 실제 프로그램은 현재 시각을 넣는다.
  4. 만료 정리 `expire_if_needed(store, now)`를 모든 Store 명령 맨 앞에서 호출한다. 힙 맨 위가 now 이하인 동안 pop하고, 맨 위가 안 지났으면 멈춘다(최소 힙이라 나머지도 안 지남).
  5. lazy deletion: DEL·SET 덮어쓰기·EXPIRE 재설정 때 힙은 건드리지 않는다. pop한 항목은 해시맵에 키가 있고 `expire_at`이 같을 때만 지우고, 아니면 버린다. pop한 key는 Store가 확인 후 free한다(C-3 소유권 규칙).
  6. 만료로 지운 키는 `evicted_keys`에 세지 않는다(LRU 제거만 센다). used_memory는 delete_data가 줄인다.
  7. TTL 남은 초는 내림.
  8. 전체 조회(DBSIZE/KEYS)의 만료 정리 범위: 명령 전 정리(4번)로 만료 키가 이미 지워져 있으므로 별도 처리 없이 제외된다. [REQUIREMENTS](REQUIREMENTS.md) 미명시 항목 해결.
  9. EXPIRE에서 힙 추가가 실패하면 만료 시각을 바꾸지 않고 `STORE_ERR_ALLOC`을 반환한다.
  10. Store 반환값 규칙: store 함수 전체에서 반환 0 이상은 성공(Redis 응답 값), 음수는 내부 에러이며 예외가 없다. 현재 `STORE_ERR_ALLOC`이 0이라 응답 값(없는 키 0)과 겹치므로 음수로 바꾼다(Store.h TODO, 사용자 수정). TTL은 응답 값 -2/-1이 음수라 규칙과 겹치므로 `int store_ttl(Store*, const char* key, long long now, long long* out_ttl)`로 바꿔 응답 값(-2·-1·남은 초)을 `*out_ttl`로 전달하고, 반환값은 성공/에러만 나타낸다.
  11. `hashmap_put`의 새 Data는 calloc으로 할당해 `lru_node = NULL`, `expire_at = 0(NO_EXPIRE)`로 시작한다(HashMap.c TODO, 사용자 수정). Store가 채우기 전 쓰레기 값을 없앤다.
- Reason:
  - 시각 인자: 테스트에서 실제로 기다리지 않고 만료 시점을 정확히 재현할 수 있다.
  - 명령 전 정리: "키 명령 전 만료 확인", "만료된 GET은 LRU 미갱신", 전체 조회의 만료 제외가 한 곳에서 함께 지켜진다.
  - 내림: 미션 예시(EXPIRE 3 직후 TTL 2)와 맞춘다. 단, 두 명령 사이에 1ms 이상 지나야 2가 된다. 같은 now로 호출하면 3이다.
- Alternative / Why not:
  - Store에 now 필드: 원형은 그대로지만 시각이 숨겨져 읽기 어렵다.
  - 키 접근 때만 lazy 확인: 힙을 쓰지 않게 되어 "힙으로 가장 빠른 만료를 찾는다"는 목표와 맞지 않고, 전체 조회에 만료 키가 남는다.
- 반영: 사용자 승인에 따라 Claude가 HashMap.h 필드, Store.h 상수·필드·원형, Store.c 원형·TODO와 기존 정의의 매개변수 목록, store_test.c의 now 인자·힙 초기화만 작성했다. 함수 본문은 사용자가 작성한다.

### CONFIG SET maxmemory 축소 시점 결정

- Decision: maxmemory를 현재 used_memory보다 낮추면 CONFIG SET 시점에 즉시 `evict_if_needed`로 제거한다. 구현은 CLI 단계에서 한다.
- Reason: 다음 SET까지 INFO memory가 `used_memory > maxmemory` 상태로 보이는 것을 피한다.
- Alternative / Why not: 다음 SET 때 제거(원문 문구 "SET 이후"에만 맞춤): 그 사이 INFO memory가 제한을 넘은 상태를 보여 준다.
- [REQUIREMENTS](REQUIREMENTS.md) 미명시 항목 해결.

<!-- 기록 형식
## YYYY-MM-DD

### TASK-XXX

작업:
- 실제 변경 사항과 TASK 경로

실제 확인된 결과:
- 사용자 확인: 실행 명령/입력/환경, 관찰한 결과
- 정적 확인: 코드나 diff에서 직접 확인한 내용 (실행 검증 아님)

Review:
- 등급: REQUIRED / RECOMMENDED / SKIP
- 진행: 대기 / 완료 / 생략 (RECOMMENDED 생략은 사용자 선택 기록)
- 이유와 리뷰 경로:
- Findings / PM Decision / Fix: 실제 발견·판단·반영 내역만 기록

남은 문제:
- 미완료 또는 미검증 항목

문서 동기화:
- REQUIREMENTS / STATUS / WORKLOG / README·diagram 중 실제 변경한 내용 또는 README 변경 불필요 이유

다음:
- 다음 행동
-->

<!-- 중요한 설계 결정이 있을 때만 추가합니다. 사소한 이름·CSS·문법 선택은 기록하지 않습니다.
### Decision
Decision:
Reason:
Alternative:
Why not:
-->

<!-- 반복 패턴이 확인된 경우에만 추가합니다.
### Harness Feedback
반복 근거: TASK/리뷰/사용자 피드백 연결
개선 제안: 대상 지침과 기존 규칙 통합 방안
사용자 승인 및 반영 상태: 실제 확인된 내용만
-->

<!-- 필수 구현·사용자 테스트 완료 후 PM이 실제 점검한 결과만 추가합니다.
### Mission Close-out
Required / Optional / Bonus 상태:
기록·코드·README·diagram 일치 여부:
정리 제안과 남은 문제:
Harness 개선 후보:
결론 및 다음 행동:
-->

<!-- 사용자가 사용량 점검을 요청하거나 개선 효과를 비교할 때만 기존 기록에 짧게 추가할 수 있습니다.
비교 작업·완료 범위 / 실제 모델·Reasoning·속도 / 호출 수 / 평균 입력 / 캐시·비캐시 입력 / 출력 / 실제 한도 변화 / 계산 한계.
누적 카운터 중복 합산·reasoning 중복 합산을 피하고 토큰·참고 크레딧·구독 한도 차감을 구분합니다.
매 TASK의 필수 보고나 새 계측 절차로 만들지 않습니다. -->
