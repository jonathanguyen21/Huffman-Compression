SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:%.c=%.o)
CC	 = clang
CFLAGS 	 = -Wall -Wpedantic -Werror -Wextra -Ofast

.PHONY: all clean

all: encode decode

encode: encode.o code.o huffman.o io.o node.o pq.o stack.o
	$(CC) -o $@ $^

decode: decode.o code.o huffman.o io.o node.o pq.o stack.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS)

spotless:
	rm -f decode encode $(OBJECTS)

format:
	clang-format -i -style=file *.[ch]
