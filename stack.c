#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "node.h"
#include "stack.h"

struct Stack {
  uint32_t top;
  uint32_t capacity;
  Node **items;
};

/*
int main(void) {

        Stack *stack = stack_create(9);
        printf("capacity: %u\n", stack->capacity);
        printf("is empty: %s\n", stack_empty(stack) ? "true" : "false");

        Node *one = node_create('a',3);
        Node *two = node_create('z',5);

        stack_push(stack, one);
        stack_push(stack, two);
        stack_print(stack);
        stack_pop(stack, &two);
        printf("is empty: %s\n", stack_empty(stack) ? "true" : "false");
        node_print(stack->items[stack->top]);

        stack_delete(&stack);
        return 0;
}
*/

// constructor for stack allocates memory for stack and array of nodes
Stack *stack_create(uint32_t capacity) {

  Stack *stack = (Stack *)calloc(1, sizeof(Stack));
  if (stack) {
    stack->capacity = capacity;
    void *mem_p = calloc(capacity, sizeof(Node));
    stack->items = (Node **)mem_p;
  }
  return stack;
}

// frees mem in array of nodes and frees stack
void stack_delete(Stack **s) {

  if (*s) {
    free((*s)->items);
    free(*s);
    *s = NULL;
  }
}

// checks if stack is empty
bool stack_empty(Stack *s) {

  if (!s->items[0]) {
    return true;
  }
  return false;
}

// checks if stack is full
bool stack_full(Stack *s) {

  if (s->top == s->capacity - 1) {
    return true;
  }
  return false;
}

// returns stack size
uint32_t stack_size(Stack *s) { return s->top + 1; }

// adds a node to the stack
bool stack_push(Stack *s, Node *n) {

  if (stack_full(s)) {
    return false;
  }
  if (stack_empty(s)) {
    s->top = 0;
    s->items[s->top] = n;
    return true;
  } else {
    s->top += 1;
    s->items[s->top] = n;
    return true;
  }
}

// removes a node from the stack
bool stack_pop(Stack *s, Node **n) {

  if (stack_empty(s)) {
    return false;
  } else {
    *n = s->items[s->top];
    s->items[s->top] = NULL;
    s->top -= 1;
    return true;
  }
}

// prints out a stack
void stack_print(Stack *s) {

  printf("current top: %u\n", s->top);
  printf("capacity: %u\n\n", s->capacity);
  for (uint32_t i = 0; i < s->top + 1; i++) {
    node_print(s->items[i]);
    printf("\n");
  }
}
