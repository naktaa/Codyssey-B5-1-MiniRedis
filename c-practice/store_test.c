#include <stdio.h>
#include <stdlib.h>

#include "Store.h"

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

    int value = 0;

    printf("== set ==\n");
    store_set(&store, "a", 1);
    store_set(&store, "b", 2);
    store_set(&store, "c", 3);
    print_lru(&store); // 기대: c b a

    printf("\n== get ==\n");
    int found = store_get(&store, "a", &value);
    printf("get a    -> found=%d value=%d (기대: 1, 1)\n", found, value);
    print_lru(&store); // 기대: a c b
    store_get(&store, "a", &value); // 이미 맨 앞
    print_lru(&store); // 기대: a c b
    printf("get none -> %d (기대: 0)\n", store_get(&store, "none", &value));

    printf("\n== set 기존 키 ==\n");
    store_set(&store, "b", 20);
    store_get(&store, "b", &value);
    printf("get b    -> value=%d (기대: 20)\n", value);
    print_lru(&store); // 기대: b a c
    printf("size     -> %zu (기대: 3)\n", hashmap_size(&store.map));

    printf("\n== del ==\n");
    printf("del c    -> %d (기대: 1)\n", store_del(&store, "c")); // TAIL 삭제
    print_lru(&store); // 기대: b a
    printf("del b    -> %d (기대: 1)\n", store_del(&store, "b")); // HEAD 삭제
    print_lru(&store); // 기대: a
    printf("del b    -> %d (기대: 0)\n", store_del(&store, "b"));
    printf("size     -> %zu (기대: 1)\n", hashmap_size(&store.map));

    printf("\n== 확장 후 LRU ==\n");
    char key[16];
    for (int i = 0; i < 20; i++) {
        snprintf(key, sizeof(key), "k%d", i);
        store_set(&store, key, i);
    }
    store_get(&store, "a", &value);
    printf("capacity -> %zu (기대: 4보다 큼)\n", store.map.capacity);
    print_lru(&store); // 기대: a k19 k18 ... k0
    size_t lru_count = 0;
    for (Node* now = store.lru.HEAD; now; now = now->next) {
        lru_count++;
    }
    printf("LRU 개수 -> %zu, size -> %zu (기대: 21, 21)\n", lru_count, hashmap_size(&store.map));

    store_free(&store);
    return 0;
}
