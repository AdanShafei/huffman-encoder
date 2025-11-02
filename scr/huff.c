#include "bitreader.h"
#include "bitwriter.h"
#include "node.h"
#include "pq.h"

#include <getopt.h>
#include <stdio.h>

typedef struct Code {
    uint64_t code;
    uint8_t code_length;
} Code;

// function that fills the histogram
uint32_t fill_histogram(FILE *fin, uint32_t *histogram) {
    // clearing all elements of the histogram array
    for (int i = 0; i < 256; ++i) {
        histogram[i] = 0;
    }
    // at least 2 values of the histogram are not zero
    ++histogram[0x00];
    ++histogram[0xff];
    // initialize the total size of the file
    uint32_t filesize = 0;
    // getting the file name
    int byte;
    // using a while loop to read bytes from the input file
    while ((byte = fgetc(fin)) != EOF) {
        // updating the histogram
        // increment histogram
        ++histogram[byte];
        // increment filesize
        ++filesize;
    }
    // rewind the input file to the beginning
    fseek(fin, 0, SEEK_SET);
    // return the file size
    return filesize;
}

// function that creates the tree
Node *create_tree(uint32_t *histogram, uint16_t *num_leaves) {
    // create priority queue
    PriorityQueue *pq = pq_create();
    // iterate over the priority queue to fill the histogram
    for (uint16_t i = 0; i < 256; ++i) {
        if (histogram[i] > 0) {
            Node *n_node = node_create((uint8_t) i, histogram[i]);
            enqueue(pq, n_node);
        }
    }

    // running the Huffman Coding algorithm
    // using a while loop if the size of the pq is not 1
    while (!pq_size_is_1(pq)) {
        // dequeue left
        Node *left = dequeue(pq);
        // dequeue right
        Node *right = dequeue(pq);
        // create a new node:
        // weight = left->weight + right->weight
        // symbol = 0
        Node *n_node = node_create(0, left->weight + right->weight);
        // assigning the left and right of the new node
        n_node->left = left;
        n_node->right = right;
        // enqueue new node
        enqueue(pq, n_node);
        (*num_leaves)++;
    }
    // dequeue the queue's only entry and return it
    Node *huffman_tree = dequeue(pq);
    // free priority queue
    pq_free(&pq);
    // incrementing the number of leaves
    (*num_leaves)++;
    // returning the tree
    return huffman_tree;
}

// function that fills the code table
void fill_code_table(Code *code_table, Node *node, uint64_t code, uint8_t code_length) {
    // if node is null
    if (node == NULL) {
        // the function returns
        return;
    }
    // if the left and right are NULL
    if (node->left == NULL && node->right == NULL) {
        // store the Huffman Code for left node
        code_table[node->symbol].code = code;
        code_table[node->symbol].code_length = code_length;
        // return
        return;
    }
    // recursive calls for left and right nodes
    fill_code_table(code_table, node->left, code, code_length + 1);
    // appending 1 to code and recurse
    code |= (uint64_t) 1 << code_length;
    fill_code_table(code_table, node->right, code, code_length + 1);
}

// function that prints the Huffman Code
void huff_write_tree(BitWriter *outbuf, Node *node) {
    if (node->left == NULL) {
        // writing the leaf node
        bit_write_bit(outbuf, 1);
        bit_write_uint8(outbuf, node->symbol);
    } else {
        // writing the internal node
        // writing the left node
        huff_write_tree(outbuf, node->left);
        // writing the right node
        huff_write_tree(outbuf, node->right);
        // writing the out buffer
        bit_write_bit(outbuf, 0);
    }
}

// function that compresses the file
void huff_compress_file(BitWriter *outbuf, FILE *fin, uint32_t filesize, uint16_t num_leaves,
    Node *code_tree, Code *code_table) {
    // writing 'H' and 'C' as magic number
    bit_write_uint8(outbuf, 'H');
    bit_write_uint8(outbuf, 'C');
    // writing  filesize
    bit_write_uint32(outbuf, filesize);
    // writing number of leaves
    bit_write_uint16(outbuf, num_leaves);
    // writing Huffman Tree
    huff_write_tree(outbuf, code_tree);
    // rewind the input file to the beginning
    fseek(fin, 0, SEEK_SET);
    // initializing the input file
    int infile;
    // reading the input file and write Huffman codes
    while ((infile = fgetc(fin)) != EOF) {
        // retrieve the code for the read character from code_table
        uint64_t code = code_table[infile].code;
        // retrieve the code_length for the read character from code_table
        uint8_t code_length = code_table[infile].code_length;
        // a for loop that runs code_length times
        for (int i = 0; i < code_length; ++i) {
            // write the least significant bit of code to output
            bit_write_bit(outbuf, (code & 1));
            // right shift the bits of code by 1
            code >>= 1;
        } // end for loop
    } // end while loop
}

// funciton that prints the usage message
void print_help(void) {
    fprintf(stdout, "Usage: huff -i infile -o outfile\n"
                    "       huff -v -i infile -o outfile\n"
                    "       huff -h\n");
}

// the main function
int main(int argc, char **argv) {
    // defining option to use in getopt()
    int option;
    // defining a file to scan the input file
    FILE *fin = NULL;
    // defining a variable for the output file name
    BitWriter *outb;
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
        case 'i':
            // opening the file with r
            fin = fopen(optarg, "r");
            // check that finis not NULL
            if (fin == NULL) {
                // print error
                fprintf(stderr, "Error opening input file\n");
                // print help message
                print_help();
                // exiting the program
                return 1;
            }
            break;
        // if the option was 'o' print the output into this file
        case 'o':
            // open output file
            outb = bit_write_open(optarg);
            // if the output file is null
            if (outb == NULL) {
                // printing error to open the output file
                fprintf(stderr, "Error opening output file\n");
                // closing output file
                bit_write_close(&outb);
                // exiting the code
                return 1;
            }
            break;
            // the default case it to break
        default: return 1; break;
        } // end of switch
    } // end of while loop

    // creating a histogram
    uint32_t histogram[256];
    // initializing the file size and filling the histogram
    uint32_t filesize = fill_histogram(fin, histogram);
    // initializing the number of leaves
    uint16_t num_leaves = 0;
    // creating the tree
    Node *code_tree = create_tree(histogram, &num_leaves);
    // if the tree is NULL
    if (code_tree == NULL) {
        // print error and help message
        fprintf(stderr, "could not create the tree");
        print_help();
        // closing input file
        fclose(fin);
    }
    // allocating the table
    Code *code_table = malloc(256 * sizeof(Code));
    // filling the table
    fill_code_table(code_table, code_tree, 0, 0);
    // compressing the file and printing result to output file
    huff_compress_file(outb, fin, filesize, num_leaves, code_tree, code_table);
    // freeing the table
    free(code_table);
    // freeing the node
    node_free(&code_tree);
    // closing input file
    fclose(fin);
    // making input file null
    fin = NULL;
    // closing output file
    bit_write_close(&outb);
    return 0;
} // end of main
