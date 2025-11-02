#include "bitreader.h"
#include "node.h"
#include "pq.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#define STACK_SIZE 64

// function that pushes nodes to the stack
void stack_push(Node **stack, int *top, Node *node) {
    // check if stack has space left
    if (*top < STACK_SIZE - 1) {
        // push node to stack
        stack[++(*top)] = node;
    } else {
        // stack overflow error - couldn't push to stack
        fprintf(stderr, "couldn't push to stack\n");
    }
}

// function that pops nodes from the stack
Node *stack_pop(Node **stack, int *top) {
    // check if stack is greater ot equal to 0
    if (*top >= 0) {
        // pop node from stack
        return stack[(*top)--];
    } else {
        // stack underflow error - couldn't pop from stack
        fprintf(stderr, "couldn't pop from stack\n");
        return NULL;
    }
}

// function to perform Huffman decoding and write the decompressed data to the output file
void dehuff_decompress_file(FILE *fout, BitReader *inbuf) {
    // using the bit read functions to read header information
    uint8_t type1 = bit_read_uint8(inbuf);
    uint8_t type2 = bit_read_uint8(inbuf);
    // read file size
    uint32_t filesize = bit_read_uint32(inbuf);
    // read the number of symbols in the file
    uint16_t num_leaves = bit_read_uint16(inbuf);
    // using the assert functions to check is the header information correct
    assert(type1 == 'H');
    assert(type2 == 'C');
    // calculate the number of nodes in the Huffman tree
    uint16_t num_nodes = 2 * num_leaves - 1;
    // initialize a stack for building the Huffman tree
    Node *stack[64];
    // initialize top of the stack
    int top = -1;
    // iterating over the number of nodes to build the tree for huffman
    for (uint16_t i = 0; i < num_nodes; ++i) {
        // read one bit to determine node type
        int bit = bit_read_bit(inbuf);
        // create a new node based on the bit value
        Node *node;
        // if bit is 1
        if (bit == 1) {
            // creating the leaf of the node using symbol
            uint8_t symbol = bit_read_uint8(inbuf);
            node = node_create(symbol, 0);
        } else {
            // creating an internal node
            node = node_create(0, 0);
            // popping the right side
            node->right = stack_pop(stack, &top);
            // popping the left side
            node->left = stack_pop(stack, &top);
        }
        // push the new node onto the stack
        stack_push(stack, &top, node);
    }
    // the top of the stack now contains the root of the Huffman tree
    if (top < 0) {
        fprintf(stderr, "Error: Empty stack\n");
        return;
    }
    // the top of the stack now contains the root of the huffman tree
    Node *code_tree = stack_pop(stack, &top);
    // iterating over the file size to decode the compressed data
    for (uint32_t i = 0; i < filesize; ++i) {
        Node *node = code_tree;
        // traverse the Huffman tree until a leaf node is reached
        while (1) {
            int bit = bit_read_bit(inbuf);
            // move to the left or right child based on the bit value
            // if bit is 0
            if (bit == 0) {
                // move node to the left
                node = node->left;
                // if not
            } else {
                // move node to the right
                node = node->right;
            }
            // if it's a leaf node, break from the loop
            if (node->left == NULL || node->right == NULL) {
                break;
            }
        }
        // write the symbol of the leaf node to the output file
        fputc(node->symbol, fout);
    }
    // freeing the memory used for creating the node
    node_free(&code_tree);
}

// function that prints the usage message
void print_help(void) {
    fprintf(stdout, "Usage: huff -i infile -o outfile\n"
                    "       huff -v -i infile -o outfile\n"
                    "       huff -h\n");
}

// the main function
int main(int argc, char **argv) {
    // defining option to use in getopt()
    int option;
    // defining a file to write the output in
    FILE *fout = NULL;
    // definig a variable to take the input file
    const char *finame;
    // checking the input and output files are provided
    while (argc < 5) {
        if (argc < 3) {
            fprintf(stderr, "input file is required\n");
            print_help();
            return 1;
        } else if (argc <= 4) {
            fprintf(stderr, "output file is required\n");
            print_help();
            return 1;
        }
    }
    // while the user provides an option
    while ((option = getopt(argc, argv, "hi:o:")) != -1) {
        // checking the options that were provided (using switch)
        switch (option) {
        // if the option was 'h' print the help message
        case 'h': print_help(); return 0;
        // if the option was 'i' use the input file
        case 'i': finame = optarg; break;
        // if the option was 'o' print the output into this file
        case 'o':
            // taking the output file from the command line (using w to write in the file)
            fout = fopen(optarg, "w");
            // check that fout is not NULL
            if (fout == NULL) {
                // print error
                fprintf(stderr, "Error opening output file\n");
                // print help
                print_help();
                // exiting the program
                return 1;
            }
            break;
            // the default case it to break
        default: break;
        } // end of switch
    } // end of while loop

    // opening the input file to read it
    BitReader *inbuf = bit_read_open(finame);
    // using the decompressing function to decode the input
    dehuff_decompress_file(fout, inbuf);
    // closing the input file
    bit_read_close(&inbuf);
    // closing the output file
    fclose(fout);
    return 0;
} // end of main
