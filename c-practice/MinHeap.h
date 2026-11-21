#ifndef MINHEAP_H
#define MINHEAP_H

#include <stddef.h>

typedef struct HeapItem HeapItem;
typedef struct MinHeap MinHeap;

struct HeapItem {
    long long expire_at; // 만료 시각(밀리초)
    char* key;           // 힙이 복사해 소유하는 문자열
};

struct MinHeap {
    HeapItem* items;  // 동적 배열
    size_t size;      // 저장된 항목 개수
    size_t capacity;  // items 배열의 칸 수
};

/*
 * 반환: 성공 1, 실패(할당 실패·빈 힙) 0.
 * 소유권: pop의 out_item->key는 호출한 쪽이 free한다.
 *         peek의 out_item->key는 빌린 주소이므로 해제하지 않는다.
 */
int minheap_init(MinHeap* heap, size_t capacity);
void minheap_free(MinHeap* heap);
int minheap_push(MinHeap* heap, long long expire_at, const char* key);
int minheap_pop(MinHeap* heap, HeapItem* out_item);
int minheap_peek(const MinHeap* heap, HeapItem* out_item);
size_t minheap_size(const MinHeap* heap);

#endif
