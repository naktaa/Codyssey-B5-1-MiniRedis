"""시연용: 트리 하나를 만들어 전위/중위/후위 순회가 어떤 순서로 나오는지 보여 준다."""

from binary_tree import build, preorder, inorder, postorder


def show(label, arr):
    """DynamicArray에 담긴 방문 순서를 공백으로 이어 출력한다."""
    print(label, " ".join(str(arr.get(i)) for i in range(arr.count)))


def main():
    root = build([1, 2, 3, 4, 5, 6, 7])

    print("배열 [1, 2, 3, 4, 5, 6, 7] -> 완전 이진 트리\n")
    print("        1\n      /   \\\n     2     3\n    / \\   / \\\n   4   5 6   7\n")

    show("전위 (나 -> 왼 -> 오른) :", preorder(root))
    show("중위 (왼 -> 나 -> 오른) :", inorder(root))
    show("후위 (왼 -> 오른 -> 나) :", postorder(root))


if __name__ == "__main__":
    main()
