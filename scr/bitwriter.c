#include "bitwriter.h"

#include <stdio.h>
#include <stdlib.h>

struct BitWriter {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};

// all the functions in this file are written based on the sudo code given in asgn8.pdf

// function that opens binary file for write using fopen() and return a pointer
BitWriter *bit_write_open(const char *filename) {
    // allocate a new BitWriter
    BitWriter *writer = calloc(1, sizeof(BitWriter));
    // open the filename for writing as a binary file
    FILE *f = fopen(filename, "w");
    if (f == NULL || writer == NULL) {
        return NULL;
    }
    // store f in the BitWriter field underlying_stream
    writer->underlying_stream = f;
    // clear the byte and bit_positions fields of the BitWriter to 0
    writer->byte = 0;
    writer->bit_position = 0;
    return writer;
}

// function that closes binary file for write using fclose()
void bit_write_close(BitWriter **pbuf) {
    if (*pbuf != NULL) {
        if ((*pbuf)->bit_position > 0) {
            // flush any remaining bits in the byte buffer to the underlying_stream
            fputc((*pbuf)->byte, (*pbuf)->underlying_stream);
        }
        // close the underlying_stream
        fclose((*pbuf)->underlying_stream);
        // free the BitWriter
        free(*pbuf);
        // set the *pbuf pointer to NULL
        *pbuf = NULL;
    }
}

// function that writes a single bit
void bit_write_bit(BitWriter *buf, uint8_t x) {
    if (buf->bit_position > 7) {
        // flush the byte buffer to the underlying stream
        if (fputc(buf->byte, buf->underlying_stream) == EOF) {
            // handle error: Could not write byte to underlying stream
            fprintf(stderr, "Error reading from stream.\n");
        }
        // clear the byte buffer and reset the bit position
        buf->byte = 0;
        buf->bit_position = 0;
    }
    // set the bit_position bit of the byte to x
    buf->byte |= (x & 1) << buf->bit_position;
    // increment the bit position
    buf->bit_position++;
}

// funciton that writes 8 bits at a time
void bit_write_uint8(BitWriter *buf, uint8_t x) {
    // 0 to 7 means less than 8
    for (int i = 0; i < 8; i++) {
        // check if the LSB of x is 1
        bit_write_bit(buf, x & 1);
        // shift x right by 1 bit, effectively discarding the LSB of x
        x >>= 1;
    }
}

// funciton that writes 16 bits at a time
void bit_write_uint16(BitWriter *buf, uint16_t x) {
    // 0 to 15 means less than 16
    for (int i = 0; i < 16; i++) {
        // check if the LSB of x is 1
        bit_write_bit(buf, x & 1);
        // shift x right by 1 bit, effectively discarding the LSB of x
        x >>= 1;
    }
}

// function that writes 32 bits at a time
void bit_write_uint32(BitWriter *buf, uint32_t x) {
    // 0 to 31 means less than 32
    for (int i = 0; i < 32; i++) {
        // check if the LSB of x is 1
        bit_write_bit(buf, x & 1);
        // shift x right by 1 bit, effectively discarding the LSB of x
        x >>= 1;
    }
}
