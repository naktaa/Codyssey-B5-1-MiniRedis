#include "LinkedList.h"

void insert_front(LinkedList* list, Node* now) {
    if (list->HEAD == NULL) {
        list->HEAD = list->TAIL = now;
        now->prev = now->next = NULL;
    }
    else {
        now->prev = NULL, now->next = list->HEAD;
        list->HEAD->prev = now;
        list->HEAD = now;
    }
}

void insert_back(LinkedList* list, Node* now) {
    if (list->TAIL == NULL) {
        now->prev = now->next = NULL;
        list->HEAD = list->TAIL = now;
    }
    else {
        now->prev = list->TAIL, now->next = NULL;
        list->TAIL->next = now;
        list->TAIL = now;
    }
}

void remove_front(LinkedList* list) {
    if (list->HEAD == NULL && list->TAIL == NULL) {
        printf("empty list!\n");
    }
    else {
        Node* temp = list->HEAD;
        if (list->HEAD == list->TAIL) {
            list->HEAD = list->TAIL = NULL;
        }
        else {
            list->HEAD = list->HEAD->next;
            list->HEAD->prev = NULL;
        }
        free(temp);
    }
}

void remove_back(LinkedList* list) {
    if (list->HEAD == NULL && list->TAIL == NULL) {
        printf("empty list!\n");
    }
    else {
        Node* temp = list->TAIL;
        if (list->HEAD == list->TAIL) {
            list->HEAD = list->TAIL = NULL;
        }
        else {
            list->TAIL = list->TAIL->prev;
            list->TAIL->next = NULL;
        }
        free(temp);
    }
}

void remove_node(LinkedList* list, Node* now) {
    if (now == list->HEAD) {
        remove_front(list);
    }
    else if (now == list->TAIL) {
        remove_back(list);
    }
    else {
        now->prev->next = now->next;
        now->next->prev = now->prev;
        free(now);
    }
}

void move_to_front(LinkedList* list, Node* now) {
    if (list->HEAD == now) {
        printf("already front!\n");
        return;
    }
    else {
        now->prev->next = now->next;
        if (now == list->TAIL) {
            list->TAIL = now->prev;
        }
        else {
            now->next->prev = now->prev;
        }

        list->HEAD->prev = now;
        now->prev = NULL, now->next = list->HEAD;
        list->HEAD = now;
    }
}