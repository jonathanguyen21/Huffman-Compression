#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "code.h"
#include "defines.h"
#include "io.h"

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;
static uint8_t buffer[BLOCK];
static uint32_t index = 0;

/*
int main(void) {

        // TEST READ_BYTES
        int infile = open("test.txt", O_RDONLY);
        uint8_t buf[BLOCK];

        int bytes_read = read_bytes(infile, buf, 50);

        printf("BYTES READ: %i\n", bytes_read);
        printf("BUF: ");
        for (int i = 0; i < bytes_read; i++) {
                printf("%c", buf[i]);
        }
        printf("\n");


        // TEST WRITE_BYTES
        int outfile = open("result.txt", O_WRONLY);

        int bytes_written = write_bytes(outfile, buf, bytes_read);
        printf("BYTES WRITTEN: %i\n", bytes_written);
        printf("\n");

        // TEST READ_BIT
        int infile = open("test.txt", O_RDONLY);
        uint8_t bit;
        printf("BITS READ:\n");
        int position = 0;
        int count = 1;
        while (true) {
                bool condition = read_bit(infile, &bit);
                printf(" %i", bit);
                if (!condition) {
                        break;
                }
                position++;
                if (position % 8 == 0) {
                        printf("\n");
                        count++;
                }
        }
        printf("\nBYTES READ = %i\n", count);


        //TEST WRITE CODE AND FLUSH
        Code c = code_init();
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);

        Code a = code_init();
        code_push_bit(&a, 0);
        code_push_bit(&a, 1);
        code_push_bit(&a, 0);
        code_push_bit(&a, 0);
        //code_push_bit(&a, 0);
        //code_push_bit(&a, 1);
        //code_push_bit(&a, 0);
        //code_push_bit(&a, 0);

        Code b = code_init();
        code_push_bit(&b, 0);
        code_push_bit(&b, 1);
        code_push_bit(&b, 0);
        code_push_bit(&b, 1);
        code_push_bit(&b, 0);
        code_push_bit(&b, 0);
        code_push_bit(&b, 0);
        code_push_bit(&b, 0);

        int outfile = open("result.txt", O_WRONLY);
        //code_print(&c);

        for(int i = 0; i < 4096; i++) {
                write_code(outfile, &c);
        }
        code_print(&c);

        write_code(outfile, &b);
        for(int i = 0; i < 7; i++) {
                write_code(outfile, &a);
        }
        code_print(&a);
        flush_codes(outfile);
        return 0;
}
*/
// read certain number of bytes from file and store in buffer
int read_bytes(int infile, uint8_t *buf, int nbytes) {

  int count = 0;
  for (int i = 0; i < nbytes; i++) {
    int bytesread = read(infile, &buf[i], 1);
    count += bytesread;
    if (bytesread ==
        0) { // read until the current bytes read is 0 or nbytes is fulfilled
      break;
    }
  }
  bytes_read += count;
  return count;
}

// write certain number of bytes from buffer to a file
int write_bytes(int outfile, uint8_t *buf, int nbytes) {

  int count = 0;
  for (int i = 0; i < nbytes; i++) {
    int byteswritten = write(outfile, &buf[i], 1);
    count += byteswritten;
    if (byteswritten <= 0) {
      break;
    }
  }
  bytes_written += count;
  return count;
}

static uint64_t bytes_read1 = 0;
static uint64_t position = 0;
static uint8_t buf[BLOCK] = {0};
static uint64_t calls = 0;
bool read_bit(int infile, uint8_t *bit) {
  if (calls == 0) {
    bytes_read1 = read_bytes(infile, buf, BLOCK);
  }
  calls += 1;

  uint64_t index = position / 8;
  if (buf[index] & (1U << (position % 8))) {
    *bit = 1;
  } else {
    *bit = 0;
  }
  position += 1;
  if (position != (bytes_read1 * 8)) {
    return true;
  } else {
    if (position != 32768) {
      return false;
    }
    bytes_read1 = read_bytes(infile, buf, BLOCK);
    position = 0;
    if (bytes_read1 == 0) {
      return false;
    }
    return true;
  }
}

void write_code(int outfile, Code *c) {
  for (uint32_t i = 0; i < c->top; i++) {
    int bufindex = index / 8;
    if (code_get_bit(c, i)) { // set bit to 1 in buffer
      buffer[bufindex] = buffer[bufindex] | (1U << (index % 8));
    } else {
      buffer[bufindex] = buffer[bufindex] & ~(1U << (index % 8));
    }
    index++;
    if (index == 32768) {
      write_bytes(outfile, buffer, BLOCK);
      index = 0;
    }
  }
}

void flush_codes(int outfile) {
  if (index % 8 == 0) {
    write_bytes(outfile, buffer, index / 8);
    return;
  }
  for (; index % 8 != 0; index++) {
    int bufindex = index / 8;
    buffer[bufindex] = buffer[bufindex] & ~(1U << (index % 8));
  }
  write_bytes(outfile, buffer, index / 8);
}
