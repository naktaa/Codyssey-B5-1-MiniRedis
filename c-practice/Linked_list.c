#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* prev;
    struct Node* next;
} *HEAD, *TAIL, *TEMP;

void insert_front(struct Node* now) {
    if (HEAD == NULL) {
        HEAD = TAIL = now;
    }
    else {
        now->next = HEAD;
        HEAD->prev = now;
        HEAD = now;
    }
}

void insert_back(struct Node* now) {
    if (TAIL == NULL) {
        HEAD = TAIL = now;
    }
    else {
        now->prev = TAIL;
        TAIL->next = now;
        TAIL = now;
    }
}

void remove_front() {
    if (HEAD == NULL && TAIL == NULL) {
        printf("empty list!\n");
    }
    else {
        TEMP = HEAD;
        if (HEAD == TAIL) {
            HEAD = TAIL = NULL;
        }
        else {
            HEAD = HEAD->next;
            HEAD->prev = NULL;
        }
        free(TEMP);
    }
}

void remove_back() {
    if (HEAD == NULL && TAIL == NULL) {
        printf("empty list!\n");
    }
    else {
        TEMP = TAIL;
        if (HEAD == TAIL) {
            HEAD = TAIL = NULL;
        }
        else {
            TAIL = TAIL->prev;
            TAIL->next = NULL;
        }
        free(TEMP);
    }
}

void remove_node(struct Node* now) {
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

void move_to_front() {
    //
}

int main() {
    HEAD = TAIL = TEMP = NULL;

    for (int i = 0; i < 10; i++) {
        TEMP = (struct Node*)malloc(sizeof(struct Node));
        if (TEMP == NULL) {
            printf("malloc failed!\n");
            return 1;
        }

        TEMP->data = i + 1;
        TEMP->prev = TEMP->next = NULL;

        insert_front(TEMP);
    }
}