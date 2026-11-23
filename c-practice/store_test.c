#include <stdio.h>
#include <stdlib.h>

#include "Store.h"

// TTL과 무관한 구간은 시각을 0으로 고정한다(만료되는 키가 없음).

/* LRU 순서를 HEAD(최근)부터 출력한다. */
static void print_lru(const Store* store) {
    printf("LRU: ");
    for (Node* now = store->lru.HEAD; now; now = now->next) {
        Data* data = now->data;
        printf("%s ", data->key);
    }
    printf("\n");
}

/* 실습 확인용 main. 생성 함수 대신 여기서 직접 준비한다. */
int main(void) {
    Store store;
    store.map.capacity = 4; // 작게 시작해서 확장 후에도 LRU 연결이 유지되는지 확인
    store.map.count = 0;
    store.map.buckets = calloc(store.map.capacity, sizeof(LinkedList));
    if (store.map.buckets == NULL) {
        return 1;
    }
    store.lru.HEAD = store.lru.TAIL = NULL;
    store.used_memory = 0;
    store.maxmemory = 0; // 무제한. 메모리 제한 테스트는 본문 구현 후 추가한다.
    store.evicted_keys = 0;
    if (!minheap_init(&store.heap, 4)) {
        return 1;
    }

    int value = 0;

    printf("== set ==\n");
    store_set(&store, "a", 1, 0);
    store_set(&store, "b", 2, 0);
    store_set(&store, "c", 3, 0);
    print_lru(&store); // 기대: c b a

    printf("\n== get ==\n");
    int found = store_get(&store, "a", &value, 0);
    printf("get a    -> found=%d value=%d (기대: 1, 1)\n", found, value);
    print_lru(&store); // 기대: a c b
    store_get(&store, "a", &value, 0); // 이미 맨 앞
    print_lru(&store); // 기대: a c b
    printf("get none -> %d (기대: 0)\n", store_get(&store, "none", &value, 0));

    printf("\n== set 기존 키 ==\n");
    store_set(&store, "b", 20, 0);
    store_get(&store, "b", &value, 0);
    printf("get b    -> value=%d (기대: 20)\n", value);
    print_lru(&store); // 기대: b a c
    printf("size     -> %zu (기대: 3)\n", hashmap_size(&store.map));

    printf("\n== del ==\n");
    printf("del c    -> %d (기대: 1)\n", store_del(&store, "c", 0)); // TAIL 삭제
    print_lru(&store); // 기대: b a
    printf("del b    -> %d (기대: 1)\n", store_del(&store, "b", 0)); // HEAD 삭제
    print_lru(&store); // 기대: a
    printf("del b    -> %d (기대: 0)\n", store_del(&store, "b", 0));
    printf("size     -> %zu (기대: 1)\n", hashmap_size(&store.map));

    printf("\n== 확장 후 LRU ==\n");
    char key[16];
    for (int i = 0; i < 20; i++) {
        snprintf(key, sizeof(key), "k%d", i);
        store_set(&store, key, i, 0);
    }
    store_get(&store, "a", &value, 0);
    printf("capacity -> %zu (기대: 4보다 큼)\n", store.map.capacity);
    print_lru(&store); // 기대: a k19 k18 ... k0
    size_t lru_count = 0;
    for (Node* now = store.lru.HEAD; now; now = now->next) {
        lru_count++;
    }
    printf("LRU 개수 -> %zu, size -> %zu (기대: 21, 21)\n", lru_count, hashmap_size(&store.map));
    // a=1(2) + k0~k9(각 3) + k10~k19(각 5) = 82
    printf("used     -> %zu, evicted -> %zu (기대: 82, 0 / 무제한이라 제거 없음)\n",
           store.used_memory, store.evicted_keys);
    store_free(&store);

    printf("\n== 메모리 제한 (maxmemory 10) ==\n");
    Store mem;
    mem.map.capacity = 4;
    mem.map.count = 0;
    mem.map.buckets = calloc(mem.map.capacity, sizeof(LinkedList));
    if (mem.map.buckets == NULL) {
        return 1;
    }
    mem.lru.HEAD = mem.lru.TAIL = NULL;
    mem.used_memory = 0;
    mem.maxmemory = 10;
    mem.evicted_keys = 0;
    if (!minheap_init(&mem.heap, 4)) {
        return 1;
    }

    store_set(&mem, "a", 111, 0);         // 4
    store_set(&mem, "b", 222, 0);         // 8
    store_get(&mem, "a", &value, 0);      // a가 최근
    store_set(&mem, "c", 333, 0);         // 12 > 10 → TAIL(b) 제거
    print_lru(&mem); // 기대: c a
    printf("used=%zu evicted=%zu (기대: 8, 1)\n", mem.used_memory, mem.evicted_keys);
    printf("get b    -> %d (기대: 0, 제거됨)\n", store_get(&mem, "b", &value, 0));

    printf("\n== 기존 키 갱신 크기 ==\n");
    store_set(&mem, "c", 3, 0);           // 4 → 2
    printf("줄어듦   -> used=%zu (기대: 6)\n", mem.used_memory);
    store_set(&mem, "a", 11111, 0);       // 4 → 6
    printf("늘어남   -> used=%zu (기대: 8)\n", mem.used_memory);
    print_lru(&mem); // 기대: a c

    printf("\n== OOM ==\n");
    printf("새 키    -> %d (기대: %d)\n", store_set(&mem, "big", 1234567890, 0), STORE_ERR_OOM); // 3+10
    printf("contains -> %d (기대: 0)\n", hashmap_contains(&mem.map, "big"));
    printf("덮어쓰기 -> %d (기대: %d)\n", store_set(&mem, "a", 1234567890, 0), STORE_ERR_OOM);  // 1+10
    store_get(&mem, "a", &value, 0);
    printf("a 유지   -> %d (기대: 11111)\n", value);
    printf("used=%zu evicted=%zu (기대: 8, 1)\n", mem.used_memory, mem.evicted_keys);

    printf("\n== 경계: 딱 maxmemory ==\n");
    printf("set x    -> %d (기대: %d)\n", store_set(&mem, "x", 123456789, 0), STORE_OK); // 1+9 = 10
    print_lru(&mem); // 기대: x  (18 → c 제거 16 → a 제거 10)
    printf("used=%zu evicted=%zu (기대: 10, 3)\n", mem.used_memory, mem.evicted_keys);

    printf("\n== del ==\n");
    store_del(&mem, "x", 0);
    printf("used=%zu (기대: 0)\n", mem.used_memory);
    print_lru(&mem); // 기대: (비어 있음)

    store_free(&mem);

    // TTL 구간: now(ms)를 직접 넣어 시간 흐름을 흉내 낸다. 시각은 앞으로만 간다.
    printf("\n== TTL 기본 ==\n");
    Store ttl;
    ttl.map.capacity = 4;
    ttl.map.count = 0;
    ttl.map.buckets = calloc(ttl.map.capacity, sizeof(LinkedList));
    if (ttl.map.buckets == NULL) {
        return 1;
    }
    ttl.lru.HEAD = ttl.lru.TAIL = NULL;
    ttl.used_memory = 0;
    ttl.maxmemory = 0;
    ttl.evicted_keys = 0;
    if (!minheap_init(&ttl.heap, 4)) {
        return 1;
    }

    long long t = 0;
    store_set(&ttl, "a", 1, 0);
    store_ttl(&ttl, "a", 0, &t);
    printf("ttl a       -> %lld (기대: -1, 만료 없음)\n", t);
    store_ttl(&ttl, "none", 0, &t);
    printf("ttl none    -> %lld (기대: -2, 없는 키)\n", t);
    printf("expire none -> %d (기대: 0)\n", store_expire(&ttl, "none", 10, 0));

    printf("expire a 3  -> %d (기대: 1)\n", store_expire(&ttl, "a", 3, 0)); // 만료 시각 3000
    store_ttl(&ttl, "a", 0, &t);
    printf("ttl a @0    -> %lld (기대: 3)\n", t);
    store_ttl(&ttl, "a", 1, &t);
    printf("ttl a @1    -> %lld (기대: 2, 내림)\n", t);
    store_ttl(&ttl, "a", 2999, &t);
    printf("ttl a @2999 -> %lld (기대: 0)\n", t);
    printf("get a @3000 -> %d (기대: 0, 만료)\n", store_get(&ttl, "a", &value, 3000));
    printf("size=%zu used=%zu evicted=%zu (기대: 0, 0, 0 / 만료는 evicted에 안 셈)\n",
           hashmap_size(&ttl.map), ttl.used_memory, ttl.evicted_keys);

    printf("\n== EXPIRE 재설정 (오래된 힙 항목) ==\n");
    store_set(&ttl, "b", 2, 3000);
    store_expire(&ttl, "b", 10, 3000);  // 13000
    store_expire(&ttl, "b", 20, 4000);  // 24000. 힙에 (13000, b)가 남는다
    printf("get b @14000 -> %d (기대: 1, 13000 항목은 버려짐)\n", store_get(&ttl, "b", &value, 14000));
    store_ttl(&ttl, "b", 14000, &t);
    printf("ttl b @14000 -> %lld (기대: 10)\n", t);
    printf("get b @24000 -> %d (기대: 0, 만료)\n", store_get(&ttl, "b", &value, 24000));

    printf("\n== DEL 후 같은 이름 다시 SET ==\n");
    store_set(&ttl, "c", 1, 30000);
    store_expire(&ttl, "c", 5, 30000);  // 35000
    store_del(&ttl, "c", 30000);
    store_set(&ttl, "c", 2, 30000);     // 새 c. 만료 없음
    found = store_get(&ttl, "c", &value, 35000);
    printf("get c @35000 -> found=%d value=%d (기대: 1, 2)\n", found, value);
    store_ttl(&ttl, "c", 35000, &t);
    printf("ttl c        -> %lld (기대: -1)\n", t);

    printf("\n== SET 덮어쓰기는 TTL을 없앤다 ==\n");
    store_set(&ttl, "d", 1, 40000);
    store_expire(&ttl, "d", 5, 40000);  // 45000
    store_set(&ttl, "d", 2, 40000);
    store_ttl(&ttl, "d", 40000, &t);
    printf("ttl d        -> %lld (기대: -1)\n", t);
    found = store_get(&ttl, "d", &value, 45000);
    printf("get d @45000 -> found=%d value=%d (기대: 1, 2)\n", found, value);

    printf("\n== EXPIRE 0 이하는 즉시 삭제 ==\n");
    store_set(&ttl, "e", 1, 50000);
    printf("expire e 0   -> %d (기대: 1)\n", store_expire(&ttl, "e", 0, 50000));
    printf("get e        -> %d (기대: 0)\n", store_get(&ttl, "e", &value, 50000));

    printf("\n== 만료된 키의 GET은 LRU를 바꾸지 않는다 ==\n");
    store_set(&ttl, "f", 1, 60000);
    store_set(&ttl, "g", 1, 60000);
    store_expire(&ttl, "f", 1, 60000);  // 61000
    print_lru(&ttl); // 기대: g f d c
    printf("get f @61000 -> %d (기대: 0)\n", store_get(&ttl, "f", &value, 61000));
    print_lru(&ttl); // 기대: g d c
    // c=2 + d=2 + g=2
    printf("used=%zu evicted=%zu (기대: 6, 0)\n", ttl.used_memory, ttl.evicted_keys);

    // 힙에 항목이 남은 채로 free → 누수 검사로 store_free의 힙 해제를 확인한다
    store_expire(&ttl, "g", 100, 61000);
    store_free(&ttl);
    return 0;
}
