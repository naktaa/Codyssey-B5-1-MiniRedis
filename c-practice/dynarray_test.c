#include <stdio.h>

#include "DynamicArray.h"

/* 배열 내용을 [a, b, c] size=n capacity=m 형식으로 출력한다. */
static void print_array(const DynamicArray* arr) {
    printf("[");
    for (size_t i = 0; i < arr->size; i++) {
        printf(i == 0 ? "%d" : ", %d", arr->items[i]);
    }
    printf("] size=%zu capacity=%zu", arr->size, arr->capacity);
}

/* 실습 확인용 main. 각 줄의 (기대: ...)와 출력이 같은지 확인한다. */
int main(void) {
    DynamicArray arr;
    if (!dynarray_init(&arr, 2)) {
        return 1;
    }
    int value = 0;

    printf("== append와 2배 확장 ==\n");
    dynarray_append(&arr, 10);
    dynarray_append(&arr, 20);
    print_array(&arr);
    printf("  (기대: [10, 20] size=2 capacity=2)\n");
    dynarray_append(&arr, 30); // 가득 찬 상태에서 추가 → 2 → 4
    print_array(&arr);
    printf("  (기대: [10, 20, 30] size=3 capacity=4)\n");
    dynarray_append(&arr, 40);
    dynarray_append(&arr, 50); // 4 → 8
    print_array(&arr);
    printf("  (기대: [10, 20, 30, 40, 50] size=5 capacity=8)\n");

    printf("\n== get ==\n");
    int found = dynarray_get(&arr, 0, &value);
    printf("get 0 -> %d, %d (기대: 1, 10)\n", found, value);
    found = dynarray_get(&arr, 4, &value);
    printf("get 4 -> %d, %d (기대: 1, 50)  마지막 칸\n", found, value);
    printf("get 5 -> %d (기대: 0)  index == size\n", dynarray_get(&arr, 5, &value));
    printf("get 7 -> %d (기대: 0)  capacity 안이지만 값이 없는 칸\n", dynarray_get(&arr, 7, &value));

    printf("\n== set ==\n");
    printf("set 1 = 21 -> %d (기대: 1)\n", dynarray_set(&arr, 1, 21));
    printf("set 5 = 60 -> %d (기대: 0)  새 칸은 append로만\n", dynarray_set(&arr, 5, 60));
    print_array(&arr);
    printf("  (기대: [10, 21, 30, 40, 50] size=5 capacity=8)\n");

    printf("\n== remove ==\n");
    printf("remove 2 -> %d (기대: 1)  중간\n", dynarray_remove(&arr, 2));
    print_array(&arr);
    printf("  (기대: [10, 21, 40, 50] size=4 capacity=8)\n");
    printf("remove 0 -> %d (기대: 1)  맨 앞\n", dynarray_remove(&arr, 0));
    print_array(&arr);
    printf("  (기대: [21, 40, 50] size=3 capacity=8)\n");
    printf("remove 2 -> %d (기대: 1)  맨 끝\n", dynarray_remove(&arr, 2));
    print_array(&arr);
    printf("  (기대: [21, 40] size=2 capacity=8)\n");
    printf("remove 2 -> %d (기대: 0)  index == size\n", dynarray_remove(&arr, 2));

    printf("\n== 빈 배열 ==\n");
    dynarray_remove(&arr, 0);
    dynarray_remove(&arr, 0);
    print_array(&arr);
    printf("  (기대: [] size=0 capacity=8)\n");
    printf("remove 0 -> %d (기대: 0)\n", dynarray_remove(&arr, 0));
    printf("get 0    -> %d (기대: 0)\n", dynarray_get(&arr, 0, &value));
    dynarray_append(&arr, 70); // 다 지운 뒤에도 다시 쓸 수 있는지
    print_array(&arr);
    printf("  (기대: [70] size=1 capacity=8)\n");

    printf("\n== 많이 추가 ==\n");
    for (int i = 0; i < 100; i++) {
        dynarray_append(&arr, i);
    }
    int all_ok = 1;
    for (int i = 0; i < 100; i++) {
        if (!dynarray_get(&arr, (size_t)i + 1, &value) || value != i) {
            all_ok = 0;
        }
    }
    printf("size=%zu capacity=%zu (기대: 101, 128)\n", arr.size, arr.capacity);
    printf("확장 후 100개 모두 순서대로 조회 -> %d (기대: 1)\n", all_ok);

    dynarray_free(&arr);
    return 0;
}
