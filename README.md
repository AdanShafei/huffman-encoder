# Huffman Data Compression (Encoder & Decoder in C)

Lossless text compression and decompression using **Huffman coding**.  
This implementation builds a frequency table, constructs a Huffman tree, encodes input bytes to variable-length codes, and writes a compact bitstream.  
The decoder reconstructs the tree from the header and recovers the original file bit-by-bit.

> This repo contains the cleaned/organized version of my original assignment implementation.

---

## 🔍 Overview

**Encoder (`huff`)**
1. Scan input to build a **frequency table** for all byte values (0–255).
2. Push singleton nodes into a **priority queue** ordered by frequency (then symbol as tiebreaker).
3. Repeatedly dequeue the two smallest trees and **merge** them into a parent node until one root remains.
4. **Assign codes** by walking the tree (left = 0, right = 1) and record each symbol’s code + codelen.
5. **Write file header** (magic bytes, optional original size, and a serialized tree or code table).
6. **Encode** input: write each symbol’s code bit-by-bit to the output stream using a bit-writer.

**Decoder (`dehuff`)**
1. Read header, **rebuild the codebook/tree**.
2. Read the bitstream and **traverse the tree** until a leaf is hit → emit the symbol.
3. Repeat until the original number of symbols is reconstructed (or EOF/tree end marker reached).

---

## 🧠 Why Huffman Works (Short)

Huffman coding assigns **shorter codes to frequent symbols** and **longer codes to rare symbols**, minimizing expected code length.  
This yields compression on natural-language inputs and many structured text files.

---

## 🧰 Tech Stack & Concepts
- **Language:** C (C11)
- **Data structures:** binary tree (Huffman tree), linked-list priority queue
- **Bit I/O:** custom bit-writer/bit-reader to pack/unpack variable-length codes
- **Build:** `Makefile`

---

## 📂 Project Structure

<pre><code>.
├── include/
│   ├── bitreader.h
│   ├── bitwriter.h
|   ├── Makefile
│   ├── node.h
│   └── pq.h
├── src/
│   ├── bitreader.c
│   ├── bitwriter.c
│   ├── node.c
│   ├── pq.c
│   ├── huff.c       # encoder main
│   └── dehuff.c     # decoder main
├── tests/
│   ├── brtest.c
│   ├── bwtest.c
│   ├── nodetest.c
│   └── pqtest.c
├── report.pdf
└── README.md
</code></pre>
