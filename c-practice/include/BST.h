#ifndef BST_H
#define BST_H

#include "BinaryTree.h"

/*
 * 이진 탐색 트리. 노드는 BinaryTree.h의 TreeNode를 그대로 쓰고,
 * 중위 순회(정렬 결과)는 tree_inorder, 해제는 tree_free를 쓴다.
 *
 * insert/remove 반환: 바뀐 뒤의 루트. 빈 트리에 넣으면 새 노드가, 루트를 지우면 새 루트가 루트가 된다.
 * insert는 같은 값이 이미 있거나 할당에 실패하면 트리를 바꾸지 않는다.
 * search 반환: 값이 있는 노드, 없으면 NULL.
 */
TreeNode* bst_insert(TreeNode* root, int value);
TreeNode* bst_search(TreeNode* root, int value);
TreeNode* bst_remove(TreeNode* root, int value);

#endif
