"""C-4 Store.c 포팅: 해시맵(찾기·저장) + LRU 리스트(사용 순서) + 최소 힙(만료 순서).

해시맵은 LRU를 모르고, LRU·메모리·TTL 갱신은 모두 이 파일에서 한다.

  Data { key, value, lru_node ─┐ }
                               ▼
  lru:  HEAD → [최근] ⇄ ... ⇄ [오래됨] ← TAIL
                 └─data─▶ Data (같은 객체를 가리킴)

모든 명령은 now(현재 시각, ms)를 받는다. 테스트는 숫자를 직접 넣고, CLI는 현재 시각을 넣는다.
"""

from linked_list import Node, LinkedList
from hash_map import Data, HashMap
from min_heap import MinHeap

# set 반환값. CLI가 OOM 에러 문구를 따로 출력하려고 구분한다.
# Python에는 할당 실패가 없어 C의 STORE_ERR_ALLOC은 옮기지 않았다.
STORE_OK = 1
STORE_ERR_OOM = -1

# Data.expire_at이 이 값이면 만료 시간이 없다.
NO_EXPIRE = 0

INITIAL_CAPACITY = 16


class Store:
    """Mini Redis 저장소.

    used_memory: 모든 키의 entry_size 합. 노드·포인터·버킷 크기는 세지 않는다.
    maxmemory: 0이면 무제한.
    evicted_keys: LRU로 자동 제거된 키 개수(누적). 만료로 지운 키는 세지 않는다.
    heap: 오래된 항목이 남을 수 있다(lazy deletion). 꺼낼 때 해시맵과 비교해 버린다.
    """

    def __init__(self):
        self.map = HashMap(INITIAL_CAPACITY)
        self.lru: LinkedList[Data] = LinkedList()  # HEAD = 가장 최근, TAIL = 가장 오래됨
        self.heap = MinHeap(INITIAL_CAPACITY)
        self.used_memory = 0
        self.maxmemory = 0
        self.evicted_keys = 0

    def _entry_size(self, key, value):
        """키 하나가 차지하는 used_memory 크기 = len(utf8(key)) + len(utf8(value))."""
        # C는 value가 int라서 자릿수로 대신했다. 미션 공식대로 UTF-8 바이트 수를 센다(len(str)은 글자 수).
        return len(key.encode("utf-8")) + len(value.encode("utf-8"))

    def _delete_data(self, data):
        """이미 찾은 Data를 LRU·used_memory·해시맵에서 모두 제거한다. used_memory 감소는 여기서만 한다."""
        delete_memory = self._entry_size(data.key, data.value)

        self.used_memory -= delete_memory

        self.lru.remove_node(data.lru_node)
        self.map.remove(data.key)

    def _evict_if_needed(self):
        """SET 이후 maxmemory를 넘었으면 LRU의 가장 오래된 키(TAIL)부터 지운다."""
        while self.maxmemory > 0 and self.used_memory > self.maxmemory and self.lru.TAIL is not None:
            self._delete_data(self.lru.TAIL.data)
            self.evicted_keys += 1

    def _expire_if_needed(self, now):
        """now 시점에 만료된 키를 힙에서 꺼내 지운다. 모든 명령의 맨 앞에서 호출한다.

        힙 맨 위가 아직 안 지났으면 나머지도 안 지났으므로 거기서 멈춘다.
        """
        while True:
            item = self.heap.peek()
            if item is None:
                return
            if item.expire_at > now:
                return

            self.heap.pop()

            found = self.map.get_data(item.key)

            # 키가 있고 만료 시각도 같을 때만 진짜 만료. 아니면 오래된 기록이라 버리기만 한다.
            if found is not None and found.expire_at == item.expire_at:
                self._delete_data(found)

    def set(self, key, value, now):
        """SET key value. 저장·갱신 후 LRU 맨 앞으로 두고 TTL을 없앤다.

        반환: STORE_OK, 단일 엔트리가 maxmemory를 넘으면 STORE_ERR_OOM(저장하지 않음).
        """
        self._expire_if_needed(now)  # 만료된 키를 먼저 지워야 아래 '기존 키' 판단과 메모리 계산이 맞다

        new_memory = self._entry_size(key, value)

        # 단일 엔트리가 maxmemory 초과하면 OOM
        if self.maxmemory > 0 and new_memory > self.maxmemory:
            return STORE_ERR_OOM

        data = self.map.get_data(key)
        prev_memory = 0
        if data is not None:
            prev_memory = self._entry_size(data.key, data.value)

        self.map.put(key, value)

        if data is None:
            new_data = self.map.get_data(key)
            new_node = Node(new_data)
            new_data.lru_node = new_node
            self.lru.insert_front(new_data.lru_node)
        else:
            self.lru.move_to_front(data.lru_node)
            data.expire_at = NO_EXPIRE  # SET 덮어쓰기는 TTL을 없앤다. 힙의 예전 항목은 _expire_if_needed가 버린다

        self.used_memory -= prev_memory
        self.used_memory += new_memory
        self._evict_if_needed()
        return STORE_OK

    def get(self, key, now):
        """GET key. 찾으면 값을 돌려주고 LRU 맨 앞으로 옮긴다. 없으면 None."""
        self._expire_if_needed(now)  # 만료된 키는 여기서 지워져 '없는 키'가 되고 LRU도 갱신되지 않는다
        data = self.map.get_data(key)
        if data is None:
            return None

        self.lru.move_to_front(data.lru_node)

        return data.value

    def delete(self, key, now):
        """DEL key. 반환: 삭제 1, 없는 키 0. 힙의 항목은 그대로 둔다(lazy deletion)."""
        self._expire_if_needed(now)  # 만료된 키를 DEL하면 '없는 키'로 0이 나오게

        data = self.map.get_data(key)
        if data is None:
            return 0

        self._delete_data(data)

        return 1

    def expire(self, key, seconds, now):
        """EXPIRE key seconds. 반환: 설정(또는 seconds <= 0이면 즉시 삭제) 1, 없는 키 0."""
        self._expire_if_needed(now)

        data = self.map.get_data(key)
        if data is None:
            return 0

        if seconds <= 0:
            self._delete_data(data)
            return 1

        expire_at = now + seconds * 1000

        self.heap.push(expire_at, key)

        data.expire_at = expire_at
        return 1

    def ttl(self, key, now):
        """TTL key. 반환: 없는 키 -2, 만료 없음 -1, 그 외 남은 초(내림)."""
        self._expire_if_needed(now)

        data = self.map.get_data(key)
        if data is None:
            return -2

        if data.expire_at == NO_EXPIRE:
            return -1

        return (data.expire_at - now) // 1000

    # ---- 아래는 C Store에 없던 메서드: CLI 명령(EXISTS·DBSIZE·KEYS·INFO)이 만료 정리 후 읽도록 추가 ----

    def exists(self, key, now):
        """EXISTS key. 반환: 있으면 1, 없으면 0. LRU는 갱신하지 않는다."""
        self._expire_if_needed(now)
        return 1 if self.map.contains(key) else 0

    def dbsize(self, now):
        """DBSIZE. 반환: 저장된 키 개수."""
        self._expire_if_needed(now)
        return self.map.size()

    def keys(self, now):
        """KEYS. 반환: 저장된 모든 키의 배열(순서 무관)."""
        self._expire_if_needed(now)
        return self.map.keys()

    def info_memory(self, now):
        """INFO memory. 반환: (used_memory, maxmemory, evicted_keys)."""
        self._expire_if_needed(now)
        return self.used_memory, self.maxmemory, self.evicted_keys
