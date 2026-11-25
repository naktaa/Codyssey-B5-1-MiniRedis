#include <stdio.h>

#include "BST.h"

/* 전위(모양 확인)와 중위(정렬 확인)를 기대값과 나란히 출력한다. */
static void print_tree(TreeNode* root, const char* pre, const char* in) {
    printf("  전위 -> "); tree_preorder(root); printf("(기대: %s)\n", pre);
    printf("  중위 -> "); tree_inorder(root);  printf("(기대: %s)\n", in);
}

/* 실습 확인용 main. 각 줄의 (기대: ...)와 출력이 같은지 확인한다. */
int main(void) {
    TreeNode* root = NULL;

    printf("== 삽입: 8 3 10 1 6 14 4 7 13 ==\n");
    printf("          8\n        /   \\\n       3     10\n      / \\      \\\n     1   6      14\n        / \\     /\n       4   7   13\n");
    int values[] = { 8, 3, 10, 1, 6, 14, 4, 7, 13 };
    for (int i = 0; i < 9; i++) {
        root = bst_insert(root, values[i]);
    }
    print_tree(root, "8 3 1 6 4 7 10 14 13", "1 3 4 6 7 8 10 13 14");
    printf("중위 순회가 정렬 순서: 왼쪽(작음) → 루트 → 오른쪽(큼)\n");

    printf("\n== 중복 삽입 6 ==\n");
    root = bst_insert(root, 6);
    print_tree(root, "8 3 1 6 4 7 10 14 13", "1 3 4 6 7 8 10 13 14");

    printf("\n== 탐색 ==\n");
    TreeNode* found = bst_search(root, 6);
    printf("search 6  -> %s (기대: 6)\n", found ? "6" : "NULL");
    if (found) {
        printf("  6의 자식 -> %d, %d (기대: 4, 7)\n", found->left->value, found->right->value);
    }
    printf("search 5  -> %s (기대: NULL)\n", bst_search(root, 5) ? "있음" : "NULL");
    printf("빈 트리   -> %s (기대: NULL)\n", bst_search(NULL, 5) ? "있음" : "NULL");

    printf("\n== 삭제: 자식 0개 (13) ==\n");
    root = bst_remove(root, 13);
    print_tree(root, "8 3 1 6 4 7 10 14", "1 3 4 6 7 8 10 14");

    printf("\n== 삭제: 자식 1개 (10, 오른쪽 자식 14가 올라옴) ==\n");
    root = bst_remove(root, 10);
    print_tree(root, "8 3 1 6 4 7 14", "1 3 4 6 7 8 14");

    printf("\n== 삭제: 자식 2개 (3, 후계자 4가 그 자리로) ==\n");
    root = bst_remove(root, 3);
    print_tree(root, "8 4 1 6 7 14", "1 4 6 7 8 14");

    printf("\n== 삭제: 루트 (8, 후계자 14가 루트로) ==\n");
    root = bst_remove(root, 8);
    print_tree(root, "14 4 1 6 7", "1 4 6 7 14");
    printf("  새 루트 -> %d (기대: 14)\n", root ? root->value : -1);

    printf("\n== 삭제: 없는 값 (99) ==\n");
    root = bst_remove(root, 99);
    print_tree(root, "14 4 1 6 7", "1 4 6 7 14");

    printf("\n== 전부 삭제 후 다시 삽입 ==\n");
    int rest[] = { 1, 4, 6, 7, 14 };
    for (int i = 0; i < 5; i++) {
        root = bst_remove(root, rest[i]);
    }
    printf("  전부 삭제 -> %s (기대: NULL)\n", root == NULL ? "NULL" : "노드 남음");
    root = bst_insert(root, 5);
    print_tree(root, "5", "5");
    tree_free(root);

    printf("\n== 정렬된 값을 차례로 삽입: 1 2 3 4 5 ==\n");
    root = NULL;
    for (int i = 1; i <= 5; i++) {
        root = bst_insert(root, i);
    }
    print_tree(root, "1 2 3 4 5", "1 2 3 4 5");
    printf("  오른쪽으로만 이어짐 -> ");
    for (TreeNode* now = root; now; now = now->right) {
        printf("%d%s", now->value, now->right ? " → " : "");
    }
    printf(" (기대: 1 → 2 → 3 → 4 → 5, 연결 리스트처럼 치우쳐 탐색이 느려짐)\n");
    tree_free(root);

    return 0;
}
