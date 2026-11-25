import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))

import bst
import binary_tree


def values(arr):
    """순회 결과 DynamicArray의 유효한 칸만 배열로 돌려준다."""
    return [arr.get(i) for i in range(arr.count)]


class TestBST(unittest.TestCase):
    """C bst_test.c 시나리오와 기대값을 옮겼다. 전위는 모양, 중위는 정렬을 확인한다."""

    def setUp(self):
        self.root = None
        for v in (8, 3, 10, 1, 6, 14, 4, 7, 13):
            self.root = bst.insert(self.root, v)

    def assert_tree(self, pre, ino):
        self.assertEqual(values(binary_tree.preorder(self.root)), pre)
        self.assertEqual(values(binary_tree.inorder(self.root)), ino)

    def test_insert(self):
        self.assert_tree([8, 3, 1, 6, 4, 7, 10, 14, 13], [1, 3, 4, 6, 7, 8, 10, 13, 14])

    def test_insert_duplicate_ignored(self):
        self.root = bst.insert(self.root, 6)
        self.assert_tree([8, 3, 1, 6, 4, 7, 10, 14, 13], [1, 3, 4, 6, 7, 8, 10, 13, 14])

    def test_search(self):
        found = bst.search(self.root, 6)
        self.assertEqual(found.value, 6)
        self.assertEqual((found.left.value, found.right.value), (4, 7))
        self.assertIsNone(bst.search(self.root, 5))
        self.assertIsNone(bst.search(None, 5))  # 빈 트리

    def test_remove_cases_in_order(self):
        # C 시나리오처럼 이전 삭제 결과 위에서 이어서 지운다.
        self.root = bst.remove(self.root, 13)  # 자식 0개
        self.assert_tree([8, 3, 1, 6, 4, 7, 10, 14], [1, 3, 4, 6, 7, 8, 10, 14])

        self.root = bst.remove(self.root, 10)  # 자식 1개: 14가 올라옴
        self.assert_tree([8, 3, 1, 6, 4, 7, 14], [1, 3, 4, 6, 7, 8, 14])

        self.root = bst.remove(self.root, 3)   # 자식 2개: 후계자 4
        self.assert_tree([8, 4, 1, 6, 7, 14], [1, 4, 6, 7, 8, 14])

        self.root = bst.remove(self.root, 8)   # 루트: 후계자 14가 루트로
        self.assert_tree([14, 4, 1, 6, 7], [1, 4, 6, 7, 14])
        self.assertEqual(self.root.value, 14)

        self.root = bst.remove(self.root, 99)  # 없는 값
        self.assert_tree([14, 4, 1, 6, 7], [1, 4, 6, 7, 14])

        for v in (1, 4, 6, 7, 14):             # 전부 삭제 후 다시 삽입
            self.root = bst.remove(self.root, v)
        self.assertIsNone(self.root)
        self.root = bst.insert(self.root, 5)
        self.assert_tree([5], [5])

    def test_sorted_insert_skews_right(self):
        self.root = None
        for v in range(1, 6):
            self.root = bst.insert(self.root, v)
        self.assert_tree([1, 2, 3, 4, 5], [1, 2, 3, 4, 5])

        chain = []
        now = self.root
        while now:
            self.assertIsNone(now.left)  # 왼쪽 자식 없이 오른쪽으로만 이어짐
            chain.append(now.value)
            now = now.right
        self.assertEqual(chain, [1, 2, 3, 4, 5])


if __name__ == "__main__":
    unittest.main()
