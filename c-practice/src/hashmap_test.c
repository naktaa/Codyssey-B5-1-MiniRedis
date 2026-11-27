#include <stdio.h>
#include <stdlib.h>

#include "HashMap.h"

#define CAP 100

/* 실습 확인용 main. 생성·정리 함수 대신 여기서 직접 준비하고 해제한다. */
int main(void) {
    HashMap map;
    map.capacity = CAP;
    map.count = 0;
    map.buckets = calloc(map.capacity, sizeof(LinkedList)); // 모든 HEAD/TAIL = NULL
    if (map.buckets == NULL) {
        return 1;
    }

    int value = 0;

    printf("== put / get ==\n");
    hashmap_put(&map, "user:1", 10);
    hashmap_put(&map, "user:2", 20);
    hashmap_put(&map, "user:1", 11); // 기존 키 갱신
    int found = hashmap_get(&map, "user:1", &value);
    printf("get user:1   -> found=%d value=%d (기대: 1, 11)\n", found, value);
    printf("get none     -> found=%d (기대: 0)\n", hashmap_get(&map, "none", &value));
    printf("size         -> %zu (기대: 2)\n", hashmap_size(&map));

    printf("\n== contains ==\n");
    printf("user:2       -> %d (기대: 1)\n", hashmap_contains(&map, "user:2"));
    printf("none         -> %d (기대: 0)\n", hashmap_contains(&map, "none"));

    printf("\n== keys ==\n");
    hashmap_keys(&map);

    printf("\n== remove ==\n");
    printf("remove user:1 -> %d (기대: 1)\n", hashmap_remove(&map, "user:1"));
    printf("remove user:1 -> %d (기대: 0)\n", hashmap_remove(&map, "user:1"));
    printf("size          -> %zu (기대: 1)\n", hashmap_size(&map));
    hashmap_remove(&map, "user:2");
    printf("keys (빈 맵):\n");
    hashmap_keys(&map);

    printf("\n== 버킷 확장 ==\n");
    char key[32];
    for (int i = 0; i < 200; i++) {
        snprintf(key, sizeof(key), "key:%d", i);
        hashmap_put(&map, key, i);
    }
    int all_ok = 1;
    for (int i = 0; i < 200; i++) {
        snprintf(key, sizeof(key), "key:%d", i);
        if (!hashmap_get(&map, key, &value) || value != i) {
            all_ok = 0;
        }
    }
    printf("size=%zu capacity=%zu (기대: 200, 400)\n", hashmap_size(&map), map.capacity);
    printf("확장 후 200개 모두 조회 -> %d (기대: 1)\n", all_ok);

    // 정리: key → Data → Node 순서로 해제하고 마지막에 버킷 배열을 해제한다.
    for (size_t i = 0; i < map.capacity; i++) {
        Node* now = map.buckets[i].HEAD;
        while (now) {
            Node* next = now->next; // free 후에는 now->next를 읽을 수 없다
            Data* data = now->data;
            free(data->key);
            free(data);
            free(now);
            now = next;
        }
    }
    free(map.buckets);

    return 0;
}
