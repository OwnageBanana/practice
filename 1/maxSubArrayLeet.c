// LeetCode 53. Maximum Subarray
//
// Return the largest sum of any contiguous, non-empty subarray of nums.
// Example: [-2,1,-3,4,-1,2,1,-5,4]  ->  6, from [4,-1,2,1]
//
// numsSize is at least 1. Aim for O(n) time and O(1) extra memory.
// (Trying every start and end is O(n^2); get that working first if you like.)
//
// LeetCode's real signature is `int maxSubArray(int* nums, int numsSize)`.
// This version takes size_t for practice.

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int maxSubArray(const int *nums, size_t numsSize) {
  assert(numsSize > 0);
  const int *f = nums + 1;
  int max = nums[0];
  int sum = nums[0];
  while(f < nums + numsSize){
    if (sum <= 0) {
      sum = *f;
    } else {
      sum += *f;
    }
    if (sum > max) {
      max = sum;
    }
    f++;
  }
  return max;
}

// Copies `in` into a heap buffer of exactly n ints, so ASan reports any read
// past the end, then checks the returned sum.
static void check(const int *in, size_t n, int want) {
  int *buf = malloc(n * sizeof(int));
  assert(buf != NULL);
  memcpy(buf, in, n * sizeof(int));
  assert(maxSubArray(buf, n) == want);
  free(buf);
}

int main(void) {
  int a[] = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
  check(a, sizeof(a) / sizeof(a[0]), 6);

  int b[] = {5, 4, -1, 7, 8};         // best is the whole array
  check(b, sizeof(b) / sizeof(b[0]), 23);

  int c[] = {1};
  check(c, 1, 1);

  int d[] = {-3, -1, -2};             // all negative: the answer isn't 0
  check(d, sizeof(d) / sizeof(d[0]), -1);

  int e[] = {-5};                     // single negative
  check(e, 1, -5);

  int f[] = {2, -1, 2};               // worth carrying a dip
  check(f, sizeof(f) / sizeof(f[0]), 3);

  int g[] = {3, -10, 4};              // not worth carrying: restart
  check(g, sizeof(g) / sizeof(g[0]), 4);

  return 0;
}
