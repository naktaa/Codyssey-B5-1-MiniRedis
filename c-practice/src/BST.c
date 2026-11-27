#include <stdlib.h>

#include "BST.h"

/*
 * 보너스 4 BST: 모든 노드에서 왼쪽 서브트리 값 < 노드 값 < 오른쪽 서브트리 값.
 * 이 규칙 덕분에 탐색·삽입·삭제가 한 갈래만 내려가고(높이만큼), 중위 순회가 정렬 순서가 된다.
 *
 *          8
 *        /   \
 *       3     10
 *      / \      \
 *     1   6      14
 *        / \     /
 *       4   7   13
 */

/* 규칙대로 내려가 빈 자리(NULL)에 새 노드를 붙인다. 재귀가 돌아오며 부모의 left/right에 다시 연결한다. */
TreeNode* bst_insert(TreeNode* root, int value) {
    if (root == NULL) {
        TreeNode* NewNode = malloc(sizeof(TreeNode));
        if (NewNode == NULL) {
            return NULL; // 빈 자리였으므로 NULL을 돌려줘도 트리는 그대로다.
        }
        NewNode->value = value;
        NewNode->left = NewNode->right = NULL;
        return NewNode;
    }

    if (value < root->value) {
        root->left = bst_insert(root->left, value);
    }
    else if (value > root->value) {
        root->right = bst_insert(root->right, value);
    }
    // 같은 값: 해시맵 키처럼 중복을 두지 않는다. 아무것도 바꾸지 않는다.

    return root;
}

/* 작으면 왼쪽, 크면 오른쪽으로 한 갈래만 내려간다. 반복문으로 충분해서 재귀를 쓰지 않는다. */
TreeNode* bst_search(TreeNode* root, int value) {
    TreeNode* now = root;
    while (now) {
        if (value == now->value) {
            return now;
        }
        else if (value < now->value) {
            now = now->left;
        }
        else {
            now = now->right;
        }
    }

    return NULL;
}

/*
 * value를 지우고 이 서브트리의 새 루트를 돌려준다. 부모는 돌려받은 노드를 자기 left/right에 다시 연결한다.
 * 지울 노드의 자식 수에 따라 세 경우로 나눈다.
 */
TreeNode* bst_remove(TreeNode* root, int value) {
    if (root == NULL) {
        return NULL; // 없는 값: 끝까지 내려와도 못 찾았다.
    }

    if (value < root->value) {
        root->left = bst_remove(root->left, value);
    }
    else if (value > root->value) {
        root->right = bst_remove(root->right, value);
    }
    else {
        // 자식 0개: 그냥 떼어 낸다. 부모의 그 자리는 NULL이 된다.
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        // 자식 1개: 그 자식이 지운 자리로 올라간다. 자식 쪽 값은 모두 부모 기준 같은 방향이라 규칙이 유지된다.
        else if (root->left == NULL || root->right == NULL) {
            TreeNode* child = root->left ? root->left : root->right;
            free(root);
            return child;
        }
        // 자식 2개: 오른쪽 서브트리의 최솟값(후계자)을 찾는다.
        // 후계자는 "지울 값보다 큰 값 중 가장 작은 값"이라 이 자리에 와도 왼쪽보다 크고 오른쪽보다 작다.
        else {
            TreeNode* successor = root->right;
            while (successor->left) {
                successor = successor->left;
            }
            // 노드를 옮기지 않고 값만 복사한 뒤, 오른쪽 서브트리에서 후계자를 지운다.
            // 후계자는 왼쪽 자식이 없으므로 그 삭제는 자식 0개나 1개 경우로 끝난다.
            root->value = successor->value;
            root->right = bst_remove(root->right, successor->value);
        }
    }

    return root;
}
