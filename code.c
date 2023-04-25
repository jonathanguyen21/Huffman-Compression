#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "code.h"
#include "defines.h"

/*
int main(void) {

        Code c = code_init();
        printf("top: %u\n", c.top);
        printf("first bit: %hhu\n", c.bits[0]);

        printf("is empty: %s\n", code_empty(&c) ? "true" : "false");
        printf("is full: %s\n", code_full(&c) ? "true" : "false");

        uint8_t bit;
        code_set_bit(&c, 255);
        code_clr_bit(&c, 7);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_pop_bit(&c, &bit);

        printf("popped bit value: %hhu\n", bit);
        printf("size: %u\n", code_size(&c));

        code_print(&c);
        return 0;
}
*/

Code code_init(void) {
  Code c;
  c.top = 0;
  for (uint8_t i = 0; i < MAX_CODE_SIZE; i++) {
    c.bits[i] = 0;
  }
  return c;
}

uint32_t code_size(Code *c) { return c->top; }

bool code_empty(Code *c) {

  if (!c->bits[0] && c->top == 0) {
    return true;
  }
  return false;
}

bool code_full(Code *c) {
  if (c->top == 256) {
    return true;
  }
  return false;
}

bool code_set_bit(Code *c, uint32_t i) {
  uint8_t index = i / 8;
  if (i > 255) {
    return false;
  }
  c->bits[index] = c->bits[index] | (1U << (i % 8));
  return true;
}

bool code_clr_bit(Code *c, uint32_t i) {

  uint8_t index = i / 8;
  if (i > 255) {
    return false;
  }
  c->bits[index] = c->bits[index] & ~(1U << (i % 8));
  return true;
}

bool code_get_bit(Code *c, uint32_t i) {

  uint8_t index = i / 8;
  if (i > 255) {
    return false;
  }
  if (c->bits[index] & (1U << (i % 8))) {
    return true;
  } else {
    return false;
  }
}

bool code_push_bit(Code *c, uint8_t bit) {

  if (code_full(c)) {
    return false;
  }
  if (code_empty(c)) {
    if (bit > 0) {
      code_set_bit(c, c->top);
    }
    if (bit == 0) {
      code_clr_bit(c, c->top);
    }
    c->top += 1;
    return true;
  }
  if (bit > 0) {
    code_set_bit(c, c->top);
  }
  if (bit == 0) {
    code_clr_bit(c, c->top);
  }
  c->top += 1;
  return true;
}

bool code_pop_bit(Code *c, uint8_t *bit) {

  if (code_empty(c)) {
    return false;
  } else {
    if (code_get_bit(c, c->top - 1)) {
      *bit = 1;
    } else {
      *bit = 0;
    }
    code_clr_bit(c, c->top - 1);
    c->top -= 1;
    return true;
  }
}

void code_print(Code *c) {

  printf("Code top: %u\n", c->top);
  printf("Code bits:\n");

  for (uint32_t i = 0; i < c->top; i++) {
    if (i % 8 == 0 && i != 0) {
      printf("\n");
    }
    printf(" %u", code_get_bit(c, i));
  }
  printf("\n");
}
