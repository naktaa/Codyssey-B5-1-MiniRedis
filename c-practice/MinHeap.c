#include <stdlib.h>
#include <string.h>

#include "MinHeap.h"

/*
 * 내부 전용 함수. 공개 함수 원형과 역할 설명은 MinHeap.h에 있다.
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
 * 공개 함수 본문(init, free, push, pop, peek, size)은 여기 아래에 직접 작성한다.
 * 구현 순서는 MinHeap.h 맨 위 주석을 참고한다.
 */
