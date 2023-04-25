#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
/*
int main(void) {

        uint64_t hist[ALPHABET] = {0};
        hist[96] = 10;
        hist[97] = 5;
        hist[98] = 11;

        Node *root = build_tree(hist);
        node_print(root);
        Code table[ALPHABET];
        for (int i = 0; i < ALPHABET; i++) {

                table[i] = code_init();
        }

        build_codes(root, table);
        delete_tree(&root);
        for (int i = 0; i < ALPHABET; i++) {
                if (table[i].top) {
                        printf("symbol: %c\n", i);
                        code_print(&table[i]);
                }
        }
        int outfile = open("result.txt", O_WRONLY);
        dump_tree(outfile, root);
        close(outfile);
        int infile = open("result.txt", O_RDWR);
        uint8_t buf[BLOCK];
        int readbyters = read_bytes(infile, buf, 50);
        printf("read bytes %i\n", readbyters);
        Node *rebuilt = rebuild_tree(readbyters, buf);
        node_print(rebuilt);
        node_print(rebuilt->left);
        node_print(rebuilt->right);
        delete_tree(&rebuilt);
        return 0;
}
*/

Node *build_tree(uint64_t hist[static ALPHABET]) {
  PriorityQueue *pq = pq_create(ALPHABET);
  for (int i = 0; i < ALPHABET; i++) {
    if (hist[i] != 0) {
      Node *node = node_create(i, hist[i]);
      enqueue(pq, node);
    }
  }
  Node *left;
  Node *right;
  Node *parent;
  while (pq_size(pq) > 1) {

    dequeue(pq, &left);
    dequeue(pq, &right);
    parent = node_join(left, right);
    enqueue(pq, parent);
  }
  Node *root;
  dequeue(pq, &root);
  pq_delete(&pq);
  return root;
}

Code c;
void build_codes(Node *root, Code table[static ALPHABET]) {
  if (!c.top) {
    c = code_init();
  }
  uint8_t bit;
  if (root) {
    if (!root->left && !root->right) {
      table[root->symbol] = c;
    } else {

      code_push_bit(&c, 0);
      build_codes(root->left, table);
      code_pop_bit(&c, &bit);

      code_push_bit(&c, 1);
      build_codes(root->right, table);
      code_pop_bit(&c, &bit);
    }
  }
}

void dump_tree(int outfile, Node *root) {

  if (root) {
    uint8_t buf[1];
    dump_tree(outfile, root->left);
    dump_tree(outfile, root->right);
    if (!root->left && !root->right) {
      buf[0] = 'L';
      write_bytes(outfile, buf, 1);
      buf[0] = root->symbol;
      write_bytes(outfile, buf, 1);
    } else {
      uint8_t interior[1] = {'I'};
      write_bytes(outfile, interior, 1);
    }
  }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {

  Stack *stack = stack_create(nbytes);
  Node *root;
  Node *left;
  Node *right;
  Node *parent;

  for (int i = 0; i < nbytes; i++) {
    if (tree[i] == 'L') {
      i += 1;
      Node *node = node_create(tree[i], 0);
      stack_push(stack, node);
    } else {
      stack_pop(stack, &right);
      stack_pop(stack, &left);
      parent = node_join(left, right);
      stack_push(stack, parent);
    }
  }
  stack_pop(stack, &root);
  stack_delete(&stack);
  return root;
}

void delete_tree(Node **root) {
  if (*root) {
    delete_tree(&(*root)->left);
    delete_tree(&(*root)->right);
    free(*root);
    *root = NULL;
  }
}
