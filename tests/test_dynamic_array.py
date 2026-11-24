import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))

from dynamic_array import DynamicArray


def values(arr):
    """유효한 칸(0 ~ count-1)만 배열로 돌려준다."""
    return [arr.get(i) for i in range(arr.count)]


class TestDynamicArray(unittest.TestCase):
    """C dynarray_test.c 시나리오와 기대값을 옮겼다."""

    def setUp(self):
        self.arr = DynamicArray(2)
        for v in (10, 20, 30, 40, 50):
            self.arr.append(v)

    def test_append_grows_double(self):
        arr = DynamicArray(2)
        arr.append(10)
        arr.append(20)
        self.assertEqual((values(arr), arr.count, arr.capacity), ([10, 20], 2, 2))
        arr.append(30)  # 가득 찬 상태에서 추가 → 2 → 4
        self.assertEqual((values(arr), arr.capacity), ([10, 20, 30], 4))
        self.assertEqual(self.arr.capacity, 8)  # 5개: 2 → 4 → 8
        self.assertEqual(values(self.arr), [10, 20, 30, 40, 50])

    def test_get(self):
        self.assertEqual(self.arr.get(0), 10)
        self.assertEqual(self.arr.get(4), 50)   # 마지막 칸
        self.assertIsNone(self.arr.get(5))      # index == count
        self.assertIsNone(self.arr.get(7))      # capacity 안이지만 값이 없는 칸
        self.assertIsNone(self.arr.get(-1))     # 음수는 범위 밖

    def test_set(self):
        self.assertTrue(self.arr.set(1, 21))
        self.assertFalse(self.arr.set(5, 60))   # 새 칸은 append로만
        self.assertEqual(values(self.arr), [10, 21, 30, 40, 50])

    def test_remove_keeps_order(self):
        self.assertTrue(self.arr.remove(2))     # 중간
        self.assertEqual(values(self.arr), [10, 20, 40, 50])
        self.assertTrue(self.arr.remove(0))     # 맨 앞
        self.assertEqual(values(self.arr), [20, 40, 50])
        self.assertTrue(self.arr.remove(2))     # 맨 끝
        self.assertEqual(values(self.arr), [20, 40])
        self.assertFalse(self.arr.remove(2))    # index == count
        self.assertEqual(self.arr.capacity, 8)  # 줄어도 capacity는 그대로
        self.assertIsNone(self.arr.items[2])    # 비운 칸은 옛 값을 잡고 있지 않음

    def test_empty_then_reuse(self):
        for _ in range(5):
            self.arr.remove(0)
        self.assertEqual((self.arr.count, self.arr.capacity), (0, 8))
        self.assertFalse(self.arr.remove(0))
        self.assertIsNone(self.arr.get(0))
        self.arr.append(70)  # 다 지운 뒤에도 다시 쓸 수 있는지
        self.assertEqual(values(self.arr), [70])

    def test_many_append(self):
        arr = DynamicArray(1)
        for i in range(100):
            arr.append(i)
        self.assertEqual((arr.count, arr.capacity), (100, 128))
        self.assertEqual(values(arr), list(range(100)))


if __name__ == "__main__":
    unittest.main()
