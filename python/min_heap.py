"""C-3 MinHeap.c 포팅: TTL 만료 관리용 최소 힙.

(expire_at, key)를 expire_at이 작은 순서로 꺼낸다. heapq는 쓰지 않는다.
배열 기반 완전 이진 트리이며 0번 인덱스부터 사용한다.
  부모 = (i - 1) // 2, 왼쪽 자식 = 2i + 1, 오른쪽 자식 = 2i + 2
"""


class HeapItem:
    """힙의 한 항목. expire_at은 만료 시각(ms)."""

    def __init__(self, expire_at, key):
        self.expire_at = expire_at
        self.key = key


class MinHeap:
    """items는 고정 길이 배열, count는 저장된 항목 개수, capacity는 배열 칸 수."""

    def __init__(self, capacity):
        # capacity는 0보다 커야 한다(0이면 2배로 늘려도 0이다).
        self.items = [None] * capacity
        self.count = 0
        self.capacity = capacity

    def _heapify_up(self, i):
        """i번 항목을 부모와 비교하며 위로 올린다. push에서 배열 끝에 넣은 직후 호출한다."""
        while i > 0:
            ex_time = self.items[i].expire_at
            if ex_time >= self.items[(i - 1) // 2].expire_at:
                break

            swap_item = self.items[i]
            self.items[i] = self.items[(i - 1) // 2]
            self.items[(i - 1) // 2] = swap_item
            i = (i - 1) // 2

    def _heapify_down(self, i):
        """i번 항목을 더 작은 자식과 비교하며 아래로 내린다. pop에서 마지막 항목을 0번으로 옮긴 뒤 호출한다."""
        while i * 2 + 1 < self.count:
            ex_time = self.items[i].expire_at
            min_time = self.items[i * 2 + 1].expire_at
            next_idx = i * 2 + 1

            if i * 2 + 2 < self.count:
                if self.items[i * 2 + 2].expire_at < min_time:
                    min_time = self.items[i * 2 + 2].expire_at
                    next_idx += 1
            if ex_time <= min_time:
                break

            swap_item = self.items[i]
            self.items[i] = self.items[next_idx]
            self.items[next_idx] = swap_item
            i = next_idx

    def push(self, expire_at, key):
        """(expire_at, key)를 추가한다. 가득 차면 배열을 2배로 새로 만들어 옮긴다."""
        if self.capacity == self.count:
            # C의 realloc 대신 2배 크기 배열을 직접 만들고 복사한다(append 자동 확장에 기대지 않음).
            tmp_items = [None] * (self.capacity * 2)
            for i in range(self.count):
                tmp_items[i] = self.items[i]
            self.items = tmp_items
            self.capacity *= 2

        self.items[self.count] = HeapItem(expire_at, key)

        self._heapify_up(self.count)
        self.count += 1

    def pop(self):
        """expire_at이 가장 작은 항목을 꺼내 돌려준다. 빈 힙이면 None."""
        if self.count == 0:
            return None

        out_item = self.items[0]
        self.items[0] = self.items[self.count - 1]
        self.items[self.count - 1] = None  # 꺼낸 칸이 옛 항목을 계속 잡고 있지 않게 비운다.
        self.count -= 1

        self._heapify_down(0)

        return out_item

    def peek(self):
        """expire_at이 가장 작은 항목을 꺼내지 않고 돌려준다. 빈 힙이면 None."""
        if self.count == 0:
            return None

        return self.items[0]

    def size(self):
        """반환: 현재 저장된 항목 개수."""
        return self.count
