#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* prev;
    struct Node* next;
} Node;
Node *HEAD = NULL, *TAIL = NULL;

void insert_front(Node*);
void insert_back(Node*);
void remove_front();
void remove_back();
void remove_node(Node*);
void move_to_front(Node*);

int main() {
    for (int i = 0; i < 10; i++) {
        Node* NEW = malloc(sizeof(Node));
        if (NEW == NULL) {
            printf("malloc failed!\n");
            return 1;
        }

        NEW->data = i + 1;
        NEW->prev = NEW->next = NULL;

        insert_front(NEW);
    }
}

void insert_front(Node* now) {
    if (HEAD == NULL) {
        HEAD = TAIL = now;
        now->prev = now->next = NULL;
    }
    else {
        now->prev = NULL, now->next = HEAD;
        HEAD->prev = now;
        HEAD = now;
    }
}

void insert_back(Node* now) {
    if (TAIL == NULL) {
        now->prev = now->next = NULL;
        HEAD = TAIL = now;
    }
    else {
        now->prev = TAIL, now->next = NULL;
        TAIL->next = now;
        TAIL = now;
    }
}

void remove_front() {
    if (HEAD == NULL && TAIL == NULL) {
        printf("empty list!\n");
    }
    else {
        Node* temp = HEAD;
        if (HEAD == TAIL) {
            HEAD = TAIL = NULL;
        }
        else {
            HEAD = HEAD->next;
            HEAD->prev = NULL;
        }
        free(temp);
    }
}

void remove_back() {
    if (HEAD == NULL && TAIL == NULL) {
        printf("empty list!\n");
    }
    else {
        Node* temp = TAIL;
        if (HEAD == TAIL) {
            HEAD = TAIL = NULL;
        }
        else {
            TAIL = TAIL->prev;
            TAIL->next = NULL;
        }
        free(temp);
    }
}

void remove_node(Node* now) {
    if (now == HEAD) {
        remove_front();
    }
    else if (now == TAIL) {
        remove_back();
    }
    else {
        now->prev->next = now->next;
        now->next->prev = now->prev;
        free(now);
    }
}

void move_to_front(Node* now) {
    if (HEAD == now) {
        printf("already front!\n");
        return;
    }
    else {
        now->prev->next = now->next;
        if (now == TAIL) {
            TAIL = now->prev;
        }
        else {
            now->next->prev = now->prev;
        }

        HEAD->prev = now;
        now->prev = NULL, now->next = HEAD;
        HEAD = now;
    }
}