// LeetCode 283. Move Zeroes
//
// Move every 0 in nums to the end, in place, keeping the non-zero values in
// their original order. Use O(1) extra memory.
//
// LeetCode's real signature is `void moveZeroes(int* nums, int numsSize)`.
// This version takes size_t for practice.

#include <assert.h>
#include <stddef.h>
#include <string.h>

void moveZeroes(int *nums, size_t numsSize) {
  if (numsSize <= 1) return;
  char *a = nums;
  while (a < nums + numsSize && *a != 0 ){
    if (a >= nums + numsSize) return;
    a++;
  }
  int *b = a + 1;
  while (b < nums + numsSize) {
    *a = *b;
    *b = 0;
    while ( a < nums + numsSize && *a != 0 && *a != 0){
      a++;
    }
    do {
      b++;
    } while (b < nums + numsSize && *b == 0);
  }
}


// Copies `in` so the test data isn't modified, runs moveZeroes, then checks
// the whole array, since the trailing zeros matter here.
static void check(const int *in, size_t n, const int *want) {
  int buf[64];
  memcpy(buf, in, n * sizeof(int));
  moveZeroes(buf, n);
  assert(memcmp(buf, want, n * sizeof(int)) == 0);
}

int main(void) {
  int a[] = {0, 1, 0, 3, 12};
  int aw[] = {1, 3, 12, 0, 0};
  check(a, sizeof(a) / sizeof(a[0]), aw);

  int b[] = {0, 0, 0};                // all zeros
  check(b, sizeof(b) / sizeof(b[0]), b);

  int c[] = {4, -2, 7};               // no zeros
  check(c, sizeof(c) / sizeof(c[0]), c);

  int d[] = {5, 6, 0, 0};             // zeros already at the end
  check(d, sizeof(d) / sizeof(d[0]), d);

  int e[] = {0, 0, 9};                // zeros all at the front
  int ew[] = {9, 0, 0};
  check(e, sizeof(e) / sizeof(e[0]), ew);

  int f[] = {-1, 0, 0, -3, 0, 2};     // negatives, mixed
  int fw[] = {-1, -3, 2, 0, 0, 0};
  check(f, sizeof(f) / sizeof(f[0]), fw);

  int g[] = {0};                      // single zero
  check(g, 1, g);

  int h[] = {8};                      // single non-zero
  check(h, 1, h);

  check(h, 0, h);                     // empty: don't touch nums at all
  return 0;
}
