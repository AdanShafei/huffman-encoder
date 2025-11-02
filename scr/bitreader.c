#include "bitreader.h"

#include <stdio.h>
#include <stdlib.h>

struct BitReader {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};

// all the functions in this file are written based on the sudo code given in asgn8.pdf

//function that opens the file and allocates memory to read from the file
BitReader *bit_read_open(const char *filename) {
    // allocate a new BitWriter
    BitReader *reader = calloc(1, sizeof(BitReader));
    if (reader == NULL) {
        return NULL;
    }
    // open the filename for writing as a binary file
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "error reading input file %s\n", filename);
        free(reader);
        return NULL;
    }
    // store f in the BitWriter field underlying_stream
    reader->underlying_stream = f;
    // clear the byte and bit_positions fields of the BitWriter to 0
    reader->byte = 0;
    reader->bit_position = 8;
    return reader;
}

// fucntion that closes the opened file and frees the memory used in opening
void bit_read_close(BitReader **pbuf) {
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

// function to read a bit from a bit reader
uint8_t bit_read_bit(BitReader *buf) {
    // if bit position exceeds 7 (end of byte), get the next byte
    if (buf->bit_position > 7) {
        int byte = fgetc(buf->underlying_stream);
        if (byte == EOF) {
            return 1;
        }
        buf->byte = (unsigned char) byte;
        buf->bit_position = 0;
    }
    // extract the current bit from the byte and increment bit position
    uint8_t bit = (buf->byte >> buf->bit_position) & 1;
    buf->bit_position += 1;
    return bit;
}

// function to read 8 bits from the BitReader
uint8_t bit_read_uint8(BitReader *buf) {
    // initialize byte as 0
    uint8_t byte = 0x00;
    // 0 to 7 means less than 8
    for (int i = 0; i < 8; i++) {
        // read a bit from the BitReader
        uint8_t b = bit_read_bit(buf);
        // set the bit at position i in the byte
        byte |= (b << i);
    }
    // eturn the byte variable
    return byte;
}

// read a 16-bit unsigned integer from a BitReader
uint16_t bit_read_uint16(BitReader *buf) {
    // initialize word as 0
    uint16_t word = 0x0000;
    // 0 to 15 means less than 16
    for (int i = 0; i < 16; i++) {
        // read a bit from the buffer
        uint16_t b = bit_read_bit(buf);
        // bitwise OR to append the bit to the word
        word |= (b << i);
    }
    // return the final 16-bit word
    return word;
}

// function to read 32 bits from buffer and convert to uint32_t
uint32_t bit_read_uint32(BitReader *buf) {
    // initialize word as 0
    uint32_t word = 0x00000000;
    // 0 to 31 means less than 32
    for (int i = 0; i < 32; i++) {
        // read a bit from buffer
        uint32_t b = bit_read_bit(buf);
        // shift the bit to the corresponding position and OR with word
        word |= (b << i);
    }
    // return the 8-bit unsigned integer
    return word;
}
