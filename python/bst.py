"""보너스 4 BST.c 포팅: 이진 탐색 트리.

모든 노드에서 왼쪽 서브트리 값 < 노드 값 < 오른쪽 서브트리 값.
노드는 binary_tree.TreeNode를 그대로 쓰고, 정렬 결과는 binary_tree.inorder로 본다.
insert/remove는 바뀐 뒤의 루트를 돌려주므로 root = insert(root, v)처럼 받는다.
"""

from binary_tree import TreeNode


def insert(root, value):
    """규칙대로 내려가 빈 자리에 새 노드를 붙인다. 같은 값이 있으면 아무것도 바꾸지 않는다."""
    if root is None:
        return TreeNode(value)

    if value < root.value:
        root.left = insert(root.left, value)
    elif value > root.value:
        root.right = insert(root.right, value)
    # 같은 값: 중복을 두지 않는다.

    return root


def search(root, value):
    """작으면 왼쪽, 크면 오른쪽으로 한 갈래만 내려간다. 반환: 값이 있는 노드, 없으면 None."""
    now = root
    while now:
        if value == now.value:
            return now
        elif value < now.value:
            now = now.left
        else:
            now = now.right

    return None


def remove(root, value):
    """value를 지우고 이 서브트리의 새 루트를 돌려준다. 지울 노드의 자식 수에 따라 세 경우로 나눈다."""
    if root is None:
        return None  # 없는 값

    if value < root.value:
        root.left = remove(root.left, value)
    elif value > root.value:
        root.right = remove(root.right, value)
    else:
        # 자식 0개: 떼어 낸다. C의 free는 GC가 대신한다.
        if root.left is None and root.right is None:
            return None
        # 자식 1개: 그 자식이 지운 자리로 올라간다.
        elif root.left is None or root.right is None:
            return root.left if root.left else root.right
        # 자식 2개: 오른쪽 서브트리의 최솟값(후계자)으로 값을 바꾸고, 오른쪽에서 후계자를 지운다.
        else:
            successor = root.right
            while successor.left:
                successor = successor.left
            root.value = successor.value
            root.right = remove(root.right, successor.value)

    return root
