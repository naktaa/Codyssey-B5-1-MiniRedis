# 스택 · 큐 · 덱

보너스 2 "스택/큐/덱 이해와 활용" 문서입니다. 세 구조는 **어디로 넣고 어디서 빼는가**만 다릅니다. 1~4절에서 개념을 정리하고, 5절에서 지금 Mini Redis 코드의 어디에 이 구조들이 들어 있는지 연결합니다.

## 1. 한눈에 보기

| 구조 | 넣는 곳 | 빼는 곳 | 나오는 순서 | 넣기 | 빼기 | 맨 앞 보기 |
| --- | --- | --- | --- | --- | --- | --- |
| 스택 | top | top | 나중에 넣은 것부터 (LIFO) | O(1) | O(1) | O(1) |
| 큐 | rear | front | 먼저 넣은 것부터 (FIFO) | O(1) | O(1) | O(1) |
| 덱 | 양 끝 | 양 끝 | 쓰는 쪽에 따라 | O(1) | O(1) | O(1) |
| (비교) 우선순위 큐 | 아무 데나 | 가장 작은 값 | 우선순위 순 | O(log n) | O(log n) | O(1) |

위 복잡도는 알맞은 방법으로 만들었을 때입니다. 잘못 만들면 이렇게 됩니다.
- 배열의 **앞**을 스택의 top으로 쓰거나, 큐를 배열 앞에서 빼면: 넣기·빼기가 O(n)
- 덱을 **단일** 연결 리스트(next만)로 만들면: 뒤에서 빼기가 O(n)
- 배열로 만든 스택·큐의 넣기는 가끔 2배 확장이 일어나서 평균 O(1)

## 2. 스택

**한 줄 정의**: 한쪽 끝(top)에서만 넣고 빼는 구조. 마지막에 넣은 것이 먼저 나온다(LIFO).

```text
      push ↓   ↑ pop
        ┌─────┐
top →   │  C  │   ← 마지막에 넣음, 가장 먼저 나옴
        ├─────┤
        │  B  │
        ├─────┤
        │  A  │   ← 처음에 넣음, 가장 늦게 나옴
        └─────┘
```

| 연산 | 하는 일 | 복잡도 이유 |
| --- | --- | --- |
| push | top 위에 올린다 | 끝 한 곳만 바뀐다 |
| pop | top을 꺼낸다 | 끝 한 곳만 바뀌고 나머지 원소는 움직이지 않는다 |
| peek | top을 꺼내지 않고 본다 | top 위치가 정해져 있어 찾을 필요가 없다 |

**만드는 방법**
- **배열**: 배열의 끝을 top으로 둡니다. push는 append, pop은 끝 칸 remove입니다. 끝 칸이라 당길 원소가 없습니다.
- **연결 리스트**: HEAD를 top으로 둡니다. push는 insert_front, pop은 remove_front입니다.
- top은 항상 **움직일 원소가 없는 쪽**에 둡니다. 배열 앞을 top으로 두면 넣고 뺄 때마다 나머지를 전부 밀고 당겨야 합니다.
- Python에서는 보통 list의 append/pop을 스택으로 씁니다. 이 프로젝트는 list를 고정 길이 배열로만 쓰므로 DynamicArray가 그 역할을 합니다.

**쓰임**
- 함수 호출 스택: 나중에 호출된 함수가 먼저 끝난다.
- 되돌리기(Undo): 마지막 작업부터 취소한다.
- 괄호 짝 검사: 여는 괄호를 쌓고, 닫는 괄호가 오면 top과 맞춰 본다.
- 깊이 우선 탐색(DFS): 갈 곳을 쌓아 두고 가장 최근에 쌓은 곳부터 들어간다.

## 3. 큐

**한 줄 정의**: 뒤(rear)로 넣고 앞(front)에서 빼는 구조. 먼저 넣은 것이 먼저 나온다(FIFO).

```text
dequeue ←                                  ← enqueue
         ┌─────┬─────┬─────┬─────┐
         │  A  │  B  │  C  │  D  │
         └─────┴─────┴─────┴─────┘
           ↑                   ↑
         front               rear
      (가장 먼저 들어옴)    (가장 늦게 들어옴)
```

| 연산 | 하는 일 | 복잡도 이유 |
| --- | --- | --- |
| enqueue | rear 뒤에 붙인다 | 끝 한 곳만 바뀐다 |
| dequeue | front를 꺼낸다 | 연결 리스트·원형 큐는 front 위치만 옮긴다. 일반 배열은 나머지를 한 칸씩 당겨야 한다 |
| peek | front를 꺼내지 않고 본다 | front 위치가 정해져 있다 |

**만드는 방법**
- **연결 리스트**: HEAD를 front, TAIL을 rear로 둡니다. enqueue는 insert_back, dequeue는 remove_front입니다. 양 끝 모두 포인터만 바꿉니다.
- **일반 배열(동적 배열)**: enqueue를 append로 하는 것은 괜찮습니다. 하지만 dequeue를 remove(0)으로 하면 뒤의 원소를 전부 한 칸씩 당깁니다.
- **원형 큐**: 배열을 쓰면서 당기기를 없앤 방법입니다.

#### 원형 큐

```text
capacity 5, A → B → C 순서로 넣었을 때

 index:   0     1     2     3     4
        ┌─────┬─────┬─────┬─────┬─────┐
        │  C  │     │     │  A  │  B  │
        └─────┴─────┴─────┴─────┴─────┘
           ↑                 ↑
          rear             front

4번 칸 다음은 0번 칸: 배열의 끝과 처음이 이어진 고리로 본다
```

- dequeue는 원소를 당기지 않고 front 인덱스만 한 칸 옮깁니다.
- enqueue는 rear 다음 칸에 넣습니다. 배열 끝에 닿으면 0번 칸으로 돌아가, 앞에서 비운 칸을 다시 씁니다.
- 꽉 차면 동적 배열처럼 2배로 늘립니다. 이때 front부터 차례로 새 배열 앞쪽에 옮겨야 순서가 유지됩니다.

**쓰임**
- 레벨 순회(BFS): 가까운 노드부터 차례로 방문한다.
- 작업 대기열: 먼저 온 요청부터 처리한다.
- 메시지 버퍼: 받은 순서대로 꺼내 읽는다.
- 커맨드 히스토리: 새 명령을 넣고, 개수를 넘으면 가장 오래된 것부터 버린다.

## 4. 덱

**한 줄 정의**: 양 끝 모두에서 넣고 뺄 수 있는 구조(Double-Ended Queue). 한쪽만 쓰면 스택, 반대쪽으로 빼면 큐가 된다.

```text
push_front →                                  ← push_back
pop_front  ←                                  → pop_back
             ┌─────┬─────┬─────┬─────┐
             │  A  │  B  │  C  │  D  │
             └─────┴─────┴─────┴─────┘
               ↑                   ↑
             front               back
```

| 연산 | 하는 일 | 복잡도 이유 |
| --- | --- | --- |
| push_front / push_back | 앞 / 뒤에 붙인다 | 끝 한 곳만 바뀐다 |
| pop_front / pop_back | 앞 / 뒤를 꺼낸다 | 이중 연결 리스트는 양 끝에서 next·prev로 새 끝을 바로 찾는다 |
| peek_front / peek_back | 앞 / 뒤를 본다 | 양 끝 위치를 들고 있다 |

**만드는 방법**
- **이중 연결 리스트**: HEAD·TAIL이 있고 노드마다 prev·next가 있어 그대로 덱이 됩니다.
- pop_back이 새 TAIL을 찾으려면 TAIL의 prev가 필요합니다. next만 있는 단일 연결 리스트로는 HEAD부터 끝 바로 앞까지 따라가야 합니다.
- **원형 배열**: 원형 큐에서 front도 뒤로 움직일 수 있게 하면 덱이 됩니다.
- Python의 `collections.deque`가 바로 이 구조입니다. 이 미션에서는 금지되어 있어 직접 만든 LinkedList를 씁니다.

**쓰임**
- 스택·큐 대신 쓰기: 쓰는 끝만 정하면 둘 다 된다.
- 슬라이딩 윈도우: 새 값은 뒤에 넣고, 범위를 벗어난 값은 앞에서 뺀다.
- 최근 N개 기록: 새 기록은 앞에 넣고, N개를 넘으면 뒤에서 버린다.

## 5. 이 프로젝트 코드와 연결

<!--
큰 구조 그림 자리 (이미지는 직접 넣음)
내용: Store 안에서 각 자료구조가 어떤 역할로 놓이는지 한 장으로
  - LRU 리스트(LinkedList): 덱 + 중간 이동
  - 해시맵 버킷(LinkedList 배열): 체인
  - Python 힙 저장소(DynamicArray): 끝 칸 넣기·빼기 = 스택 연산
  - MinHeap(TTL): 우선순위 큐
-->

지금 LinkedList의 양 끝 연산을 덱·스택·큐에 맞춰 보면 이렇습니다.

```text
         front                                 back
         HEAD                                  TAIL
          ↓                                     ↓
          [A]  <->  [B]  <->  [C]  <->  [D]
insert_front / remove_front          insert_back / remove_back

스택: HEAD 쪽만 사용        push = insert_front      pop = remove_front     (top = HEAD)
큐:   뒤로 넣고 앞에서 뺌    enqueue = insert_back    dequeue = remove_front
LRU:  새 키 insert_front, 사용한 키 move_to_front, 메모리 초과 시 TAIL부터 제거
```

| 구조 | 지금 코드 | 코드 | 메모 |
| --- | --- | --- | --- |
| 덱 (넣기) | LinkedList.c insert_front · insert_back | [L6-16](/c-practice/src/LinkedList.c#L6) [L18-28](/c-practice/src/LinkedList.c#L18) | HEAD = front, TAIL = back |
| 덱 (빼기) | LinkedList.c remove_front · remove_back | [L30-45](/c-practice/src/LinkedList.c#L30) [L47-62](/c-practice/src/LinkedList.c#L47) | prev가 있어 remove_back도 새 TAIL을 바로 찾음 |
| 스택 | DynamicArray.c append · remove | [L41-57](/c-practice/src/DynamicArray.c#L41) [L94-106](/c-practice/src/DynamicArray.c#L94) | 끝 칸에서만 넣고 빼면 당길 원소가 없음 |
| 스택 연산을 쓰는 곳 | min_heap.py pop의 끝 칸 remove | [L75](/python/min_heap.py#L75) | 힙 pop이 마지막 칸을 스택처럼 떼어 냄 |
| 우선순위 큐 | MinHeap.c push · pop | [L99-121](/c-practice/src/MinHeap.c#L99) [L129-139](/c-practice/src/MinHeap.c#L129) | 들어온 순서가 아니라 expire_at이 작은 순서로 나옴 |
| 덱 + 중간 이동 | Store.c LRU: 새 키 insert_front, SET·GET move_to_front, TAIL 제거 | [L136](/c-practice/src/Store.c#L136) [L139](/c-practice/src/Store.c#L139) [L159](/c-practice/src/Store.c#L159) [L57-58](/c-practice/src/Store.c#L57) | 덱 연산만으로는 중간 노드를 앞으로 옮길 수 없음 |

**주의할 점: `remove_front`는 data를 돌려주지 않습니다.** ([LinkedList.c](/c-practice/src/LinkedList.c) [L30-45](/c-practice/src/LinkedList.c#L30)) 노드를 떼어 Node만 해제하므로, 큐의 dequeue처럼 꺼낸 값이 필요하면 remove_front를 부르기 **전에** HEAD의 data를 먼저 읽어 둬야 합니다. 해시맵 remove가 remove_node 전에 Data 주소를 꺼내 두는 것과 같은 이유입니다([HashMap.c](/c-practice/src/HashMap.c) [L159-161](/c-practice/src/HashMap.c#L159)). data 자체는 해제되지 않으므로 data의 주인이 따로 해제해야 합니다.

## 6. 다음 보너스와 연결

- 보너스 3 레벨 순회: 방문할 노드를 큐에 넣고 front부터 꺼냅니다. `collections`를 못 쓰므로 LinkedList의 insert_back + remove_front를 큐로 씁니다.
- 보너스 5 Pub/Sub: 받은 메시지 큐(inbox)를 LinkedList로 두고, PUBLISH는 rear(insert_back)에 넣고, 읽을 때는 front(remove_front)부터 꺼냅니다. 서버가 없어 구독자는 REPL 하나뿐이라 큐도 하나입니다([PubSub.c](/c-practice/src/PubSub.c) [L41](/c-practice/src/PubSub.c#L41) [L46-54](/c-practice/src/PubSub.c#L46)).
