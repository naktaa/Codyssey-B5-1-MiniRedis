#include <stdio.h>
#include <stdlib.h>

#include "BinaryTree.h"
#include "LinkedList.h"

/*
 * 보너스 3 이진 트리: 노드마다 왼쪽·오른쪽 자식 포인터를 가진다.
 * tree_build는 배열을 힙과 같은 인덱스 규칙으로 노드 트리로 바꾼다.
 *   arr[i]의 왼쪽 자식 = arr[2i + 1], 오른쪽 자식 = arr[2i + 2]
 * 그래서 같은 데이터를 "배열(힙)"과 "포인터로 이은 노드" 두 방식으로 비교할 수 있다.
 *
 *          arr[0]
 *         /      \
 *     arr[1]    arr[2]
 *     /   \     /   \
 *  arr[3] arr[4] arr[5] arr[6]
 */

/* i번 칸을 루트로 하는 서브트리를 만든다. 범위 밖이면 NULL. 할당 실패면 만든 만큼 해제하고 NULL. */
static TreeNode* _build(const int* arr, size_t n, size_t i) {
    if (i >= n) {
        return NULL;
    }

    TreeNode* NewNode = malloc(sizeof(TreeNode));
    if (NewNode == NULL) {
        return NULL;
    }
    NewNode->value = arr[i];
    NewNode->left = _build(arr, n, i * 2 + 1);
    NewNode->right = _build(arr, n, i * 2 + 2);

    // 자식 칸이 범위 안인데 NULL이면 범위 밖이 아니라 할당 실패다. 반쯤 만든 트리를 남기지 않는다.
    if ((i * 2 + 1 < n && NewNode->left == NULL) || (i * 2 + 2 < n && NewNode->right == NULL)) {
        tree_free(NewNode);
        return NULL;
    }

    return NewNode;
}

TreeNode* tree_build(const int* arr, size_t n) {
    return _build(arr, n, 0);
}

/* 전위: 루트 → 왼쪽 → 오른쪽. 루트를 가장 먼저 찍는다. */
void tree_preorder(const TreeNode* root) {
    if (root == NULL) {
        return;
    }
    printf("%d ", root->value);
    tree_preorder(root->left);
    tree_preorder(root->right);
}

/* 중위: 왼쪽 → 루트 → 오른쪽. 루트를 왼쪽 서브트리와 오른쪽 서브트리 사이에 찍는다. */
void tree_inorder(const TreeNode* root) {
    if (root == NULL) {
        return;
    }
    tree_inorder(root->left);
    printf("%d ", root->value);
    tree_inorder(root->right);
}

/* 후위: 왼쪽 → 오른쪽 → 루트. 자식을 다 본 뒤 루트를 찍는다(tree_free가 이 순서를 쓴다). */
void tree_postorder(const TreeNode* root) {
    if (root == NULL) {
        return;
    }
    tree_postorder(root->left);
    tree_postorder(root->right);
    printf("%d ", root->value);
}

/* 큐 뒤(TAIL)에 트리 노드를 넣는다. 리스트 Node의 data가 TreeNode를 빌려 가리킨다. 반환: 성공 1, 할당 실패 0. */
static int _enqueue(LinkedList* queue, TreeNode* tree_node) {
    Node* NewNode = malloc(sizeof(Node));
    if (NewNode == NULL) {
        return 0;
    }
    NewNode->data = tree_node;
    insert_back(queue, NewNode);

    return 1;
}

/*
 * 레벨: 위층부터 왼쪽 → 오른쪽. 가까운 노드부터 방문해야 해서 큐(FIFO)를 쓴다.
 * 큐는 직접 만든 LinkedList: enqueue = insert_back, dequeue = remove_front.
 */
void tree_levelorder(TreeNode* root) {
    if (root == NULL) {
        return;
    }

    LinkedList queue = { NULL, NULL };
    if (!_enqueue(&queue, root)) {
        return;
    }

    while (queue.HEAD) {
        // remove_front는 Node를 해제하고 data를 돌려주지 않으므로 트리 노드 주소를 먼저 꺼낸다.
        TreeNode* now = queue.HEAD->data;
        remove_front(&queue);
        printf("%d ", now->value);

        if ((now->left && !_enqueue(&queue, now->left)) || (now->right && !_enqueue(&queue, now->right))) {
            // 할당 실패: 남은 큐 Node를 모두 해제하고 멈춘다. 트리 노드는 빌린 주소라 건드리지 않는다.
            while (queue.HEAD) {
                remove_front(&queue);
            }
            return;
        }
    }
}

/* 모든 노드를 해제한다. 후위 순서: 부모를 먼저 해제하면 자식 주소를 읽을 수 없게 된다. */
void tree_free(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    tree_free(root->left);
    tree_free(root->right);
    free(root);
}
