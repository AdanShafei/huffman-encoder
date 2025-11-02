#include "node.h"

#include <stdio.h>
#include <stdlib.h>

// all the functions in this file are written based on the sudo code given in asgn8.pdf
// function that creates the node
Node *node_create(uint8_t symbol, uint32_t weight) {
    // allocate memory for the new node
    Node *new_node = (Node *) calloc(1, sizeof(Node));
    if (new_node == NULL) {
        return NULL;
    }
    // assign the given symbol to the new node
    new_node->symbol = symbol;
    // assign the given weight to the new node
    new_node->weight = weight;
    // initialize the code value to 0
    new_node->code = 0;
    // initialize the code length to 0
    new_node->code_length = 0;
    // initialize the left child to NULL
    new_node->left = NULL;
    // initialize the right child to NULL
    new_node->right = NULL;
    // return the created node
    return new_node;
}

// function that frees all the memory used to allocate the node
void node_free(Node **node) {
    // check if node is not NULL
    if (*node != NULL) {
        // using recursion
        // free left node
        node_free(&(*node)->left);
        // free right node
        node_free(&(*node)->right);
        free(*node);
        // set the pointer to NULL
        *node = NULL;
    }
}

// fucntion that prints the node
void node_print_node(Node *tree, char ch, int indentation) {
    // if tree is empty return
    if (tree == NULL)
        return;
    // using recursion to print right subtree
    node_print_node(tree->right, '/', indentation + 3);
    // print node info
    printf("%*cweight = %u", indentation + 1, ch, tree->weight);
    // if node is a leaf
    if (tree->left == NULL && tree->right == NULL) {
        if (' ' <= tree->symbol && tree->symbol <= '~') {
            // print ASCII symbol
            printf(", symbol = '%c'", tree->symbol);

        } else {
            // else print non-ASCII symbol
            printf(", symbol = 0x%02x", tree->symbol);
        }
    }
    printf("\n");
    // using recursion to print left subtree
    node_print_node(tree->left, '\\', indentation + 3);
}

// function that prints the tree (using the node printing fucntion)
void node_print_tree(Node *tree) {
    // calling the function print node to print every node of the tree
    node_print_node(tree, '<', 2);
}
