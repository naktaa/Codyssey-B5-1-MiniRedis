import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))

from store import Store, STORE_OK, STORE_ERR_OOM, INITIAL_CAPACITY


def lru(store):
    """LRU 순서를 HEAD(최근)부터 키 목록으로 돌려준다."""
    result = []
    now = store.lru.HEAD
    while now:
        result.append(now.data.key)
        now = now.next
    return result


class TestStoreLRU(unittest.TestCase):
    """C store_test.c의 set/get/del/확장 구간. TTL과 무관하므로 시각은 0으로 고정한다."""

    def test_lru_order(self):
        s = Store()
        s.set("a", "1", 0)
        s.set("b", "2", 0)
        s.set("c", "3", 0)
        self.assertEqual(lru(s), ["c", "b", "a"])

        self.assertEqual(s.get("a", 0), "1")
        self.assertEqual(lru(s), ["a", "c", "b"])
        s.get("a", 0)  # 이미 맨 앞
        self.assertEqual(lru(s), ["a", "c", "b"])
        self.assertIsNone(s.get("none", 0))

        s.set("b", "20", 0)  # 기존 키
        self.assertEqual(s.get("b", 0), "20")
        self.assertEqual(lru(s), ["b", "a", "c"])
        self.assertEqual(s.map.size(), 3)

        self.assertEqual(s.delete("c", 0), 1)  # TAIL 삭제
        self.assertEqual(lru(s), ["b", "a"])
        self.assertEqual(s.delete("b", 0), 1)  # HEAD 삭제
        self.assertEqual(lru(s), ["a"])
        self.assertEqual(s.delete("b", 0), 0)
        self.assertEqual(s.map.size(), 1)

    def test_lru_after_resize(self):
        s = Store()
        s.set("a", "1", 0)
        for i in range(20):
            s.set("k%d" % i, str(i), 0)
        s.get("a", 0)
        self.assertGreater(s.map.capacity, INITIAL_CAPACITY)
        self.assertEqual(lru(s), ["a"] + ["k%d" % i for i in range(19, -1, -1)])
        self.assertEqual(s.map.size(), 21)
        # a=1(2) + k0~k9(각 3) + k10~k19(각 5) = 82
        self.assertEqual((s.used_memory, s.evicted_keys), (82, 0))


class TestStoreMemory(unittest.TestCase):
    """C store_test.c의 메모리 제한 구간(maxmemory 10)."""

    def test_memory_limit(self):
        s = Store()
        s.maxmemory = 10

        s.set("a", "111", 0)  # 4
        s.set("b", "222", 0)  # 8
        s.get("a", 0)         # a가 최근
        s.set("c", "333", 0)  # 12 > 10 → TAIL(b) 제거
        self.assertEqual(lru(s), ["c", "a"])
        self.assertEqual((s.used_memory, s.evicted_keys), (8, 1))
        self.assertIsNone(s.get("b", 0))

        # 기존 키 갱신 크기
        s.set("c", "3", 0)  # 4 → 2
        self.assertEqual(s.used_memory, 6)
        s.set("a", "11111", 0)  # 4 → 6
        self.assertEqual(s.used_memory, 8)
        self.assertEqual(lru(s), ["a", "c"])

        # OOM
        self.assertEqual(s.set("big", "1234567890", 0), STORE_ERR_OOM)  # 3 + 10
        self.assertFalse(s.map.contains("big"))
        self.assertEqual(s.set("a", "1234567890", 0), STORE_ERR_OOM)  # 1 + 10, 덮어쓰기
        self.assertEqual(s.get("a", 0), "11111")  # 기존 값 유지
        self.assertEqual((s.used_memory, s.evicted_keys), (8, 1))

        # 경계: 딱 maxmemory
        self.assertEqual(s.set("x", "123456789", 0), STORE_OK)  # 1 + 9 = 10
        self.assertEqual(lru(s), ["x"])  # 18 → c 제거 16 → a 제거 10
        self.assertEqual((s.used_memory, s.evicted_keys), (10, 3))

        s.delete("x", 0)
        self.assertEqual(s.used_memory, 0)
        self.assertEqual(lru(s), [])

    def test_utf8_bytes(self):
        # 미션 공식: len(utf8(key)) + len(utf8(value)). 한글은 글자당 3바이트.
        s = Store()
        s.set("이름", "철수", 0)
        self.assertEqual(s.used_memory, 12)

    def test_mission_example(self):
        # 미션 결과 예시: maxmemory 30에서 user:3 저장 후 user:1 제거, used_memory 22.
        s = Store()
        s.maxmemory = 30
        s.set("user:1", "Alice", 0)
        s.set("user:2", "Bob", 0)
        s.set("user:3", "Charlie", 0)
        self.assertIsNone(s.get("user:1", 0))
        self.assertEqual(s.info_memory(0), (22, 30, 1))


class TestStoreTTL(unittest.TestCase):
    """C store_test.c의 TTL 구간. now(ms)를 직접 넣어 시간 흐름을 흉내 낸다."""

    def test_ttl(self):
        s = Store()

        s.set("a", "1", 0)
        self.assertEqual(s.ttl("a", 0), -1)     # 만료 없음
        self.assertEqual(s.ttl("none", 0), -2)  # 없는 키
        self.assertEqual(s.expire("none", 10, 0), 0)

        self.assertEqual(s.expire("a", 3, 0), 1)  # 만료 시각 3000
        self.assertEqual(s.ttl("a", 0), 3)
        self.assertEqual(s.ttl("a", 1), 2)  # 내림
        self.assertEqual(s.ttl("a", 2999), 0)
        self.assertIsNone(s.get("a", 3000))  # 만료
        # 만료는 evicted에 안 셈
        self.assertEqual((s.map.size(), s.used_memory, s.evicted_keys), (0, 0, 0))

        # EXPIRE 재설정 (오래된 힙 항목)
        s.set("b", "2", 3000)
        s.expire("b", 10, 3000)  # 13000
        s.expire("b", 20, 4000)  # 24000. 힙에 (13000, b)가 남는다
        self.assertEqual(s.get("b", 14000), "2")  # 13000 항목은 버려짐
        self.assertEqual(s.ttl("b", 14000), 10)
        self.assertIsNone(s.get("b", 24000))

        # DEL 후 같은 이름 다시 SET
        s.set("c", "1", 30000)
        s.expire("c", 5, 30000)  # 35000
        s.delete("c", 30000)
        s.set("c", "2", 30000)  # 새 c. 만료 없음
        self.assertEqual(s.get("c", 35000), "2")
        self.assertEqual(s.ttl("c", 35000), -1)

        # SET 덮어쓰기는 TTL을 없앤다
        s.set("d", "1", 40000)
        s.expire("d", 5, 40000)  # 45000
        s.set("d", "2", 40000)
        self.assertEqual(s.ttl("d", 40000), -1)
        self.assertEqual(s.get("d", 45000), "2")

        # EXPIRE 0 이하는 즉시 삭제
        s.set("e", "1", 50000)
        self.assertEqual(s.expire("e", 0, 50000), 1)
        self.assertIsNone(s.get("e", 50000))

        # 만료된 키의 GET은 LRU를 바꾸지 않는다
        s.set("f", "1", 60000)
        s.set("g", "1", 60000)
        s.expire("f", 1, 60000)  # 61000
        self.assertEqual(lru(s), ["g", "f", "d", "c"])
        self.assertIsNone(s.get("f", 61000))
        self.assertEqual(lru(s), ["g", "d", "c"])
        # c=2 + d=2 + g=2
        self.assertEqual((s.used_memory, s.evicted_keys), (6, 0))

    def test_expired_key_hidden_from_other_commands(self):
        s = Store()
        s.set("a", "1", 0)
        s.set("b", "2", 0)
        s.expire("a", 1, 0)
        self.assertEqual(s.exists("a", 1000), 0)
        self.assertEqual(s.dbsize(1000), 1)
        self.assertEqual(s.keys(1000), ["b"])
        self.assertEqual(s.delete("a", 1000), 0)
        self.assertEqual(s.info_memory(1000), (2, 0, 0))


if __name__ == "__main__":
    unittest.main()
