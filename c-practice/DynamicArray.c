#include <stdlib.h>

#include "DynamicArray.h"

/*
 * 보너스 1 동적 배열: 칸이 모자라면 2배 크기로 늘어나는 int 배열.
 * size(저장된 개수)와 capacity(배열 칸 수)를 따로 둔다. size == capacity가 되면 다음 append에서 확장한다.
 * MinHeap의 items·size·capacity와 같은 구조이며, 힙은 이 확장 로직을 push 안에 직접 가지고 있다.
 */

/*
 * 빈 배열을 준비한다. capacity는 0보다 커야 한다(0이면 2배로 늘려도 0이다).
 * 반환: 성공 1, 할당 실패 0.
 */
int dynarray_init(DynamicArray* arr, size_t capacity) {
    int* init_items = malloc(sizeof(int) * capacity);
    if (init_items == NULL) {
        return 0;
    }

    arr->items = init_items;
    arr->size = 0;
    arr->capacity = capacity;

    return 1;
}

/* items 배열을 해제한다. 원소가 int라 원소마다 해제할 것은 없다. */
void dynarray_free(DynamicArray* arr) {
    free(arr->items);
    arr->items = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

/*
 * 맨 끝에 value를 추가한다. 가득 차 있으면 먼저 capacity를 2배로 늘린다.
 * 반환: 성공 1, 할당 실패 0. 실패하면 배열은 호출 전 상태 그대로다.
 * 시간: 평균 O(1). 확장하는 순간만 O(n)이지만 2배씩 늘려서 확장은 드물게 일어난다.
 */
int dynarray_append(DynamicArray* arr, int value) {
    if (arr->size == arr->capacity) {
        // realloc 결과를 바로 arr->items에 넣지 않는다. 실패(NULL)하면 원래 주소를 잃어 누수가 나기 때문이다.
        int* tmp_items = realloc(arr->items, sizeof(int) * arr->capacity * 2);
        if (tmp_items == NULL) {
            return 0;
        }
        // realloc은 기존 값을 새 공간 앞쪽에 그대로 옮겨 주므로 직접 복사하지 않아도 된다.
        arr->items = tmp_items;
        arr->capacity *= 2;
    }

    arr->items[arr->size] = value; // 빈 칸 중 맨 앞(= 현재 size 위치)에 넣는다.
    arr->size++;

    return 1;
}

/*
 * index 칸의 값을 out_value에 담는다.
 * 반환: 성공 1, 범위 밖 0. 시간 O(1).
 */
int dynarray_get(const DynamicArray* arr, size_t index, int* out_value) {
    // size 이상 칸은 할당은 되어 있어도 아직 값을 넣지 않은 칸이라 읽으면 안 된다.
    // index가 size_t라 음수는 들어올 수 없으므로 위쪽 경계만 본다.
    if (index >= arr->size) {
        return 0;
    }

    *out_value = arr->items[index];

    return 1;
}

/*
 * 이미 있는 index 칸의 값을 value로 바꾼다. size 이상 칸에 쓰면 중간이 빈 배열이 되므로 막는다.
 * 반환: 성공 1, 범위 밖 0. 시간 O(1).
 */
int dynarray_set(DynamicArray* arr, size_t index, int value) {
    if (index >= arr->size) {
        return 0;
    }

    arr->items[index] = value;

    return 1;
}

/*
 * index 칸을 지우고 뒤쪽 원소를 한 칸씩 앞으로 당긴다. 순서는 그대로 유지된다.
 * 반환: 성공 1, 범위 밖(빈 배열 포함) 0. 시간 O(n): 뒤에 있는 원소 수만큼 옮긴다.
 * 줄어든 뒤에도 capacity는 그대로 둔다(축소는 미션 요구에 없음).
 */
int dynarray_remove(DynamicArray* arr, size_t index) {
    if (index >= arr->size) {
        return 0;
    }

    // 앞에서부터 옮겨야 아직 옮기지 않은 값을 덮어쓰지 않는다. i+1 칸 값을 i 칸으로 당긴다.
    for (size_t i = index; i + 1 < arr->size; i++) {
        arr->items[i] = arr->items[i + 1];
    }
    arr->size--; // 마지막 칸은 이제 쓰지 않는 칸이 된다(값은 남아 있어도 size 밖이라 읽히지 않는다).

    return 1;
}
