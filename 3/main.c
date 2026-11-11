
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main () {
  char *hw = "hello world";

  size_t t1 = strlen(hw); // 11
  char *p1 = malloc(t1);  //malloc 11 bytes
  p1 = memcpy(p1, hw, t1); // memcpy copies hello world

  size_t t2 = strlen(hw) + 1; // 12 feels more correct, it includes the \0
  char *p2 = malloc(t2);  //malloc 12 bytes
  p2 = memcpy(p2, hw, t2); // memcpy copies hello world\0

  printf("strcmp : %d \n", strcmp(p1, p2)); // prints 0. strings are equal. Is this just luck because the following bytes of f1 are 0 ?

  // testing when following bytes are not 0
  char buf[64];
  printf("%d \n", sizeof(buf));
  for(size_t i = 0; i < sizeof(buf)-1; i++) {
    buf[i] = 'a';
  }

  p1 = memcpy(buf, hw, t1); // memcpy copies hello world
  p2 = memcpy(buf+31, hw, t2); // memcpy copies hello world\0

  printf("p1: %s \n", p1); // p1: hello worldaaaaaaaaaaaaaaaaaaaahello world
  printf("p2: %s \n", p2); // p2: hello world
  printf("strcmp from buf: %d \n", strcmp(p1, p2)); // strcmp from buf: 97


  // looks as though method 2 was correct to begin with. I _knew_ this but also I was seeing weird behaviour so better to validate.
}