import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))

from linked_list import Node, LinkedList


def values(lst):
    """HEAD → TAIL 순서의 data 목록과 TAIL → HEAD 역순 목록을 함께 돌려준다(prev 연결 확인용)."""
    forward = []
    now = lst.HEAD
    while now:
        forward.append(now.data)
        now = now.next
    backward = []
    now = lst.TAIL
    while now:
        backward.append(now.data)
        now = now.prev
    return forward, backward


def make(*items):
    lst = LinkedList()
    nodes = [Node(x) for x in items]
    for n in nodes:
        lst.insert_back(n)
    return lst, nodes


class TestLinkedList(unittest.TestCase):
    def assertOrder(self, lst, expected):
        forward, backward = values(lst)
        self.assertEqual(forward, expected)
        self.assertEqual(backward, expected[::-1])

    def test_insert_front_and_back(self):
        lst = LinkedList()
        lst.insert_front(Node("b"))
        lst.insert_front(Node("a"))
        lst.insert_back(Node("c"))
        self.assertOrder(lst, ["a", "b", "c"])

    def test_remove_front_alone(self):
        lst, _ = make("a", "b", "c")
        lst.remove_front()
        self.assertOrder(lst, ["b", "c"])
        lst.remove_front()
        lst.remove_front()
        self.assertIsNone(lst.HEAD)
        self.assertIsNone(lst.TAIL)
        lst.remove_front()  # 빈 리스트: 아무 일도 없어야 한다
        self.assertIsNone(lst.HEAD)

    def test_remove_back_alone(self):
        lst, _ = make("a", "b", "c")
        lst.remove_back()
        self.assertOrder(lst, ["a", "b"])
        lst.remove_back()
        lst.remove_back()
        self.assertIsNone(lst.HEAD)
        self.assertIsNone(lst.TAIL)
        lst.remove_back()
        self.assertIsNone(lst.TAIL)

    def test_remove_node(self):
        lst, nodes = make("a", "b", "c", "d")
        lst.remove_node(nodes[1])  # 중간
        self.assertOrder(lst, ["a", "c", "d"])
        lst.remove_node(nodes[0])  # HEAD
        self.assertOrder(lst, ["c", "d"])
        lst.remove_node(nodes[3])  # TAIL
        self.assertOrder(lst, ["c"])
        lst.remove_node(nodes[2])  # 단일
        self.assertOrder(lst, [])

    def test_move_to_front(self):
        lst, nodes = make("a", "b", "c")
        lst.move_to_front(nodes[0])  # 이미 HEAD
        self.assertOrder(lst, ["a", "b", "c"])
        lst.move_to_front(nodes[1])  # 중간
        self.assertOrder(lst, ["b", "a", "c"])
        lst.move_to_front(nodes[2])  # TAIL
        self.assertOrder(lst, ["c", "b", "a"])
        self.assertIs(lst.TAIL, nodes[0])


if __name__ == "__main__":
    unittest.main()
