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
    return 0;
}
