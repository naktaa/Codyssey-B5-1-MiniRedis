"""보너스 3 BinaryTree.c 포팅: 이진 트리와 순회 4종.

build는 배열을 힙과 같은 인덱스 규칙으로 노드 트리로 바꾼다.
  arr[i]의 왼쪽 자식 = arr[2i + 1], 오른쪽 자식 = arr[2i + 2]
C는 순회하며 값을 출력했지만, Python은 방문한 값을 DynamicArray에 담아 돌려준다(테스트에서 비교하기 위해).
C의 tree_free는 GC가 대신하므로 없다.
"""

from dynamic_array import DynamicArray
from linked_list import LinkedList, Node


class TreeNode:
    """트리의 한 노드. left/right는 자식 노드, 없으면 None."""

    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None


def _build(arr, i):
    """i번 칸을 루트로 하는 서브트리를 만든다. 범위 밖이면 None."""
    if i >= len(arr):
        return None

    new_node = TreeNode(arr[i])
    new_node.left = _build(arr, i * 2 + 1)
    new_node.right = _build(arr, i * 2 + 2)
    # C의 할당 실패 처리는 Python에 없어서 뺐다.
    return new_node


def build(arr):
    """배열을 완전 이진 트리로 만든다. 반환: 루트, 빈 배열이면 None."""
    return _build(arr, 0)


def _preorder(root, out):
    if root is None:
        return
    out.append(root.value)
    _preorder(root.left, out)
    _preorder(root.right, out)


def _inorder(root, out):
    if root is None:
        return
    _inorder(root.left, out)
    out.append(root.value)
    _inorder(root.right, out)


def _postorder(root, out):
    if root is None:
        return
    _postorder(root.left, out)
    _postorder(root.right, out)
    out.append(root.value)


def preorder(root):
    """전위: 루트 → 왼쪽 → 오른쪽. 반환: 방문한 값의 DynamicArray."""
    out = DynamicArray(4)
    _preorder(root, out)
    return out


def inorder(root):
    """중위: 왼쪽 → 루트 → 오른쪽. BST에서는 정렬 순서가 된다."""
    out = DynamicArray(4)
    _inorder(root, out)
    return out


def postorder(root):
    """후위: 왼쪽 → 오른쪽 → 루트. 자식을 다 본 뒤 루트를 방문한다."""
    out = DynamicArray(4)
    _postorder(root, out)
    return out


def levelorder(root):
    """레벨: 위층부터 왼쪽 → 오른쪽. 큐는 직접 만든 LinkedList(insert_back + remove_front)."""
    out = DynamicArray(4)
    if root is None:
        return out

    queue = LinkedList()
    queue.insert_back(Node(root))

    while queue.HEAD:
        # remove_front는 data를 돌려주지 않으므로 트리 노드를 먼저 꺼낸다(C와 같은 이유).
        now = queue.HEAD.data
        queue.remove_front()
        out.append(now.value)

        if now.left:
            queue.insert_back(Node(now.left))
        if now.right:
            queue.insert_back(Node(now.right))

    return out
