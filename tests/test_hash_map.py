import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))

from hash_map import HashMap

CAP = 100


class TestHashMap(unittest.TestCase):
    """C hashmap_test.c 시나리오와 기대값을 옮겼다."""

    def setUp(self):
        self.map = HashMap(CAP)

    def test_hash_matches_c(self):
        # C hashmap_hash와 같은 값(바이트 누적, 64비트 오버플로).
        h = self.map._hash
        self.assertEqual(h(""), 0)
        self.assertEqual(h("a"), 97)
        self.assertEqual(h("ab"), 97 * 31 + 98)
        self.assertEqual(h("가"), (234 * 31 + 176) * 31 + 128)  # UTF-8: EA B0 80
        self.assertEqual(h("x" * 40), 11907139948119108608)  # C 실행 결과와 비교한 값
        self.assertEqual(h("가나다"), 204539175982653)

    def test_put_get(self):
        self.map.put("user:1", "10")
        self.map.put("user:2", "20")
        self.map.put("user:1", "11")  # 기존 키 갱신
        self.assertEqual(self.map.get("user:1"), "11")
        self.assertIsNone(self.map.get("none"))
        self.assertEqual(self.map.size(), 2)

    def test_contains(self):
        self.map.put("user:2", "20")
        self.assertTrue(self.map.contains("user:2"))
        self.assertFalse(self.map.contains("none"))

    def test_keys(self):
        self.assertEqual(self.map.keys(), [])
        self.map.put("user:1", "10")
        self.map.put("user:2", "20")
        self.assertEqual(sorted(self.map.keys()), ["user:1", "user:2"])

    def test_remove(self):
        self.map.put("user:1", "11")
        self.map.put("user:2", "20")
        self.assertTrue(self.map.remove("user:1"))
        self.assertFalse(self.map.remove("user:1"))
        self.assertEqual(self.map.size(), 1)
        self.map.remove("user:2")
        self.assertEqual(self.map.keys(), [])

    def test_collision_same_bucket(self):
        # 97, 101, 105 → 4로 나눈 나머지가 모두 1: 같은 체인 안에서 키 내용으로 구별해야 한다.
        small = HashMap(4)
        for k in ("a", "e", "i"):
            small.put(k, k.upper())
        self.assertEqual(small.capacity, 4)  # 3 * 4 > 4 * 3 이 아니므로 확장 없음
        self.assertEqual([small.get(k) for k in ("a", "e", "i")], ["A", "E", "I"])
        self.assertTrue(small.remove("e"))
        self.assertEqual([small.get(k) for k in ("a", "e", "i")], ["A", None, "I"])

    def test_resize(self):
        for i in range(200):
            self.map.put("key:%d" % i, str(i))
        all_ok = all(self.map.get("key:%d" % i) == str(i) for i in range(200))
        self.assertEqual(self.map.size(), 200)
        self.assertEqual(self.map.capacity, 400)
        self.assertTrue(all_ok)

    def test_get_data(self):
        self.map.put("k", "v")
        data = self.map.get_data("k")
        self.assertEqual((data.key, data.value, data.lru_node, data.expire_at), ("k", "v", None, 0))
        self.assertIsNone(self.map.get_data("none"))


if __name__ == "__main__":
    unittest.main()
