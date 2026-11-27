#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <stddef.h>

typedef struct TreeNode TreeNode;

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
};

/*
 * tree_build: 배열을 완전 이진 트리로 만든다. 반환: 루트, n == 0이거나 할당 실패면 NULL.
 * 순회 4개는 방문한 값을 공백으로 구분해 출력한다(줄바꿈은 호출한 쪽이 한다).
 * 소유권: tree_build가 만든 노드는 tree_free로 해제한다.
 */
TreeNode* tree_build(const int* arr, size_t n);
void tree_preorder(const TreeNode* root);
void tree_inorder(const TreeNode* root);
void tree_postorder(const TreeNode* root);
void tree_levelorder(TreeNode* root);
void tree_free(TreeNode* root);

#endif
