#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

#include "LinkedList.h"

typedef struct Data {
    char* key; // 문자열 주소. 복사·할당·해제 책임은 구현 전에 정한다.
    int value;
    Node* lru_node; // Store의 LRU 리스트에서 이 키를 담은 노드. 해시맵은 읽거나 해제하지 않는다.
    long long expire_at; // 만료 시각(ms). Store가 관리하며 Store.h의 NO_EXPIRE면 만료 없음. 해시맵은 읽지 않는다.
} Data;

typedef struct HashMap {
    // 버킷 배열. 각 칸이 한 버킷의 LinkedList이고, 그 Node의 data가 Data*를 가리킨다.
    LinkedList* buckets;
    size_t capacity; // 버킷 개수. 항목 개수와 다르며, 사용 전 0보다 커야 한다.
    size_t count;    // 저장된 서로 다른 키의 개수.
} HashMap;

size_t hashmap_hash(const char* key);
int hashmap_put(HashMap* map, const char* key, int value);
int hashmap_get(const HashMap* map, const char* key, int* out_value);
int hashmap_remove(HashMap* map, const char* key);
int hashmap_contains(const HashMap* map, const char* key);
void hashmap_keys(const HashMap* map);
size_t hashmap_size(const HashMap* map);
Data* hashmap_get_data(const HashMap* map, const char* key);
void hashmap_free(HashMap* map);

#endif
