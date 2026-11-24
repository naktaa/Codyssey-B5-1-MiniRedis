import os
import random
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))

from min_heap import MinHeap


class TestMinHeap(unittest.TestCase):
    def test_empty(self):
        heap = MinHeap(4)
        self.assertEqual(heap.size(), 0)
        self.assertIsNone(heap.peek())
        self.assertIsNone(heap.pop())

    def test_peek_does_not_remove(self):
        heap = MinHeap(4)
        heap.push(300, "c")
        heap.push(100, "a")
        heap.push(200, "b")
        item = heap.peek()
        self.assertEqual((item.expire_at, item.key), (100, "a"))
        self.assertEqual(heap.size(), 3)

    def test_single(self):
        heap = MinHeap(1)
        heap.push(5, "x")
        item = heap.pop()
        self.assertEqual((item.expire_at, item.key), (5, "x"))
        self.assertEqual(heap.size(), 0)
        self.assertIsNone(heap.pop())

    def test_grow(self):
        heap = MinHeap(1)
        for t in (3, 1, 2, 5, 4):
            heap.push(t, "k%d" % t)
        self.assertEqual(heap.items.capacity, 8)  # 1 → 2 → 4 → 8 (확장은 DynamicArray가 맡음)
        self.assertEqual(heap.size(), 5)
        self.assertEqual([heap.pop().key for _ in range(5)], ["k1", "k2", "k3", "k4", "k5"])

    def test_random_push_then_pop_all_sorted(self):
        rng = random.Random(0)
        heap = MinHeap(2)
        times = [rng.randint(0, 1000) for _ in range(200)]  # 중복 값 포함
        for i, t in enumerate(times):
            heap.push(t, "key:%d" % i)
        popped = [heap.pop().expire_at for _ in range(200)]
        self.assertEqual(popped, sorted(times))
        self.assertEqual(heap.size(), 0)


if __name__ == "__main__":
    unittest.main()
