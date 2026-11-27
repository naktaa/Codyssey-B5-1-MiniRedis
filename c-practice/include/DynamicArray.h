#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stddef.h>

typedef struct DynamicArray DynamicArray;

struct DynamicArray {
    int* items;       // 실제 배열. 0 ~ size-1 칸만 유효하다.
    size_t size;      // 저장된 원소 개수
    size_t capacity;  // items 배열의 칸 수
};

/*
 * 반환: 성공 1, 실패(할당 실패·범위 밖 인덱스) 0.
 * 인덱스는 0 이상 size 미만만 유효하다. set은 있는 칸만 바꾸고, 새 칸은 append로만 만든다.
 */
int dynarray_init(DynamicArray* arr, size_t capacity);
void dynarray_free(DynamicArray* arr);
int dynarray_append(DynamicArray* arr, int value);
int dynarray_get(const DynamicArray* arr, size_t index, int* out_value);
int dynarray_set(DynamicArray* arr, size_t index, int value);
int dynarray_remove(DynamicArray* arr, size_t index);

#endif
