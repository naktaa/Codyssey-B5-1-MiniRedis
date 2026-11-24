# Mini Redis 학습 노트: 해시맵·이중 연결 리스트·최소 힙으로 만든 LRU·TTL 키-값 저장소

## 1. 한눈에 보기

- **목적**: 내장 컬렉션 없이 직접 만든 자료구조로 Redis의 LRU 제거·TTL 만료·메모리 제한을 구현합니다.
- **기능**: `SET` `GET` `DEL` `EXISTS` `DBSIZE` `KEYS` · `CONFIG SET maxmemory` `INFO memory` · `EXPIRE` `TTL` (+ `exit`/`quit`)
- **결과**: C 확인용 main은 sanitizer 빌드로 사용자 실행 확인, leaks 0. Python unittest 30개는 Claude 실행 통과 · 사용자 **미검증**. 대표 출력(store_test): `used=8 evicted=1 (기대: 8, 1)`
- **관련 파일**
  - C: [LinkedList.c](/c-practice/LinkedList.c) · [HashMap.c](/c-practice/HashMap.c) · [MinHeap.c](/c-practice/MinHeap.c) · [Store.c](/c-practice/Store.c) · 헤더 [LinkedList.h](/c-practice/LinkedList.h) [HashMap.h](/c-practice/HashMap.h) [MinHeap.h](/c-practice/MinHeap.h) [Store.h](/c-practice/Store.h) · 확인용 [hashmap_test.c](/c-practice/hashmap_test.c) [store_test.c](/c-practice/store_test.c)
  - Python: [python/](/python/) · [tests/](/tests/)

구현 설명은 직접 작성한 C 코드 기준이고, Python과의 차이는 5절에 모았습니다.

## 2. 구조

흐름: 확인용 main([store_test.c](/c-practice/store_test.c)) → `store_*` 함수([Store.c](/c-practice/Store.c)) → HashMap.c · LinkedList.c · MinHeap.c. CLI는 Python([main.py](/python/main.py))에만 있습니다.

![Store 자료구조 관계](images/store-structure.png)
> 그림 메모: `Store` 구조체가 `map`(`HashMap`), `lru`(`LinkedList`), `heap`(`MinHeap`)을 값으로 가진다.
> `map.buckets`는 `LinkedList` 배열이고, 버킷 `Node.data`(`void*`)가 `Data { key, value, lru_node, expire_at }`를 가리킨다.
> `Data.lru_node` → `lru`의 `Node`, 그 `Node.data` → 같은 `Data` (양방향 화살표로 강조). `lru.HEAD` = 최근, `lru.TAIL` = 가장 오래됨.
> `heap.items[]`의 `HeapItem { expire_at, key }`는 포인터 없이 자기 key 복사본으로만 `Data`를 찾는다(점선 화살표).
> 같은 `Data`를 버킷 `Node`와 LRU `Node` 두 개가 가리키고, `Data`를 해제하는 쪽은 해시맵뿐이라는 점을 강조.

| 구성 요소 | 자료구조 | 하는 일 | 소유(해제 책임) |
| --- | --- | --- | --- |
| `Store.map` | 체이닝 해시맵 (`HashMap`) | key → Data를 평균 O(1)에 찾기·저장 | 버킷 배열, 버킷 Node, Data, key 문자열 |
| `Store.lru` | 이중 연결 리스트 (`LinkedList`) | 사용 순서 기록. HEAD 최근, TAIL 제거 대상 | LRU Node만. data는 빌린 주소 |
| `Store.heap` | 최소 힙 (`MinHeap`) | (expire_at, key)를 만료 시각 순으로 꺼내기 | items 배열, 각 항목의 key 복사본 |
| `Data` | 구조체 (HashMap.h) | key·value와 lru_node·expire_at. 세 구조를 잇는 중심 | 해시맵 |
| `HeapItem` | 구조체 (MinHeap.h) | 힙의 한 칸. 만료 시각과 key | 힙. pop하면 key는 호출한 쪽으로 넘어감 |

## 3. 자료구조 구현

### 3.1 LinkedList

**구조** ([LinkedList.h](/c-practice/LinkedList.h) [L7-16](/c-practice/LinkedList.h#L7))

```text
HEAD -> [A] <-> [B] <- TAIL

[A] = Node { prev = NULL, data = (void*), next = [B] }
[B] = Node { prev = [A],  data = (void*), next = NULL }
```

Node는 prev·next·data 세 필드만 가지고, LinkedList는 HEAD·TAIL 두 포인터만 가집니다. data가 `void*`라 리스트는 무엇을 담는지 모르고, 해시맵 버킷과 LRU가 같은 리스트 코드를 씁니다.

**핵심 연산**

| 연산 | 하는 일 | 코드 |
| --- | --- | --- |
| `insert_front(list, now)` | now를 맨 앞에 붙임 | [L6-16](/c-practice/LinkedList.c#L6) |
| `insert_back(list, now)` | now를 맨 뒤에 붙임 | [L18-28](/c-practice/LinkedList.c#L18) |
| `remove_front(list)` | 맨 앞 노드를 떼고 free. 빈 리스트면 그대로 | [L30-45](/c-practice/LinkedList.c#L30) |
| `remove_back(list)` | 맨 뒤 노드를 떼고 free. 빈 리스트면 그대로 | [L47-62](/c-practice/LinkedList.c#L47) |
| `remove_node(list, now)` | 양 끝은 위 두 함수에 맡기고, 중간은 이웃을 이은 뒤 free | [L64-76](/c-practice/LinkedList.c#L64) |
| `move_to_front(list, now)` | 떼어서 맨 앞으로 옮김. free하지 않음 | [L78-95](/c-practice/LinkedList.c#L78) |

#### insert_front ([LinkedList.c](/c-practice/LinkedList.c) [L6-16](/c-practice/LinkedList.c#L6))

새 노드 N의 prev·next를 정하고, 기존 HEAD와 리스트의 HEAD를 N으로 바꿉니다.

```text
before:  HEAD -> [A] <-> [B] <- TAIL
after:   HEAD -> [N] <-> [A] <-> [B] <- TAIL
changed: N.prev = NULL, N.next = A, A.prev = N, HEAD = N
```

빈 리스트면 HEAD와 TAIL이 모두 N이 됩니다. insert_back은 방향만 반대입니다.

#### remove_node ([LinkedList.c](/c-practice/LinkedList.c) [L64-76](/c-practice/LinkedList.c#L64))

중간 노드는 앞뒤 이웃을 서로 이은 뒤 free합니다.

```text
before:  HEAD -> [A] <-> [B] <-> [C] <- TAIL
after:   HEAD -> [A] <-> [C] <- TAIL        (B is freed)
changed: A.next = C (now->prev->next = now->next), C.prev = A (now->next->prev = now->prev)
```

now가 HEAD나 TAIL이면 remove_front·remove_back을 불러, HEAD·TAIL 갱신 코드를 한 곳에만 둡니다.

#### move_to_front ([LinkedList.c](/c-practice/LinkedList.c) [L78-95](/c-practice/LinkedList.c#L78))

노드를 새로 만들거나 free하지 않고 연결만 바꿔 맨 앞으로 옮깁니다.

```text
before:  HEAD -> [A] <-> [B] <-> [C] <- TAIL
step 1:  B.next = NULL, TAIL = B             (detach C)
step 2:  A.prev = C, C.prev = NULL, C.next = A, HEAD = C
after:   HEAD -> [C] <-> [A] <-> [B] <- TAIL
```

```c
        now->prev->next = now->next;
        if (now == list->TAIL) {
            list->TAIL = now->prev;
        }
        else {
            now->next->prev = now->prev;
        }

        list->HEAD->prev = now;
        now->prev = NULL, now->next = list->HEAD;
        list->HEAD = now;
```

now가 TAIL이면 뒤 이웃이 없으므로 TAIL만 앞으로 당깁니다. 노드 주소가 그대로라 `Data.lru_node`가 계속 같은 노드를 가리킬 수 있습니다.

**왜 이렇게 했나**

- remove 계열은 Node만 free하고 data는 건드리지 않습니다. Node는 리스트 소유지만 data가 가리키는 Data는 해시맵 소유라서, 호출하는 쪽이 remove 전에 data를 꺼내 둡니다.
- move_to_front는 free하지 않습니다. LRU 갱신은 같은 노드를 재사용해야 Data와의 연결이 끊기지 않습니다.

> **Q4. 왜 단일 연결이 아니라 이중 연결(prev)이어야 하나?**
> 노드를 떼려면 앞 노드의 next를 바꿔야 하는데, prev가 없으면 앞 노드를 찾으려고 HEAD부터 O(n)을 걸어야 합니다.
> remove_back도 새 TAIL을 `list->TAIL->prev`로 바로 구합니다.
> LRU는 TAIL 제거와 중간 노드 이동이 계속 일어나므로 prev가 O(1)의 조건입니다. ([LinkedList.c](/c-practice/LinkedList.c) [L57](/c-practice/LinkedList.c#L57), [L72-73](/c-practice/LinkedList.c#L72))

**복잡도**: 노드를 이미 알고 있을 때 6개 연산 모두 O(1)입니다. key로 노드를 찾아야 하면 처음부터 걸어야 하므로 O(n)입니다. 이 차이 때문에 4.1의 `Data.lru_node`가 필요합니다.

### 3.2 HashMap

**구조** ([HashMap.h](/c-practice/HashMap.h) [L8-20](/c-practice/HashMap.h#L8))

```text
buckets (capacity = 4, count = 3)
 [0] (empty)
 [1] HEAD -> [a] <-> [e] <- TAIL
 [2] HEAD -> [b] <- TAIL
 [3] (empty)

[x] = Node, Node.data -> Data(key="x", ...)
```

buckets는 `LinkedList` 배열이고 각 칸이 3.1의 리스트입니다. 빈 버킷은 HEAD·TAIL이 NULL이므로, 배열을 calloc으로 잡으면 모든 칸이 빈 리스트로 시작합니다.

**핵심 연산**

| 연산 | 하는 일 | 코드 |
| --- | --- | --- |
| `hashmap_hash(key)` | key 바이트에 31을 곱하며 더한 size_t 값 | [L18-27](/c-practice/HashMap.c#L18) |
| `hashmap_find_node(map, key, out_list)` | 체인을 돌며 strcmp로 찾음. 못 찾아도 버킷 주소는 넘겨 줌 | [L34-50](/c-practice/HashMap.c#L34) |
| `hashmap_put(map, key, value)` | 있으면 값만 교체, 없으면 Data·key·Node를 할당해 버킷 끝에 추가 | [L59-94](/c-practice/HashMap.c#L59) |
| `hashmap_remove(map, key)` | 노드를 떼고 key → Data 순으로 free, count - 1 | [L152-167](/c-practice/HashMap.c#L152) |
| `hashmap_get` · `hashmap_contains` · `hashmap_get_data` | 모두 hashmap_find_node로 찾음 | - |
| `hashmap_keys(map)` | 모든 버킷을 돌며 key를 출력 | [L182-196](/c-practice/HashMap.c#L182) |
| `hashmap_free(map)` | 모든 노드의 key → Data → Node, 마지막에 버킷 배열 free | [L219-234](/c-practice/HashMap.c#L219) |

#### 해시 계산 예

"ab"의 바이트 97, 98을 차례로 누적하고, capacity로 나눈 나머지가 버킷 번호입니다.

```text
key = "ab"  ->  bytes 97, 98
sum = 0
sum = 0  * 31 + 97 = 97
sum = 97 * 31 + 98 = 3105
bucket = 3105 % capacity(4) = 1
```

#### 확장 ([HashMap.c](/c-practice/HashMap.c) [L96-123](/c-practice/HashMap.c#L96))

count * 4 > capacity * 3이 되면 버킷을 2배로 늘리고 모든 노드를 새 번호로 옮깁니다.

```text
put a(97), e(101), i(105): count=3 -> 12 > 12 false, no resize
put b(98):                 count=4 -> 16 > 12 true,  resize 4 -> 8

before (capacity 4)
 [1] HEAD -> [a] <-> [e] <-> [i] <- TAIL
 [2] HEAD -> [b] <- TAIL

after (capacity 8)
 [1] HEAD -> [a] <-> [i] <- TAIL       (97 % 8 = 1, 105 % 8 = 1)
 [2] HEAD -> [b] <- TAIL               (98 % 8 = 2)
 [5] HEAD -> [e] <- TAIL               (101 % 8 = 5)
```

```c
        for (size_t i = 0; i < map->capacity; i++) {
            LinkedList* prev_list = &map->buckets[i];
            Node* now = prev_list->HEAD;

            while (now) {
                Data* data = now->data;
                size_t idx = hashmap_hash(data->key) % new_capacity;
                LinkedList* nextList = &new_buckets[idx];
                Node* now_temp = now->next;
                insert_back(nextList, now);
                now = now_temp;
            }
        }
```

노드를 새로 할당하지 않고 기존 Node를 새 버킷으로 옮긴 뒤, 옛 버킷 배열만 free합니다. insert_back이 now->next를 NULL로 바꾸므로, `now_temp`에 다음 노드를 먼저 저장해야 옛 체인을 끝까지 돌 수 있습니다.

**왜 이렇게 했나**

- 체이닝은 같은 버킷에 여러 키가 와도 리스트에 이어 붙이면 되므로 충돌 처리가 단순하고, 3.1 리스트를 그대로 재사용합니다.
- key는 `malloc(strlen(key) + 1)`과 strcpy로 복사해 해시맵이 소유합니다. hashmap_test.c처럼 호출하는 쪽이 같은 버퍼를 재사용해도 저장된 key는 바뀌지 않습니다.
- put은 Data → key → Node 순으로 할당하고, 중간에 실패하면 그때까지 할당한 것만 free한 뒤 0을 돌려줍니다. 확장용 calloc이 실패하면 확장만 포기하고, 이미 넣은 항목은 그대로 둡니다.
- 해제는 key → Data → Node 순서입니다. Data 안의 key 포인터를 먼저 free해야 하고, hashmap_remove는 remove_node가 Node를 free하므로 data 주소를 먼저 꺼내 둡니다.

> **Q1. 왜 31을 곱해 누적하고, 왜 문자가 아니라 바이트(`unsigned char`)를 쓰나?**
> 곱하며 누적하면 글자 순서가 결과에 반영되어 "ab"(3105)와 "ba"(3135)가 달라지고, 31은 홀수 소수라 값이 고르게 퍼집니다.
> char는 부호가 있을 수 있어 한글처럼 128 이상인 바이트가 음수가 되므로, `unsigned char`로 바꿔 0~255로 누적합니다. size_t는 부호 없는 정수라 overflow하면 2^64로 나눈 나머지로 감기므로 그대로 써도 됩니다.
> Python 정수는 overflow가 없어 같은 값을 내려면 매 단계 64비트로 잘라야 합니다(5절). ([HashMap.c](/c-practice/HashMap.c) [L21-24](/c-practice/HashMap.c#L21))

> **Q2. 왜 로드 팩터 0.75를 넘으면 2배로 늘리고, 확장 때 모든 노드를 다시 배치하나?**
> 로드 팩터(count / capacity)가 커지면 체인이 길어지는데, 2배씩 늘리면 확장 비용 O(n)이 여러 번의 put에 나뉘어 평균 O(1)이 유지됩니다.
> count / capacity > 0.75의 양변에 4 × capacity를 곱하면 `map->count * 4 > map->capacity * 3`이 되므로 두 조건은 같고, 정수 비교라 실수 오차가 없습니다.
> 버킷 번호는 hash % capacity라 capacity가 바뀌면 번호도 바뀌므로, 모든 노드를 새 번호로 옮겨야 다시 찾을 수 있습니다. ([HashMap.c](/c-practice/HashMap.c) [L97](/c-practice/HashMap.c#L97), [L111](/c-practice/HashMap.c#L111))

**복잡도**: put·get·remove·contains는 평균 O(1)이고, 모든 키가 한 버킷에 몰리면 최악 O(n)입니다. 확장은 O(n)이지만 2배씩 늘어 평균 O(1)이고, keys와 free는 O(capacity + n)입니다.

### 3.3 MinHeap

**구조** ([MinHeap.h](/c-practice/MinHeap.h) [L9-18](/c-practice/MinHeap.h#L9))

```text
items (array)   idx:   0    1    2    3
                      [10] [30] [40] [50]

tree view:             10 (0)
                      /      \
                 30 (1)      40 (2)
                 /
            50 (3)

parent = (i - 1) / 2,  left = 2i + 1,  right = 2i + 2
```

완전 이진 트리를 배열에 왼쪽부터 채운 형태이고, items는 malloc으로 잡은 `HeapItem` 배열입니다. 부모는 항상 자식 이하이므로 가장 빠른 만료가 items[0]에 있습니다.

**핵심 연산**

| 연산 | 하는 일 | 코드 |
| --- | --- | --- |
| `minheap_init(heap, capacity)` | capacity칸 배열을 malloc, size 0 | [L69-80](/c-practice/MinHeap.c#L69) |
| `minheap_push(heap, expire_at, key)` | 가득 차면 realloc 2배, key를 복사해 끝에 넣고 위로 올림 | [L99-121](/c-practice/MinHeap.c#L99) |
| `minheap_pop(heap, out_item)` | items[0]을 넘기고, 마지막 항목을 0번으로 옮겨 아래로 내림 | [L129-139](/c-practice/MinHeap.c#L129) |
| `minheap_peek(heap, out_item)` | items[0]을 복사해 넘김. key는 빌린 주소 | [L147-153](/c-practice/MinHeap.c#L147) |
| `minheap_size(heap)` | 저장된 항목 개수 | [L156-158](/c-practice/MinHeap.c#L156) |
| `minheap_free(heap)` | 남은 항목의 key를 모두 free한 뒤 배열 free | [L86-91](/c-practice/MinHeap.c#L86) |

#### push → _heapify_up ([MinHeap.c](/c-practice/MinHeap.c) [L25-35](/c-practice/MinHeap.c#L25))

배열 끝에 넣고, 부모보다 작으면 자리를 바꾸며 올라갑니다.

```text
start:            [30] [50] [40]
push 10 at idx 3: [30] [50] [40] [10]
10 < parent 50:   [30] [10] [40] [50]     (swap idx 3 <-> 1)
10 < parent 30:   [10] [30] [40] [50]     (swap idx 1 <-> 0, stop at i = 0)
```

i가 size_t라 0에서 1을 빼면 아주 큰 수가 되므로, `while (i > 0)`으로 먼저 확인한 뒤 부모 인덱스를 계산합니다.

#### pop → _heapify_down ([MinHeap.c](/c-practice/MinHeap.c) [L43-62](/c-practice/MinHeap.c#L43))

items[0]을 넘기고, 마지막 항목을 0번으로 옮긴 뒤 더 작은 자식과 바꾸며 내려갑니다.

```text
start:             [10] [30] [40] [50]
take 10, last->0:  [50] [30] [40] (50)    (size 4 -> 3, idx 3 is unused)
50 > min child 30: [30] [50] [40]         (swap idx 0 <-> 1)
idx 1 has no child (2*1+1 = 3 >= size): stop
```

```c
    while (i * 2 + 1 < heap->size) {
        long long ex_time = heap->items[i].expire_at;
        long long min_time = heap->items[i * 2 + 1].expire_at;
        size_t next_idx = i * 2 + 1;

        if (i * 2 + 2 < heap->size) {
            if (heap->items[i * 2 + 2].expire_at < min_time) {
                min_time = heap->items[i * 2 + 2].expire_at;
                next_idx += 1;
            }
        }
        if (ex_time <= min_time) break;
```

반복 조건은 "왼쪽 자식이 있는가"이고, 안쪽 if는 "오른쪽 자식도 있는가"입니다. 자식이 없는 경우와 오른쪽만 없는 경우를 따로 처리합니다.

**왜 이렇게 했나**

- realloc 결과를 임시 포인터로 받습니다. 실패하면 NULL이 오는데 그대로 items에 넣으면 원래 배열을 잃기 때문에, 성공했을 때만 바꿉니다.
- push는 key를 strcpy로 복사해 힙이 소유합니다. Store가 키를 지워도 힙 항목의 key는 남아 있어야 나중에 꺼내서 비교할 수 있습니다.
- pop은 key 소유권을 호출한 쪽에 넘기고(호출한 쪽이 free), peek은 힙의 key를 빌려 주므로 free하면 안 됩니다.

> **Q5. 왜 정렬된 리스트나 전체 순회 대신 최소 힙인가?**
> TTL 처리에 필요한 것은 "지금 가장 먼저 만료되는 키"를 반복해서 아는 것뿐이고, 전체 정렬은 필요 없습니다.
> 정렬 배열은 삽입마다 O(n) 이동이 들고, 매 명령마다 전체 키를 훑으면 O(n)입니다. 힙은 peek O(1), push·pop O(log n)으로 이 질문에만 빠르게 답합니다. ([MinHeap.c](/c-practice/MinHeap.c) [L147-153](/c-practice/MinHeap.c#L147))

**복잡도**: peek·size O(1), push·pop O(log n). 확장은 O(n)이지만 2배씩 늘어 평균 O(1)이 더해질 뿐입니다.

## 4. Store: 자료구조를 묶는 방법

[Store.h](/c-practice/Store.h)의 `Store` 구조체가 세 자료구조와 used_memory·maxmemory·evicted_keys를 가집니다. 모든 store 함수는 맨 앞에서 expire_if_needed를 부르고, 해시맵은 LRU·TTL을 모르며 그 갱신은 모두 Store.c가 합니다.

### 4.1 LRU (map + list)

```text
map.buckets[i]:  HEAD -> [k] <- TAIL
                          | data
                          v
                 Data { key, value, lru_node, expire_at }
                          ^ data              | lru_node
                          |                   v
lru:             HEAD -> [x] <-> [k] <-> [y] <- TAIL
                 (recent)                (oldest)

[k] in buckets and [k] in lru are two different Node, same Data
```

같은 Data를 버킷 Node와 LRU Node 두 개가 가리키고, `Data.lru_node`가 LRU 쪽 노드 주소를 직접 기억합니다.

| 연산 | 하는 일 | 코드 |
| --- | --- | --- |
| `store_set` 새 키 | hashmap_put 후 LRU Node를 malloc, Data와 서로 연결하고 insert_front | [L126-137](/c-practice/Store.c#L126) |
| `store_set` 기존 키 | lru_node로 바로 move_to_front, TTL 없앰 | [L138-141](/c-practice/Store.c#L138) |
| `store_get` | 찾았을 때만 move_to_front 후 값 전달, 없으면 0 | [L154-163](/c-practice/Store.c#L154) |
| `hashmap_contains` | get을 부르지 않아 LRU를 바꾸지 않음 | [L173-176](/c-practice/HashMap.c#L173) |
| `store_free` | LRU Node만 free한 뒤 hashmap_free, minheap_free | [L239-250](/c-practice/Store.c#L239) |

```c
    if (data == NULL) {
        Node* new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            hashmap_remove(map, key);
            return STORE_ERR_ALLOC;
        }
        Data* new_data = hashmap_get_data(map, key);
        new_node->data = new_data;
        new_data->lru_node = new_node;
        insert_front(&store->lru, new_data->lru_node);
    }
    else {
        move_to_front(&store->lru, data->lru_node);
        data->expire_at = NO_EXPIRE; // SET 덮어쓰기는 TTL을 없앤다. 힙의 예전 항목은 expire_if_needed가 버린다
    }
```

LRU Node 할당이 실패하면 방금 넣은 키를 hashmap_remove로 되돌려, 해시맵에만 있고 LRU에는 없는 키가 생기지 않게 합니다. LRU Node의 data는 빌린 주소라 store_free는 Node만 free하고, Data는 hashmap_free가 해제합니다.

> **Q3. 왜 해시맵만으로도, 리스트만으로도 O(1) LRU가 안 되나?**
> 해시맵은 키를 빨리 찾지만 사용 순서가 없어서, 가장 오래된 키를 찾으려면 전부 훑어야 합니다(O(n)). 리스트는 순서가 있지만 key로 노드를 찾으려면 O(n)입니다(3.1 복잡도).
> 둘을 합치고 `Data.lru_node`로 연결하면 hashmap_get_data(평균 O(1)) → lru_node(O(1)) → move_to_front·remove_node(O(1))가 되어 전체가 O(1)입니다. ([Store.c](/c-practice/Store.c) [L139](/c-practice/Store.c#L139), [L159](/c-practice/Store.c#L159))

### 4.2 TTL (heap + lazy deletion)

| 연산 | 하는 일 | 코드 |
| --- | --- | --- |
| `store_expire` | 없으면 0, 0 이하면 delete_data, 아니면 힙에 push하고 expire_at 기록 | [L192-209](/c-practice/Store.c#L192) |
| `store_ttl` | 없으면 -2, NO_EXPIRE면 -1, 아니면 남은 ms / 1000을 out_ttl로 | [L216-232](/c-practice/Store.c#L216) |
| `store_set` 덮어쓰기 | expire_at을 NO_EXPIRE(0)로 바꿔 TTL 삭제 | [L140](/c-practice/Store.c#L140) |
| `expire_if_needed` | 힙 맨 위부터 만료된 항목을 꺼내 진짜 만료만 지움 | [L70-90](/c-practice/Store.c#L70) |

store_expire는 힙 push를 먼저 하고 성공했을 때만 expire_at을 바꾸므로, push가 실패하면 이전 TTL이 그대로 남습니다. -2·-1도 Redis 응답 값이라, 반환값이 아니라 out_ttl로 넘기고 반환값은 성공·에러만 나타냅니다.

**만료 정리** ([Store.c](/c-practice/Store.c) [L70-90](/c-practice/Store.c#L70))

```c
    while (1) {
        res = minheap_peek(&store->heap, &item);
        if (res == 0) return;
        if (item.expire_at > now) return;

        minheap_pop(&store->heap, &item);

        Data* found = hashmap_get_data(&store->map, item.key);
        // 키가 있고 만료 시각도 같을 때만 진짜 만료. 아니면 오래된 기록이라 버리기만 한다.
        if (found != NULL && found->expire_at == item.expire_at) {
            delete_data(store, found);
        }

        free(item.key);
    }
```

힙 맨 위가 아직 안 지났으면 나머지도 안 지났으므로 바로 멈춥니다. 맨 위가 지났으면 꺼내서, 해시맵의 현재 expire_at과 같을 때만 진짜 만료로 지웁니다. pop으로 key 소유권을 받았으므로, 지웠든 버렸든 마지막에 `free(item.key)`를 합니다.

**오래된 항목을 버리는 예** ([store_test.c](/c-practice/store_test.c) [L172-179](/c-practice/store_test.c#L172))

```text
now=3000  SET b; EXPIRE b 10   -> heap: [(13000,b)]               b.expire_at=13000
now=4000  EXPIRE b 20          -> heap: [(13000,b), (24000,b)]    b.expire_at=24000
now=14000 GET b
          peek (13000,b) <= 14000 -> pop -> 13000 != 24000 -> discard (b stays)
          peek (24000,b) >  14000 -> stop
          GET b -> found=1
```

SET으로 덮어쓰면 expire_at이 NO_EXPIRE가 되므로, 힙에 남은 예전 항목은 시각이 맞지 않아 버려집니다.

오래된 항목이 남기 때문에 힙 크기 m은 살아 있는 키 수 n보다 커질 수 있고, push·pop은 O(log m)입니다.

> **Q6. 왜 EXPIRE를 다시 걸거나 DEL할 때 힙에서 바로 지우지 않고 나중에 버리나?**
> 힙은 맨 위만 빨리 알 수 있고, 특정 key가 배열 어디에 있는지는 O(m)으로 찾아야 합니다. 찾은 뒤에도 자리를 다시 맞춰야 합니다.
> 대신 꺼낼 때 expire_at을 비교하면 재설정·SET 덮어쓰기·DEL·DEL 후 같은 이름 재SET이 모두 한 줄로 걸러집니다. ([Store.c](/c-practice/Store.c) [L84](/c-practice/Store.c#L84))

### 4.3 메모리 제한과 TAIL 제거

| 연산 | 하는 일 | 코드 |
| --- | --- | --- |
| `entry_size(key, value)` | `strlen(key)` + value의 10진수 자릿수 | [L32-34](/c-practice/Store.c#L32) |
| `delete_data(store, data)` | used_memory 감소 → LRU 노드 제거 → hashmap_remove | [L42-49](/c-practice/Store.c#L42) |
| `evict_if_needed(store)` | 제한을 넘는 동안 TAIL의 Data를 delete_data, evicted_keys + 1 | [L56-61](/c-practice/Store.c#L56) |

C는 value가 int라 크기를 자릿수로 대신하고(-5 → 2), Python은 미션 공식대로 value 문자열의 UTF-8 바이트 수를 씁니다. delete_data는 DEL·EXPIRE 0·TTL 만료·LRU 제거가 함께 쓰므로 used_memory 감소가 이 한 곳에만 있습니다. hashmap_remove가 data와 key를 free하므로 그 호출이 마지막이고, 그 뒤에는 data를 읽지 않습니다.

**TAIL부터 제거** ([Store.c](/c-practice/Store.c) [L56-61](/c-practice/Store.c#L56))

```c
    while (store->maxmemory > 0 && store->used_memory > store->maxmemory && store->lru.TAIL != NULL) {
        delete_data(store, store->lru.TAIL->data);
        store->evicted_keys++;
    }
```

**C 확인 예시 (maxmemory 10)** ([store_test.c](/c-practice/store_test.c) [L101-107](/c-practice/store_test.c#L101))

```text
store_set a 111   1 + 3 = 4   used=4    lru: HEAD -> [a] <- TAIL
store_set b 222   1 + 3 = 4   used=8    lru: HEAD -> [b] <-> [a] <- TAIL
store_get a                             lru: HEAD -> [a] <-> [b] <- TAIL
store_set c 333   1 + 3 = 4   used=12   lru: HEAD -> [c] <-> [a] <-> [b] <- TAIL
  12 > 10 -> delete_data(TAIL = b)      used=8, evicted_keys=1
                                        lru: HEAD -> [c] <-> [a] <- TAIL
  8 <= 10 -> stop
```

store_test의 기대값(LRU `c a`, used 8, evicted 1)과 같습니다. 미션 8절 예시(문자열 value)는 Python에서 11 + 9 + 13 = 33 → TAIL인 user:1 제거 → 22가 되어, 미션 문서의 `used_memory:22`, `evicted_keys:1`과 같습니다(test_store.py의 test_mission_example).

> **Q7. SET 한 번에서 used_memory는 어떤 순서로 바뀌고, 왜 OOM 검사를 저장 전에 하나?**
> 순서: 만료 정리 → new_memory 계산 → OOM 검사 → 기존 키면 prev_memory 계산 → hashmap_put·LRU 갱신 → `used_memory -= prev_memory; += new_memory` → evict_if_needed.
> 엔트리 하나가 maxmemory보다 크면 다른 키를 다 지워도 들어갈 수 없으므로, 저장 전에 거부해야 기존 키와 덮어쓰려던 기존 값이 그대로 남습니다.
> 기존 키는 옛 크기를 빼고 새 크기를 더해야 값이 짧아지거나 길어질 때 모두 맞습니다. ([Store.c](/c-practice/Store.c) [L108-146](/c-practice/Store.c#L108), [L113-115](/c-practice/Store.c#L113))

> **Q8. 왜 TTL 만료로 지운 키는 evicted_keys에 세지 않나?**
> evicted_keys는 메모리가 부족해서 원치 않게 지워진 키의 수를 보여 주는 지표입니다. 만료는 사용자가 정한 수명이 끝난 정상 삭제입니다.
> 그래서 카운트는 evict_if_needed 안에만 있고, expire_if_needed는 delete_data만 부릅니다. ([Store.c](/c-practice/Store.c) [L59](/c-practice/Store.c#L59), [L85](/c-practice/Store.c#L85))

### 헷갈리는 개념 비교

| 비교 | A | B | 차이 |
| --- | --- | --- | --- |
| 해시값 / 버킷 번호 | `hashmap_hash(key)`: size_t 값 | hash % capacity | 해시값은 capacity와 무관하고, 버킷 번호는 확장하면 바뀜 |
| capacity / count | 버킷(배열 칸) 수 | 저장된 서로 다른 키 수 | 로드 팩터 = count / capacity |
| LRU 제거 / TTL 만료 | evict_if_needed, TAIL부터 | expire_if_needed, 힙 맨 위부터 | 제거만 evicted_keys를 셈. 둘 다 delete_data 사용 |
| OOM 거부 / LRU 제거 | 단일 엔트리 > maxmemory: 저장 안 함 | 저장 후 합계 > maxmemory: 오래된 키 삭제 | OOM은 저장 전, 제거는 저장 후 |
| peek의 key / pop의 key | 힙이 가진 문자열을 빌림 | 소유권이 호출한 쪽으로 넘어옴 | pop한 key만 호출한 쪽이 free |
| C 크기 / Python 크기 | strlen(key) + value 자릿수 | key·value의 UTF-8 바이트 수 | C는 value가 int라 대체, Python이 미션 공식 |

## 5. C → Python 포팅

| 항목 | C | Python | 달라진 이유 |
| --- | --- | --- | --- |
| 해제와 할당 실패 | Node·key·Data를 free, 할당 실패 시 되돌리고 0, `STORE_ERR_ALLOC` | 참조를 끊으면 GC가 회수, pop한 칸은 None으로 비움, 실패 분기 없음 | Python은 메모리를 직접 해제하지 않고 할당 실패를 처리할 일이 없음 |
| 데이터 소유권 | `void* data`, 소유자가 해제. 힙은 key를 strcpy한 복사본을 소유 | data는 객체 참조. 같은 Data를 두 Node가 공유, 힙 key는 문자열을 그대로 저장 | 해제할 소유자가 필요 없고 str은 불변이라 복사하지 않아도 안전 |
| 없음 표현 | NULL, out 파라미터(out_value, out_item, out_ttl) | None, 반환값. find_node는 (node, list) 튜플 | 여러 값을 반환할 수 있어 out 파라미터가 필요 없음 |
| 해시 함수 | size_t가 2^64로 감김 | 매 단계 `& SIZE_MASK`로 64비트 자름 | Python 정수는 overflow가 없음 |
| 배열 할당과 확장 | calloc으로 버킷, malloc·realloc으로 힙 2배 | 고정 길이 list를 만들고, 확장은 2배 list를 새로 만들어 인덱스로 복사 | 미션이 허용한 "고정 길이 배열" 수준으로만 list 사용 |
| 값과 메모리 크기 | int value, strlen(key) + 자릿수 | str value, key·value UTF-8 바이트 수 | 미션 공식 len(utf8(key)) + len(utf8(value)) |
| 함수 → 메서드 | `hashmap_put(&map, k, v)` | `map.put(k, v)` (첫 인자가 self) | Python 관례, 미션의 메서드 이름과 일치 |
| 기타 변경 | store_del, 힙 size 필드, keys 출력, `/`, init·free 함수 | delete, 힙 count 필드, keys 배열 반환, `//`, `__init__`. Store에 exists·dbsize·keys·info_memory 추가 | 예약어·이름 충돌, 정수 나눗셈, CLI 출력 분리 |

## 6. 기능 정리 (부록)

| 명령 | 하는 일 | 쓰는 자료구조 연산 (C 함수) |
| --- | --- | --- |
| `SET key value` | 저장·덮어쓰기, TTL 삭제, 초과 시 LRU 제거 | hashmap_put, insert_front·move_to_front, evict_if_needed |
| `GET key` | 값 반환, 성공 시 LRU 갱신 | hashmap_get_data, move_to_front |
| `DEL key` | 삭제 후 1/0 | delete_data(remove_node, hashmap_remove) |
| `EXISTS key` | 있으면 1, LRU 갱신 없음 | hashmap_contains |
| `DBSIZE` | 키 개수 | hashmap_size |
| `KEYS` | 키 목록, 없으면 (empty array) | hashmap_keys |
| `EXPIRE key seconds` | 만료 설정, 0 이하는 즉시 삭제 | minheap_push 또는 delete_data |
| `TTL key` | -2 / -1 / 남은 초 | hashmap_get_data |
| `CONFIG SET maxmemory n` | 제한 값 변경(즉시 제거 없음) | Store.maxmemory 대입 |
| `INFO memory` | used_memory·maxmemory·evicted_keys 3줄 | Store 필드 읽기 |
| `exit` / `quit` | REPL 종료 | - |

모든 키 명령은 먼저 expire_if_needed(minheap_peek·minheap_pop)를 부릅니다. C는 확인용 main이 store 함수를 직접 부르고, 명령 입력·파싱·에러 출력은 Python [main.py](/python/main.py)에만 있습니다.

<details>
<summary>실행·테스트 방법</summary>

프로젝트 루트에서 실행합니다.

**C (사용자 확인 완료)**

```bash
mkdir -p build
gcc -Wall -Wextra -g -fsanitize=address,undefined -o build/store c-practice/store_test.c c-practice/Store.c c-practice/HashMap.c c-practice/LinkedList.c c-practice/MinHeap.c
./build/store
```
→ 모든 줄의 값이 `(기대: ...)`와 같고 sanitizer 오류가 없음

```bash
gcc -Wall -Wextra -g -o build/store c-practice/store_test.c c-practice/Store.c c-practice/HashMap.c c-practice/LinkedList.c c-practice/MinHeap.c
leaks --atExit -- ./build/store
```
→ `0 leaks for 0 total leaked bytes`

**Python (사용자 미검증)**

```bash
python3 -m unittest discover -s tests -v
```
→ 마지막 줄 `Ran 30 tests ... OK`

```bash
python3 python/main.py
```
→ `mini-redis>` 프롬프트. 미션 8절 예시를 입력하면 `INFO memory`가 `used_memory:22` / `maxmemory:30` / `evicted_keys:1`

</details>

| 증상 | 원인 | 해결 |
| --- | --- | --- |
| sanitizer `heap-use-after-free` | hashmap_remove·remove_node 뒤에 data나 Node를 읽음 | 필요한 값을 해제 전에 꺼내 두고, 해제 호출을 마지막에 |
| macOS에서 sanitizer로 누수가 안 잡힘 | macOS는 LeakSanitizer 미지원 | sanitizer 없이 빌드해 leaks로 확인 |
| `EXPIRE k 3` 직후 `TTL k`가 2 | 남은 ms를 1000으로 내림, 입력 사이에 ms가 흐름 | 정상. 미션 예시와 같음 |
| `KEYS *`가 인자 개수 에러 | 패턴 매칭 미구현(미션 범위 밖) | 인자 없이 `KEYS` |
| 한글 키의 used_memory가 글자 수보다 큼 | Python은 UTF-8 바이트 수로 셈(한글 3바이트) | 정상. 미션 공식 |
| maxmemory를 낮춰도 키가 그대로 | LRU 제거는 SET 이후에만 실행 | 다음 SET에서 제거됨 |
| `Ran 0 tests` | 실행 위치나 `-s tests` 누락 | 프로젝트 루트에서 위 명령 실행 |

**미검증·한계**

- Python 전체 unittest와 REPL 시나리오는 Claude만 실행했고 사용자는 미검증입니다. C 최소 힙 확인용 main은 저장소에 남아 있지 않아, 지금은 Python test_min_heap.py로만 다시 확인할 수 있습니다.
- C는 value가 int이고 CLI가 없어, 문자열 value와 명령 입력은 Python에서만 동작합니다.
- 만료는 명령이 들어올 때만 정리하며, 백그라운드 정리는 없습니다.
- 같은 키에 EXPIRE를 반복하면 오래된 힙 항목이 만료 시각까지 남아 힙 크기 m이 커집니다.
- 해시맵과 힙은 늘어나기만 하고 줄어들지 않습니다.
