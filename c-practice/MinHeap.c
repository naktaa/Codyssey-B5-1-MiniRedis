#include <stdlib.h>
#include <string.h>

#include "MinHeap.h"

/*
 * C-3 최소 힙: TTL 만료 관리를 위해 (expire_at, key)를 expire_at이 작은 순서로 꺼낸다.
 * 배열 기반 완전 이진 트리이며 0번 인덱스부터 사용한다.
 *   부모 = (i - 1) / 2, 왼쪽 자식 = 2i + 1, 오른쪽 자식 = 2i + 2
 * 부모의 expire_at은 항상 자식 이하이므로 가장 빨리 만료되는 항목이 items[0]에 있다.
 * expire_at은 C-3 테스트에서 실제 시간 없이 숫자만 넣는다.
 *
 * 구현 순서 권장:
 *   init/free → heapify_up/push → peek/size → heapify_down/pop
 *   → 무작위 push 후 전부 pop해서 오름차순인지 확인
 *
 * 아래 원형의 ;를 지우고 { } 본문을 직접 작성한다.
 */

/*
 * i번 항목을 부모와 비교하며 위로 올린다. push에서 배열 끝에 넣은 직후 호출한다.
 * TODO: while (i > 0) 동안 부모보다 expire_at이 작으면 자리를 바꾸고 i를 부모로 옮기기.
 *       부모 이하가 되면 멈춘다. i는 size_t이므로 0에서 빼지 않도록 조건을 먼저 본다.
 */
static void _heapify_up(MinHeap* heap, size_t i);

/*
 * i번 항목을 더 작은 자식과 비교하며 아래로 내린다. pop에서 마지막 항목을 0번으로 옮긴 뒤 호출한다.
 * TODO: 자식 인덱스가 size 이상이면 그 자식은 없다. 자식이 둘 다 없는 경우와
 *       오른쪽 자식만 없는 경우를 구분하기.
 * TODO: 더 작은 자식보다 크면 자리를 바꾸고 i를 그 자식으로 옮기기. 아니면 멈춘다.
 */
static void _heapify_down(MinHeap* heap, size_t i);

/*
 * 빈 힙을 준비한다. capacity는 0보다 커야 한다(0이면 2배로 늘려도 0이다).
 * 반환: 성공 1, 할당 실패 0.
 * TODO: capacity만큼 items를 할당하고 size와 capacity의 초기 값을 정하기.
 */
int minheap_init(MinHeap* heap, size_t capacity) {
    HeapItem* InitItems = malloc(sizeof(HeapItem) * capacity);
    if (InitItems == NULL) {
        return 0;
    }

    heap->items = InitItems;
    heap->size = 0;
    heap->capacity = capacity;

    return 1;
}

/*
 * 힙이 가진 모든 메모리를 해제한다.
 * TODO: 남아 있는 항목들의 key를 모두 해제한 뒤 items 배열을 해제하기.
 */
void minheap_free(MinHeap* heap) {
    for (size_t i = 0; i < heap->size; i++) {
        free((heap->items + i)->key);
    }
    free(heap->items);
}

/*
 * (expire_at, key)를 추가한다. key는 strlen + 1 크기로 복사해서 저장한다.
 * 반환: 성공 1, 할당 실패 0. 실패하면 힙은 호출 전 상태 그대로여야 한다.
 * TODO: 가득 차면 realloc으로 2배 확장하기. 결과는 임시 포인터로 받아 실패 시 원본을 보존한다.
 * TODO: key 복사본을 만들고 배열 끝에 둔 뒤 _heapify_up으로 제자리를 찾기.
 */
int minheap_push(MinHeap* heap, long long expire_at, const char* key);

/*
 * expire_at이 가장 작은 항목을 꺼내 *out_item에 전달한다.
 * 반환: 성공 1, 빈 힙 0.
 * 소유권: out_item->key는 호출한 쪽으로 넘어간다. 힙은 해제하지 않는다.
 * TODO: items[0]을 전달하고, 마지막 항목을 0번으로 옮긴 뒤 _heapify_down으로 내리기.
 */
int minheap_pop(MinHeap* heap, HeapItem* out_item);

/*
 * expire_at이 가장 작은 항목을 꺼내지 않고 *out_item에 복사한다.
 * 반환: 성공 1, 빈 힙 0.
 * 소유권: out_item->key는 힙이 가진 문자열을 빌린 것이다. 그 항목이 pop되기 전까지만 유효하다.
 * TODO: 직접 구현하기.
 */
int minheap_peek(const MinHeap* heap, HeapItem* out_item);

/* 반환: 현재 저장된 항목 개수. TODO: 직접 구현하기. */
size_t minheap_size(const MinHeap* heap);
