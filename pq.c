#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "node.h"
#include "pq.h"

int l_child(int n);
int r_child(int n);
int parent(int n);
void up_heap(Node **a, int n);
void down_heap(Node **a, int heap_size);

struct PriorityQueue {

  uint32_t capacity;
  uint32_t n_elements;
  Node **items;
};

/*
int main(void) {

        PriorityQueue *pq = pq_create(3);
        printf("capacity: %u\n", pq->capacity);
        printf("is empty: %s\n", pq_empty(pq) ? "true" : "false");
        printf("is full: %s\n", pq_full(pq) ? "true" : "false");

        Node *a = node_create('a', 10);
        Node *b = node_create('b', 10);
        Node *c = node_create('c', 10);
        enqueue(pq, a);
        enqueue(pq, b);
        enqueue(pq, c);
        pq_print(pq);
        printf("is full: %s\n", pq_full(pq) ? "true" : "false");
        Node *n;
        dequeue(pq, &n);
        pq_print(pq);
        dequeue(pq, &n);
        pq_print(pq);
        dequeue(pq, &n);

        printf("is empty: %s\n", pq_empty(pq) ? "true" : "false");

        pq_print(pq);

        node_delete(&a);
        node_delete(&b);
        node_delete(&c);
        pq_delete(&pq);
        return 0;
}
*/

PriorityQueue *pq_create(uint32_t capacity) {

  PriorityQueue *pq = (PriorityQueue *)calloc(1, sizeof(PriorityQueue));
  pq->capacity = capacity;
  void *mem_p = calloc(capacity, sizeof(Node));
  pq->items = (Node **)mem_p;

  return pq;
}

void pq_delete(PriorityQueue **q) {

  if (*q) {
    free((*q)->items);
    free(*q);
    *q = NULL;
  }
}

bool pq_empty(PriorityQueue *q) {
  if (q->n_elements == 0) {
    return true;
  }
  return false;
}

bool pq_full(PriorityQueue *q) {
  if (q->n_elements == q->capacity) {
    return true;
  }
  return false;
}

uint32_t pq_size(PriorityQueue *q) { return q->n_elements; }

bool enqueue(PriorityQueue *q, Node *n) {

  if (pq_full(q)) {
    return false;
  }

  q->items[q->n_elements] = n;
  q->n_elements += 1;

  up_heap(q->items, q->n_elements - 1);
  return true;
}

bool dequeue(PriorityQueue *q, Node **n) {

  if (pq_empty(q)) {
    return false;
  }
  *n = q->items[0];
  q->n_elements -= 1;
  q->items[0] = q->items[q->n_elements];
  down_heap(q->items, q->n_elements);
  return true;
}

void pq_print(PriorityQueue *q) {

  printf("\nn_elements: %u\n", q->n_elements);
  for (uint32_t i = 0; i < q->n_elements; i++) {
    node_print(q->items[i]);
    printf("\n");
  }
}

int l_child(int n) { return 2 * n + 1; }

int r_child(int n) { return 2 * n + 2; }

int parent(int n) { return (n - 1) / 2; }

void up_heap(Node **a, int n) {
  while (n > 0 && a[n]->frequency < a[parent(n)]->frequency) {
    Node *temp = a[n];
    a[n] = a[parent(n)];
    a[parent(n)] = temp;
    n = parent(n);
  }
}

void down_heap(Node **a, int heap_size) {
  int n = 0;
  int smaller;
  while (l_child(n) < heap_size) {
    if (r_child(n) == heap_size) {
      smaller = l_child(n);
    } else if (a[r_child(n)]->frequency > a[l_child(n)]->frequency) {
      smaller = l_child(n);
    } else {
      smaller = r_child(n);
    }
    if (a[n]->frequency < a[smaller]->frequency) {
      break;
    }
    Node *temp = a[n];
    a[n] = a[smaller];
    a[smaller] = temp;
    n = smaller;
  }
}
