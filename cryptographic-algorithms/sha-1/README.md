# SHA-1 Implementation in C

This repository contains the implementation of the SHA-1 algorithm in C. The program takes an input message and prints its corresponding 160-bit hash.

## Prerequisites

- `gcc` compiler
- Standard C libraries

## How to Run

1. **Set the Input Message**:  
   Initialize the input message you want to hash in the `char*` variable named `message` within the `main` function of the program.  
   By default, the program is set to hash the following example message:  
   `"The quick brown fox jumps over the lazy dog"`

2. **Compile the Program**:  
   Use the following command to compile the program:  
   ```bash
   gcc sha.c -o sha

3. **Run the generated executable**:
   ```bash
   ./sha
