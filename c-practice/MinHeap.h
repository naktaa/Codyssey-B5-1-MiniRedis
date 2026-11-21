#ifndef MINHEAP_H
#define MINHEAP_H

#include <stddef.h>

/*
 * C-3 최소 힙: TTL 만료 관리를 위해 (expire_at, key)를 expire_at이 작은 순서로 꺼낸다.
 * 배열 기반 완전 이진 트리이며 0번 인덱스부터 사용한다.
 *   부모 = (i - 1) / 2, 왼쪽 자식 = 2i + 1, 오른쪽 자식 = 2i + 2
 * 부모의 expire_at은 항상 자식 이하이므로 가장 빨리 만료되는 항목이 items[0]에 있다.
 *
 * 구현 순서 권장:
 *   init/free → heapify_up/push → peek/size → heapify_down/pop
 *   → 무작위 push 후 전부 pop해서 오름차순인지 확인
 */

typedef struct HeapItem {
    long long expire_at; // 만료 시각(밀리초). C-3 테스트에서는 실제 시간 없이 숫자만 넣는다.
    char* key;           // 힙이 strlen + 1 크기로 복사해 소유하는 문자열.
} HeapItem;

typedef struct MinHeap {
    HeapItem* items;  // 동적 배열. 가득 차면 2배로 늘린다.
    size_t size;      // 저장된 항목 개수.
    size_t capacity;  // items 배열의 칸 수. 항목 개수와 다르다.
} MinHeap;

/*
 * 빈 힙을 준비한다. capacity는 0보다 커야 한다(0이면 2배로 늘려도 0이다).
 * 반환: 성공 1, 할당 실패 0.
 * TODO: capacity만큼 items를 할당하고 size와 capacity의 초기 값을 정하기.
 */
int minheap_init(MinHeap* heap, size_t capacity);

/*
 * 힙이 가진 모든 메모리를 해제한다.
 * TODO: 남아 있는 항목들의 key를 모두 해제한 뒤 items 배열을 해제하기.
 */
void minheap_free(MinHeap* heap);

/*
 * (expire_at, key)를 추가한다. key는 복사해서 저장한다.
 * 반환: 성공 1, 할당 실패 0. 실패하면 힙은 호출 전 상태 그대로여야 한다.
 * TODO: 가득 차면 realloc으로 2배 확장하기. 결과는 임시 포인터로 받아 실패 시 원본을 보존한다.
 * TODO: key 복사본을 만들고 배열 끝에 둔 뒤 _heapify_up으로 제자리를 찾기.
 */
int minheap_push(MinHeap* heap, long long expire_at, const char* key);

/*
 * expire_at이 가장 작은 항목을 꺼내 *out_item에 전달한다.
 * 반환: 성공 1, 빈 힙 0.
 * 소유권: out_item->key는 호출한 쪽으로 넘어간다. 다 쓴 뒤 호출한 쪽이 free한다.
 * TODO: items[0]을 전달하고, 마지막 항목을 0번으로 옮긴 뒤 _heapify_down으로 내리기.
 */
int minheap_pop(MinHeap* heap, HeapItem* out_item);

/*
 * expire_at이 가장 작은 항목을 꺼내지 않고 *out_item에 복사한다.
 * 반환: 성공 1, 빈 힙 0.
 * 소유권: out_item->key는 힙이 가진 문자열을 빌린 것이다. 해제하지 않으며,
 *         그 항목이 pop되기 전까지만 유효하다.
 * TODO: 직접 구현하기.
 */
int minheap_peek(const MinHeap* heap, HeapItem* out_item);

/* 반환: 현재 저장된 항목 개수. TODO: 직접 구현하기. */
size_t minheap_size(const MinHeap* heap);

#endif
