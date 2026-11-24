"""C-2 HashMap.c 포팅: 체이닝 해시맵.

버킷 배열의 각 칸이 LinkedList이고, 그 Node의 data가 Data를 가리킨다.
dict·set·내장 hash()는 쓰지 않는다. 버킷 배열은 고정 길이 list로만 쓰고
확장도 새 배열을 만들어 직접 옮긴다.
"""

from linked_list import Node, LinkedList

# C의 size_t(macOS 64비트)와 같은 오버플로 결과를 내기 위한 마스크.
SIZE_MASK = (1 << 64) - 1


class Data:
    """키 하나의 실제 데이터. lru_node·expire_at은 Store가 관리하며 해시맵은 읽지 않는다."""

    def __init__(self, key, value):
        self.key = key
        self.value = value
        self.lru_node = None  # Store의 LRU 리스트에서 이 키를 담은 노드
        self.expire_at = 0    # 만료 시각(ms). 0이면 만료 없음(store.NO_EXPIRE)


class HashMap:
    """체이닝 해시맵. capacity는 버킷 개수, count는 저장된 서로 다른 키 개수."""

    def __init__(self, capacity):
        # 각 칸을 별도의 LinkedList 객체로 만든다([LinkedList()] * n은 한 객체를 공유하게 된다).
        self.buckets = [LinkedList() for _ in range(capacity)]
        self.capacity = capacity
        self.count = 0

    def _hash(self, key):
        """key의 UTF-8 바이트를 31배씩 누적한 해시값. C의 unsigned char 누적과 같은 값을 낸다."""
        total = 0

        for ch in key.encode("utf-8"):
            total = (total * 31 + ch) & SIZE_MASK

        return total

    def _find_node(self, key):
        """key가 들어 있는 노드와 key가 속한 버킷 리스트를 돌려준다. 못 찾으면 노드는 None."""
        idx = self._hash(key) % self.capacity
        lst = self.buckets[idx]

        # 못 찾아도 put이 이 리스트에 넣어야 하므로 리스트는 항상 돌려준다.
        now = lst.HEAD
        while now:
            data = now.data
            if data.key == key:
                return now, lst
            now = now.next
        return None, lst

    def put(self, key, value):
        """키가 있으면 값을 갱신하고, 없으면 새 항목을 저장한다. 로드 팩터 0.75 초과 시 2배 확장.

        기존 키 갱신 시 count는 늘지 않는다.
        """
        found, lst = self._find_node(key)

        # 이미 있는 키: 값만 바꾸고 count는 그대로 둔다.
        if found:
            data = found.data
            data.value = value
            return

        NewData = Data(key, value)
        NewNode = Node(NewData)
        lst.insert_back(NewNode)
        self.count += 1

        # 버킷 2배 확장
        if self.count * 4 > self.capacity * 3:
            new_capacity = self.capacity * 2
            new_buckets = [LinkedList() for _ in range(new_capacity)]

            for i in range(self.capacity):
                prev_list = self.buckets[i]
                now = prev_list.HEAD

                while now:
                    data = now.data
                    idx = self._hash(data.key) % new_capacity
                    nextList = new_buckets[idx]
                    # insert_back이 now.next를 바꾸므로 다음 노드를 먼저 기억한다.
                    now_temp = now.next
                    nextList.insert_back(now)
                    now = now_temp

            self.buckets = new_buckets
            self.capacity = new_capacity

    def get(self, key):
        """key의 값을 돌려준다. 없으면 None."""
        found, _ = self._find_node(key)
        if found is None:
            return None

        data = found.data
        return data.value

    def remove(self, key):
        """key를 삭제한다. 반환: 삭제했으면 True, 해당 키가 없으면 False."""
        found, lst = self._find_node(key)
        if found is None:
            return False

        lst.remove_node(found)
        self.count -= 1

        return True

    def contains(self, key):
        """반환: 키가 있으면 True, 없으면 False."""
        # get을 부르지 않는다: get에만 LRU 갱신이 붙기 때문이다.
        found, _ = self._find_node(key)
        return found is not None

    def keys(self):
        """저장된 모든 키를 count 길이의 배열로 돌려준다. 순서는 무관하다."""
        # C는 여기서 바로 출력했다. Python은 CLI가 Redis 형식으로 출력하도록 배열로 돌려준다.
        result = [None] * self.count
        idx = 0
        for i in range(self.capacity):
            lst = self.buckets[i]

            now = lst.HEAD
            while now:
                data = now.data
                result[idx] = data.key
                idx += 1
                now = now.next
        return result

    def size(self):
        """반환: 현재 저장된 서로 다른 키의 개수."""
        return self.count

    def get_data(self, key):
        """key의 Data를 돌려준다. 없으면 None. Store가 lru_node·expire_at에 접근할 때 쓴다."""
        found, _ = self._find_node(key)

        if found is None:
            return None

        return found.data
