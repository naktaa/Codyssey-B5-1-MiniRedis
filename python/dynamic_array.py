"""보너스 1 DynamicArray.c 포팅: 칸이 모자라면 2배로 늘어나는 동적 배열.

Python list의 append 자동 확장에 기대지 않고, [None] * capacity 고정 길이 배열과 인덱스 접근만 쓴다.
C와 달리 원소 타입 제한이 없어 MinHeap이 HeapItem을 담는 저장소로 쓴다.
"""


class DynamicArray:
    """items는 고정 길이 배열, count는 저장된 원소 개수, capacity는 배열 칸 수.

    0 ~ count-1 칸만 유효하다. C의 size 필드는 MinHeap과 같은 이유(size() 이름과 겹침)로 count라 부른다.
    """

    def __init__(self, capacity):
        # capacity는 0보다 커야 한다(0이면 2배로 늘려도 0이다).
        self.items = [None] * capacity
        self.count = 0
        self.capacity = capacity

    def append(self, value):
        """맨 끝에 value를 추가한다. 가득 차 있으면 먼저 capacity를 2배로 늘린다. 평균 O(1)."""
        if self.count == self.capacity:
            # C의 realloc 대신 2배 크기 배열을 직접 만들고 같은 인덱스로 복사한다.
            tmp_items = [None] * (self.capacity * 2)
            for i in range(self.count):
                tmp_items[i] = self.items[i]
            self.items = tmp_items
            self.capacity *= 2

        self.items[self.count] = value  # 빈 칸 중 맨 앞(= 현재 count 위치)에 넣는다.
        self.count += 1

    def get(self, index):
        """index 칸의 값을 돌려준다. 범위 밖이면 None. O(1)."""
        # count 이상 칸은 아직 값을 넣지 않은 칸이다. 음수 인덱스도 C의 size_t처럼 범위 밖으로 본다.
        if index < 0 or index >= self.count:
            return None

        return self.items[index]

    def set(self, index, value):
        """이미 있는 index 칸의 값을 바꾼다. 반환: 바꿨으면 True, 범위 밖이면 False. O(1)."""
        # count 이상 칸에 쓰면 중간이 빈 배열이 되므로 막는다. 새 칸은 append로만 만든다.
        if index < 0 or index >= self.count:
            return False

        self.items[index] = value
        return True

    def remove(self, index):
        """index 칸을 지우고 뒤쪽 원소를 한 칸씩 앞으로 당긴다. 반환: 지웠으면 True, 범위 밖이면 False. O(n)."""
        if index < 0 or index >= self.count:
            return False

        # 앞에서부터 옮겨야 아직 옮기지 않은 값을 덮어쓰지 않는다.
        for i in range(index, self.count - 1):
            self.items[i] = self.items[i + 1]
        self.items[self.count - 1] = None  # C에는 없는 줄: 빈 칸이 옛 객체를 계속 잡고 있지 않게 비운다(GC).
        self.count -= 1

        return True
