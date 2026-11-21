#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct node Node;
typedef struct linkedlist LinkedList;

struct node {
    void* data;
    Node* prev;
    Node* next;
};

struct linkedlist {
    Node* HEAD;
    Node* TAIL;
};

void insert_front(LinkedList*, Node*);
void insert_back(LinkedList*, Node*);
void remove_front(LinkedList*);
void remove_back(LinkedList*);
void remove_node(LinkedList*, Node*);
void move_to_front(LinkedList*, Node*);

#endif
