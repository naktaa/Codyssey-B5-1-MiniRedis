"""C-3 MinHeap.c 포팅: TTL 만료 관리용 최소 힙.

(expire_at, key)를 expire_at이 작은 순서로 꺼낸다. heapq는 쓰지 않는다.
배열 기반 완전 이진 트리이며 0번 인덱스부터 사용한다.
  부모 = (i - 1) // 2, 왼쪽 자식 = 2i + 1, 오른쪽 자식 = 2i + 2
보너스 1: 저장 공간(배열·개수·2배 확장)은 DynamicArray가 맡고, 힙은 순서 규칙만 지킨다.
"""

from dynamic_array import DynamicArray


class HeapItem:
    """힙의 한 항목. expire_at은 만료 시각(ms)."""

    def __init__(self, expire_at, key):
        self.expire_at = expire_at
        self.key = key


class MinHeap:
    """items는 HeapItem을 담는 DynamicArray. 항목 개수는 items.count, 배열 칸 수는 items.capacity."""

    def __init__(self, capacity):
        # capacity는 0보다 커야 한다(0이면 2배로 늘려도 0이다).
        self.items = DynamicArray(capacity)

    def _heapify_up(self, i):
        """i번 항목을 부모와 비교하며 위로 올린다. push에서 배열 끝에 넣은 직후 호출한다."""
        while i > 0:
            ex_time = self.items.get(i).expire_at
            if ex_time >= self.items.get((i - 1) // 2).expire_at:
                break

            swap_item = self.items.get(i)
            self.items.set(i, self.items.get((i - 1) // 2))
            self.items.set((i - 1) // 2, swap_item)
            i = (i - 1) // 2

    def _heapify_down(self, i):
        """i번 항목을 더 작은 자식과 비교하며 아래로 내린다. pop에서 마지막 항목을 0번으로 옮긴 뒤 호출한다."""
        while i * 2 + 1 < self.items.count:
            ex_time = self.items.get(i).expire_at
            min_time = self.items.get(i * 2 + 1).expire_at
            next_idx = i * 2 + 1

            if i * 2 + 2 < self.items.count:
                if self.items.get(i * 2 + 2).expire_at < min_time:
                    min_time = self.items.get(i * 2 + 2).expire_at
                    next_idx += 1
            if ex_time <= min_time:
                break

            swap_item = self.items.get(i)
            self.items.set(i, self.items.get(next_idx))
            self.items.set(next_idx, swap_item)
            i = next_idx

    def push(self, expire_at, key):
        """(expire_at, key)를 추가한다. 가득 차면 DynamicArray.append가 2배로 늘린다."""
        # 확장 코드는 DynamicArray로 옮겨 갔다. 힙은 끝에 넣고 제자리만 찾는다.
        self.items.append(HeapItem(expire_at, key))

        # append가 count를 먼저 올렸으므로 방금 넣은 항목은 count - 1번 칸에 있다.
        self._heapify_up(self.items.count - 1)

    def pop(self):
        """expire_at이 가장 작은 항목을 꺼내 돌려준다. 빈 힙이면 None."""
        if self.items.count == 0:
            return None

        out_item = self.items.get(0)
        last = self.items.count - 1
        self.items.set(0, self.items.get(last))
        # 끝 칸을 지우므로 당길 원소가 없어 O(1)이다. 빈 칸 비우기도 remove가 한다.
        self.items.remove(last)

        self._heapify_down(0)

        return out_item

    def peek(self):
        """expire_at이 가장 작은 항목을 꺼내지 않고 돌려준다. 빈 힙이면 None."""
        if self.items.count == 0:
            return None

        return self.items.get(0)

    def size(self):
        """반환: 현재 저장된 항목 개수."""
        return self.items.count
