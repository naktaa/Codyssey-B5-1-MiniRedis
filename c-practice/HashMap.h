#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

#include "LinkedList.h"

typedef struct Data {
    char* key; // 문자열 주소. 복사·할당·해제 책임은 구현 전에 정한다.
    int value;
    Node* lru_node; // Store의 LRU 리스트에서 이 키를 담은 노드. 해시맵은 읽거나 해제하지 않는다.
} Data;

typedef struct HashMap {
    // 각 버킷이 첫 Entry의 주소를 저장하므로 배열의 원소 타입은 Entry*이다.
    // 그 배열을 동적으로 관리할 시작 주소이므로 Entry**로 표현한다.
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
