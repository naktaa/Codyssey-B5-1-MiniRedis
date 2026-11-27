#include <stdio.h>

#include "BinaryTree.h"

/* 시연용 main. 트리 하나를 만들어 순회 4개가 어떤 순서로 나오는지 보여 준다. */
int main(void) {
    int arr[] = { 1, 2, 3, 4, 5, 6, 7 };
    TreeNode* root = tree_build(arr, 7);
    if (root == NULL) {
        return 1;
    }

    printf("배열 {1, 2, 3, 4, 5, 6, 7} -> 완전 이진 트리\n\n");
    printf("        1\n      /   \\\n     2     3\n    / \\   / \\\n   4   5 6   7\n\n");

    printf("전위 (나 -> 왼 -> 오른) : "); tree_preorder(root);   printf("\n");
    printf("중위 (왼 -> 나 -> 오른) : "); tree_inorder(root);    printf("\n");
    printf("후위 (왼 -> 오른 -> 나) : "); tree_postorder(root);  printf("\n");
    printf("레벨 (위층부터 왼->오)  : "); tree_levelorder(root); printf("\n");

    tree_free(root);
    return 0;
}
