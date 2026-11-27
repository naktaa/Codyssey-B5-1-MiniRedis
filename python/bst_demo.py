"""시연용: 섞인 순서로 넣은 값이 BST 중위 순회에서 정렬되어 나오는 것을 보여 준다."""

from binary_tree import inorder
from bst import insert, search, remove


def show(label, arr):
    """DynamicArray에 담긴 방문 순서를 공백으로 이어 출력한다."""
    print(label, " ".join(str(arr.get(i)) for i in range(arr.count)))


def main():
    values = [40, 80, 20, 60, 50, 30, 70]
    root = None
    for v in values:
        root = insert(root, v)

    print("삽입 순서:", " ".join(str(v) for v in values), "\n")
    print("      40\n     /  \\\n   20    80\n     \\   /\n     30 60\n        / \\\n       50  70\n")

    show("중위 순회 (정렬 결과) :", inorder(root))

    print("\n탐색 60 ->", "있음" if search(root, 60) else "없음")
    print("탐색 65 ->", "있음" if search(root, 65) else "없음")

    root = remove(root, 40)
    print("\n40 삭제 (루트, 자식 2개 -> 후계자 50이 자리를 채움)")
    show("중위 순회 (정렬 결과) :", inorder(root))


if __name__ == "__main__":
    main()
