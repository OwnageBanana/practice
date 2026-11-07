// LeetCode 189. Rotate Array
//
// Rotate nums to the right by k steps, in place. k can be larger than
// numsSize. Example: [1,2,3,4,5,6,7], k = 3  ->  [5,6,7,1,2,3,4]
//
// Aim for O(n) time and O(1) extra memory. (A second array of size n is
// the easy version; try that first if you like, then do it without one.)
//
// LeetCode's real signature is `void rotate(int* nums, int numsSize, int k)`.
// This version takes size_t for practice.

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void reverse(int *nums, size_t numsSize) {
  if (numsSize <= 1) return;
  int *s = nums;
  int *f = nums + numsSize - 1;
  while (s < f) {
    int i = *s;
    *s = *f;
    *f = i;
    // *s += *f;
    // *f = *s - *f;
    // *s = *s - *f;

    s++; f--;
  }
}

void rotate(int *nums, size_t numsSize, size_t k) {
  if (numsSize <= 1) return;
  k = k % numsSize;
  if (k == 0) return;
  reverse(nums, numsSize);
  reverse(nums, k);
  reverse(nums + k, numsSize - k);

  int *p = nums;
  while (p < nums+numsSize){
    printf("%d, ", *p);
    p++;
  }
    printf("\n");
  // TODO
}

// Copies `in` into a heap buffer of exactly n ints, so ASan reports any read
// or write past the end. The + 1 keeps malloc(0) from returning NULL, which
// memcpy may not be given even with a size of 0.
static void check(const int *in, size_t n, size_t k, const int *want) {
  int *buf = malloc(n * sizeof(int) + 1);
  assert(buf != NULL);
  memcpy(buf, in, n * sizeof(int));
  rotate(buf, n, k);
  assert(memcmp(buf, want, n * sizeof(int)) == 0);
  free(buf);
}

int main(void) {
  int a[] = {1, 2, 3, 4, 5, 6, 7};
  int aw[] = {5, 6, 7, 1, 2, 3, 4};
  check(a, sizeof(a) / sizeof(a[0]), 3, aw);

  int b[] = {-1, -100, 3, 99};
  int bw[] = {3, 99, -1, -100};
  check(b, sizeof(b) / sizeof(b[0]), 2, bw);

  int c[] = {1, 2, 3};
  check(c, 3, 0, c);                  // k = 0: unchanged
  check(c, 3, 3, c);                  // k = n: full circle, unchanged

  int cw[] = {3, 1, 2};
  check(c, 3, 10, cw);                // k > n: 10 steps is the same as 1

  int d[] = {9};
  check(d, 1, 5, d);                  // single element

  check(d, 0, 5, d);                  // empty with k > 0: watch for k % 0
  return 0;
}
