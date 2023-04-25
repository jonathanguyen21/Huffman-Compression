#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
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
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  %s [-h] [-i infile] [-o outfile]\n", argv[0]);
      fprintf(stderr, "\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
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
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  %s [-h] [-i infile] [-o outfile]\n", argv[0]);
      fprintf(stderr, "\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");

      return 1;
    }
  }

  read_bytes(infile, hh.b, 16);

  // making sure magic number is the same
  if (hh.s.magic != MAGIC) {
    fprintf(stderr, "Error: unable to read header.\n");
    return 1;
  }

  // setting outfile permissions to infile permissions
  fchmod(outfile, hh.s.permissions);

  uint8_t tree_dump[hh.s.tree_size];
  read_bytes(infile, tree_dump, hh.s.tree_size);

  // rebuilding tree
  Node *root = rebuild_tree(hh.s.tree_size, tree_dump);
  // reading infile bit by bit
  uint8_t bit;
  uint8_t buf[1];
  Node *current = root;
  uint32_t decoded_symbols = 0;
  while (decoded_symbols != hh.s.file_size) {
    read_bit(infile, &bit);
    if (bit == 0) {
      current = current->left;
    }
    if (bit == 1) {
      current = current->right;
    }
    if (!current->left && !current->right) {
      decoded_symbols += 1;
      buf[0] = current->symbol;
      write_bytes(outfile, buf, 1);
      current = root;
    }
  }
  // write_bytes(outfile, buf, block_position);

  if (stats_enabled == 1) {
    fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
    fprintf(stderr, "Decompressed file size: %lu bytes\n", hh.s.file_size);
    fprintf(stderr, "Space saving: %0.2lf%%\n",
            100 * (1 - ((double)bytes_read / hh.s.file_size)));
  }

  delete_tree(&root);
  close(infile);
  close(outfile);
  return 0;
}
