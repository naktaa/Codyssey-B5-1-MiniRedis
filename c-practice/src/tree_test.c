#include <stdio.h>

#include "BinaryTree.h"

/* 순회 4개를 기대값과 나란히 출력한다. */
static void print_all(TreeNode* root, const char* pre, const char* in, const char* post, const char* level) {
    printf("전위 -> "); tree_preorder(root);   printf("(기대: %s)\n", pre);
    printf("중위 -> "); tree_inorder(root);    printf("(기대: %s)\n", in);
    printf("후위 -> "); tree_postorder(root);  printf("(기대: %s)\n", post);
    printf("레벨 -> "); tree_levelorder(root); printf("(기대: %s)\n", level);
}

/* 실습 확인용 main. 각 줄의 (기대: ...)와 출력이 같은지 확인한다. */
int main(void) {
    printf("== 노드 7개 (꽉 찬 트리) ==\n");
    printf("        1\n      /   \\\n     2     3\n    / \\   / \\\n   4   5 6   7\n");
    int arr7[] = { 1, 2, 3, 4, 5, 6, 7 };
    TreeNode* root = tree_build(arr7, 7);
    if (root == NULL) {
        return 1;
    }
    print_all(root, "1 2 4 5 3 6 7", "4 2 5 1 6 3 7", "4 5 2 6 7 3 1", "1 2 3 4 5 6 7");
    printf("레벨 순회 = 원래 배열 순서: 힙이 트리를 배열에 담는 순서와 같다\n");
    tree_free(root);

    printf("\n== 노드 6개 (마지막 층 오른쪽이 빔) ==\n");
    printf("        1\n      /   \\\n     2     3\n    / \\   /\n   4   5 6\n");
    int arr6[] = { 1, 2, 3, 4, 5, 6 };
    root = tree_build(arr6, 6);
    if (root == NULL) {
        return 1;
    }
    print_all(root, "1 2 4 5 3 6", "4 2 5 1 6 3", "4 5 2 6 3 1", "1 2 3 4 5 6");
    printf("3의 오른쪽 자식 -> %s (기대: NULL)\n", root->right->right == NULL ? "NULL" : "있음");
    tree_free(root);

    printf("\n== 노드 1개 ==\n");
    int arr1[] = { 42 };
    root = tree_build(arr1, 1);
    if (root == NULL) {
        return 1;
    }
    print_all(root, "42", "42", "42", "42");
    tree_free(root);

    printf("\n== 빈 트리 ==\n");
    root = tree_build(arr1, 0);
    printf("tree_build(n=0) -> %s (기대: NULL)\n", root == NULL ? "NULL" : "노드");
    print_all(root, "(없음)", "(없음)", "(없음)", "(없음)");
    tree_free(root); // NULL이어도 안전해야 한다

    return 0;
}
