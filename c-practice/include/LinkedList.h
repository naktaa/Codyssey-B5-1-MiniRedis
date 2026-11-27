#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct Node Node;
typedef struct LinkedList LinkedList;

struct Node {
    void* data;
    Node* prev;
    Node* next;
};

struct LinkedList {
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
