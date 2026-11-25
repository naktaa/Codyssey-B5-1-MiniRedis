import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))

import binary_tree


def values(arr):
    """순회 결과 DynamicArray의 유효한 칸만 배열로 돌려준다."""
    return [arr.get(i) for i in range(arr.count)]


class TestBinaryTree(unittest.TestCase):
    """C tree_test.c 시나리오와 기대값을 옮겼다."""

    def assert_orders(self, root, pre, ino, post, level):
        self.assertEqual(values(binary_tree.preorder(root)), pre)
        self.assertEqual(values(binary_tree.inorder(root)), ino)
        self.assertEqual(values(binary_tree.postorder(root)), post)
        self.assertEqual(values(binary_tree.levelorder(root)), level)

    def test_full_tree_7(self):
        root = binary_tree.build([1, 2, 3, 4, 5, 6, 7])
        self.assert_orders(root,
                           [1, 2, 4, 5, 3, 6, 7],
                           [4, 2, 5, 1, 6, 3, 7],
                           [4, 5, 2, 6, 7, 3, 1],
                           [1, 2, 3, 4, 5, 6, 7])  # 레벨 순회 = 원래 배열 순서

    def test_last_level_right_empty_6(self):
        root = binary_tree.build([1, 2, 3, 4, 5, 6])
        self.assert_orders(root,
                           [1, 2, 4, 5, 3, 6],
                           [4, 2, 5, 1, 6, 3],
                           [4, 5, 2, 6, 3, 1],
                           [1, 2, 3, 4, 5, 6])
        self.assertIsNone(root.right.right)  # 3의 오른쪽 자식

    def test_single_node(self):
        root = binary_tree.build([42])
        self.assert_orders(root, [42], [42], [42], [42])

    def test_empty_tree(self):
        root = binary_tree.build([])
        self.assertIsNone(root)
        self.assert_orders(root, [], [], [], [])


if __name__ == "__main__":
    unittest.main()
