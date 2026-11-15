
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fn(int **p)
{
  *p = malloc(sizeof(*p));
  // **p = malloc(sizeof(**p));
  // **p = 1234;
  printf("from fn: p   = %llu\n", p);
  printf("from fn: *p  = %llu\n", *p);
  // printf("from fn: **p = %llu\n", **p);
}

int main () {

  int *p = NULL;
  // fn(&p);

  printf("from main: p = %llu\n", p);
  printf("from main: &p  = %llu\n", &p);
  p = malloc(sizeof(*p));
  printf("from main: p = %llu\n", p);
  printf("from main: &p  = %llu\n", &p);
  printf("from main: *p = %llu\n", *p);
  *p = 1234;
  printf("from main: p = %llu\n", p);
  printf("from main: &p  = %llu\n", &p);
  printf("from main: *p = %llu\n", *p);
  // fn(&p);

  // printf("from main: p  = %llu\n", p);
  // printf("from main: *p = %llu\n", *p);
}