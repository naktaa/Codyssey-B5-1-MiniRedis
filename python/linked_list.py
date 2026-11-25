"""C-1 LinkedList.c 포팅: 이중 연결 리스트.

C와 같이 Node는 data만 가리키고, 리스트는 HEAD/TAIL로 양 끝을 관리한다.
C의 free(Node)는 Python에서 참조를 끊는 것으로 대신한다(GC가 회수).
"""

from typing import Optional


class Node:
    """리스트의 한 칸. data는 실제 데이터(해시맵의 Data 등)를 가리킨다."""

    def __init__(self, data=None):
        self.data = data
        # 타입 표시: None으로 시작해도 편집기가 Node의 prev/next로 알아보게 한다(실행에는 영향 없음).
        self.prev: Optional["Node"] = None
        self.next: Optional["Node"] = None


class LinkedList:
    """HEAD/TAIL로 양 끝을 가리키는 이중 연결 리스트. 빈 리스트는 둘 다 None.

    모든 삽입/삭제/이동 연산은 O(1)이다.
    """

    def __init__(self):
        self.HEAD: Optional[Node] = None
        self.TAIL: Optional[Node] = None

    def insert_front(self, now: Node):
        """now 노드를 리스트 맨 앞에 붙인다."""
        if self.HEAD is None:
            self.HEAD = self.TAIL = now
            now.prev = now.next = None
        else:
            now.prev, now.next = None, self.HEAD
            self.HEAD.prev = now
            self.HEAD = now

    def insert_back(self, now: Node):
        """now 노드를 리스트 맨 뒤에 붙인다."""
        if self.TAIL is None:
            now.prev = now.next = None
            self.HEAD = self.TAIL = now
        else:
            now.prev, now.next = self.TAIL, None
            self.TAIL.next = now
            self.TAIL = now

    def remove_front(self):
        """맨 앞 노드를 떼어 낸다. 빈 리스트면 아무것도 하지 않는다."""
        if self.HEAD is None and self.TAIL is None:
            return
        else:
            if self.HEAD is self.TAIL:
                self.HEAD = self.TAIL = None
            else:
                self.HEAD = self.HEAD.next
                self.HEAD.prev = None

    def remove_back(self):
        """맨 뒤 노드를 떼어 낸다. 빈 리스트면 아무것도 하지 않는다."""
        if self.HEAD is None and self.TAIL is None:
            return
        else:
            if self.HEAD is self.TAIL:
                self.HEAD = self.TAIL = None
            else:
                self.TAIL = self.TAIL.prev
                self.TAIL.next = None

    def remove_node(self, now: Node):
        """now 노드를 리스트에서 떼어 낸다. 양 끝이면 remove_front/back을 쓴다."""
        if now is self.HEAD:
            self.remove_front()
        elif now is self.TAIL:
            self.remove_back()
        else:
            now.prev.next = now.next
            now.next.prev = now.prev

    def move_to_front(self, now: Node):
        """이미 리스트에 있는 now 노드를 맨 앞으로 옮긴다. 노드는 새로 만들지 않는다."""
        if self.HEAD is now:
            return
        else:
            now.prev.next = now.next
            if now is self.TAIL:
                self.TAIL = now.prev
            else:
                now.next.prev = now.prev

            self.HEAD.prev = now
            now.prev, now.next = None, self.HEAD
            self.HEAD = now
