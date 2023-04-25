# How to build and run encode.c
Contains implementation of the Huffman encoder
1. Open terminal
2. Go into the same directory as encode.c
3. Compile using Makefile by entering "make encode" into the terminal
4. Run program with "./encode [OPTIONS]..."

# Command-line options accepted in encode.c
- -h: displays program synopsis and usage.
- -i _infile_: Specifies the input file to encode using Huffman coding. The default input should be set as stdin.
- -o _outfile_: Specifies the output file to write the compressed input to. The default output should be set as stdout.
- -v: Prints compression statistics to stderr.

# How to build and run decode.c
Contains implementation of the Huffman decoder
1. Open terminal
2. Go into the same directory as decode.c
3. Compile using Makefile by entering "make decode" into the terminal
4. Run program with "./decode [OPTIONS]..."

# Command-line options accepted in encode.c
- -h: displays program synopsis and usage.
- -i _infile_: Specifies the input file to decode using Huffman coding. The default input should be set as stdin.
- -o _outfile_: Specifies the output file to write the decompressed input to. The default output should be set as stdout.
- -v: Prints decompression statistics to stderr.

# node.c
Contains implementation of the node ADT

# pq.c
Contains implementation of the priority queue ADT

# code.c
Contains implementation of code ADT

# io.c
Contains implementation of the I/O module

# stack.c
Contains implementation of the stack ADT

# huffman.c
Contains implementation of the Huffman coding module interface

# scan-build false positives
- my program has no bugs and false positives when running scan-build

# Sources
- David Wu: talked with him to get a better understanding of min heaps
