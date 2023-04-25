#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "node.h"

/*
int main(void) {

        Node *left = node_create('z', 2);
        Node *right = node_create('a',3);
        Node *parent = node_join(left, right);

        printf("compare left and right: %s\n", node_cmp(left, right) ? "true" :
"false"); printf("symbol: %hhu\n", parent->symbol); node_print(left);

        node_print_sym(right);
        node_delete(&left);
        node_delete(&right);
        node_delete(&parent);

        return 0;
}
*/

// creates a node and allocates memory for it
Node *node_create(uint8_t symbol, uint64_t frequency) {

  Node *node = (Node *)calloc(1, sizeof(Node));
  if (node) {
    node->symbol = symbol;
    node->frequency = frequency;
  }
  return node;
}

// frees a node and sets its pointer to null
void node_delete(Node **n) {

  if (*n) {
    free(*n);
    *n = NULL;
  }
}

// joins two nodes with a parent
Node *node_join(Node *left, Node *right) {
  Node *parent = node_create('$', left->frequency + right->frequency);
  parent->left = left;
  parent->right = right;
  return parent;
}

// prints nodes symbol and frequency
void node_print(Node *n) {
  int left = 0;
  int right = 0;
  printf("symbol: %c\n", n->symbol);
  printf("frequency: %lu\n", n->frequency);
  if (n->left) {
    left = 1;
    printf("left child: %c\n", n->left->symbol);
  } else {
    printf("no left child\n");
  }
  if (n->right) {
    right = 1;
    printf("right child: %c\n", n->right->symbol);
  } else {
    printf("no right child\n");
  }
  printf("\n");
  if (left == 1) {
    node_print(n->left);
  }
  if (right == 1) {
    node_print(n->right);
  }
}

// compares the frequency of two nodes
bool node_cmp(Node *n, Node *m) {
  if (n->frequency > m->frequency) {
    return true;
  } else {
    return false;
  }
}

void node_print_sym(Node *n) { printf("symbol: %c\n", n->symbol); }
