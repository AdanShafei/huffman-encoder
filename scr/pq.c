#include "pq.h"

#include "node.h"

#include <stdio.h>
typedef struct ListElement ListElement;
struct ListElement {
    Node *tree;
    ListElement *next;
};

struct PriorityQueue {
    ListElement *list;
};

// function that creates priority queue object and returns a pointer
PriorityQueue *pq_create(void) {
    // allocating the priority queue in the memory
    PriorityQueue *pq = (PriorityQueue *) calloc(1, sizeof(PriorityQueue));
    // if the priority queue is NULL the function returns NULL
    if (pq == NULL) {
        return NULL;
    }
    // initialize the list to NULL
    pq->list = NULL;
    // function returns pq
    return pq;
}

// function that free the priority queue
void pq_free(PriorityQueue **q) {
    // if the priority queue is NULL
    if (*q == NULL) {
        // function returns
        return;
    }
    // iterate over the priority queue and free each node
    ListElement *e = (*q)->list;
    while (e != NULL) {
        // freeing the nodes
        node_free(&(e->tree));
        e = e->next;
    }
    // freeing the priority queue
    free(*q);
    // setting priority queue to NULL
    *q = NULL;
}

// function that checks if the the priority queue is empty
bool pq_is_empty(PriorityQueue *q) {
    // check if the priority queue is NULL or the list is empty
    if (q == NULL || q->list == NULL) {
        // fucntion returns true
        return true;
    }
    // if not function returns false
    return false;
}

// function that checks is the size of the queue if 1
bool pq_size_is_1(PriorityQueue *q) {
    // check if priority queue is not NULL and the next item it NULL
    return q->list != NULL && q->list->next == NULL;
}

// based on asgn8.pdf
// function that checks if the first tree is less than the second tree
bool pq_less_than(ListElement *e1, ListElement *e2) {
    // compare the weights of the trees
    // if the first weight is less
    if (e1->tree->weight < e2->tree->weight) {
        // function returns true
        return true;
    }
    // if weights are equal, compare the symbols
    else if (e1->tree->weight == e2->tree->weight) {
        // function returns the comparison result
        return (e1->tree->symbol < e2->tree->symbol);
    }
    // first tree is not less than the second tree
    else {
        // function returns false
        return false;
    }
}

// based on asgn8.pdf
// fucntion that enqueue
void enqueue(PriorityQueue *q, Node *tree) {
    // allocate a new ListElement
    ListElement *new_element = (ListElement *) calloc(1, sizeof(ListElement));
    if (new_element == NULL) {
        // if allocation fails return
        return;
    }
    // set the tree field to the value of the tree function parameter
    new_element->tree = tree;
    // if the queue is empty
    if (pq_is_empty(q)) {
        // add the new element
        q->list = new_element;
        // checking if the new element is less than the tree
    } else if (pq_less_than(new_element, q->list)) {
        // new element goes before all existing elements of the list
        new_element->next = q->list;
        q->list = new_element;
    } else {
        // find the existing element where the new element should be placed after
        ListElement *prev = q->list;
        while (prev->next != NULL && pq_less_than(prev->next, new_element)) {
            prev = prev->next;
        }
        // insert new element after element 2
        new_element->next = prev->next;
        prev->next = new_element;
    }
}

// function that dequeue
Node *dequeue(PriorityQueue *q) {
    // check if the queue is empty
    if (pq_is_empty(q)) {
        // printing error message and return
        fprintf(stderr, "Error: Attempted to dequeue from an empty queue.\n");
        return NULL;
    }
    // get the front element of the queue
    ListElement *front = q->list;
    Node *dequeued_tree = front->tree;
    // update the queue to remove the front element
    q->list = front->next;
    // free the ListElement (but not the tree itself)
    free(front);
    // return the dequeued tree
    return dequeued_tree;
}

// based on asgn8.pdf
// function that prints the trees in the priority queue
void pq_print(PriorityQueue *q) {
    // check if the priority queue is not NULL
    assert(q != NULL);
    // start from the beginning of the list
    ListElement *e = q->list;
    // variable to keep track of the position
    int position = 1;
    // iterate over the list and print each tree
    while (e != NULL) {
        // print a separator based on the position
        if (position++ == 1) {
            printf("=============================================\n");
        } else {
            printf("---------------------------------------------\n");
        }
        // print the tree using a helper function
        node_print_tree(e->tree);
        // move to the next element in the list
        e = e->next;
    }
    // print a final separator at the end
    printf("=============================================\n");
}
