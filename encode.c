#define _GNU_SOURCE

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"

#define OPTIONS "hi:o:v"

union {
  uint8_t b[16];
  Header s;
} hh;

int main(int argc, char **argv) {

  int opt = 0;
  // variables for files set or not
  int infile = 0;  // default stdin
  int outfile = 1; // default stdout
  int stats_enabled = 0;

  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {

    case 'h':
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  %s [-h] [-i infile] [-o outfile]\n", argv[0]);
      fprintf(stderr, "\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 0;
    case 'i':
      infile = open(optarg, O_RDONLY);
      if (infile == -1) {
        return 1;
      }
      break;
    case 'o':
      outfile = open(optarg, O_WRONLY | O_CREAT);
      if (outfile == -1) {
        return 1;
      }
      break;
    case 'v':
      stats_enabled = 1;
      break;
    default:
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  %s [-h] [-i infile] [-o outfile]\n", argv[0]);
      fprintf(stderr, "\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 1;
    }
  }

  uint64_t hist[ALPHABET] = {0};
  uint8_t buf[BLOCK];
  int stdinput;

  // READING IN SYMBOLS FROM INFILE
  int bytes_read_now = read_bytes(infile, buf, BLOCK);
  if (infile == 0) {
    stdinput = open("/tmp", O_TMPFILE | O_RDWR);
  }
  for (; bytes_read_now != 0; bytes_read_now = read_bytes(infile, buf, BLOCK)) {
    if (infile == 0) {
      bytes_written -= write_bytes(stdinput, buf, bytes_read_now);
    }
    for (int i = 0; i < bytes_read_now; i++) {
      hist[buf[i]] += 1;
    }
  }

  uint16_t unique_symbols = 0;
  for (int i = 0; i < ALPHABET; i++) {
    if (hist[i] != 0) {
      unique_symbols += 1;
    }
  }
  if (hist[0] == 0) {
    hist[0] = 1;
    unique_symbols += 1;
  }
  if (hist[1] == 0) {
    hist[1] = 1;
    unique_symbols += 1;
  }

  Node *root = build_tree(hist);
  Code table[ALPHABET];
  // intialize all the codes in the table
  for (int i = 0; i < ALPHABET; i++) {
    table[i] = code_init();
  }

  build_codes(root, table);

  struct stat stats;
  if (infile != 0) {
    fstat(infile, &stats);
  } else if (infile == 0) {
    struct stat temp;
    fstat(infile, &temp);
    fchmod(stdinput, temp.st_mode);
    fstat(stdinput, &stats);
  }
  fchmod(outfile, stats.st_mode);
  Header header;
  header.magic = MAGIC;
  header.permissions = stats.st_mode;
  header.tree_size = (3 * unique_symbols) - 1;
  header.file_size = stats.st_size;

  // NEED TO WRITE OUT HEADER SOMEHOW
  hh.s = header;
  write_bytes(outfile, hh.b, 16);

  dump_tree(outfile, root); // ISSUE MIGHT BE RIGHT HERE

  if (infile != 0) {
    lseek(infile, 0, SEEK_SET); // go back to beginning of infile

    // WRITE OUT CODE OF EACH SYMBOL TO OUTFILE
    bytes_read_now = read_bytes(infile, buf, BLOCK);
    for (; bytes_read_now != 0;
         bytes_read_now = read_bytes(infile, buf, BLOCK)) {
      for (int i = 0; i < bytes_read_now; i++) {
        write_code(outfile, &table[buf[i]]);
      }
    }
  } else if (infile == 0) {

    lseek(stdinput, 0, SEEK_SET);
    // WRITE OUT CODE OF EACH SYMBOL TO OUTFILE
    bytes_read_now = read_bytes(stdinput, buf, BLOCK);
    for (; bytes_read_now != 0;
         bytes_read_now = read_bytes(stdinput, buf, BLOCK)) {
      for (int i = 0; i < bytes_read_now; i++) {
        write_code(outfile, &table[buf[i]]);
      }
    }
  }
  flush_codes(outfile);

  if (stats_enabled == 1) {
    fprintf(stderr, "Uncompressed file size: %lu bytes\n", header.file_size);
    fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
    fprintf(stderr, "Space saving: %0.2lf%%\n",
            100 * (1 - ((double)bytes_written / header.file_size)));
  }

  delete_tree(&root);
  if (infile == 0) {
    close(stdinput);
  }
  close(infile);
  close(outfile);
  return 0;
}
