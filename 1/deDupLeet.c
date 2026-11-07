// LeetCode 26. Remove Duplicates from Sorted Array
//
// nums is sorted in non-decreasing order. Remove the duplicates in place so
// the first k elements hold the unique values in their original order, and
// return k. What's left after index k doesn't matter. Use O(1) extra memory.
//
// LeetCode's real signature is `int removeDuplicates(int* nums, int numsSize)`.
// This version takes size_t for practice.

#include <assert.h>
#include <stddef.h>
#include <string.h>

size_t removeDuplicates(int *nums, size_t numsSize) {
  // TODO: one pointer reads every element, one marks where the next unique goes
  if (numsSize <= 1) return numsSize;
  // int *read = nums + 1;
  int *write = nums;
  // while (read < nums + numsSize){
  for (size_t i = 1; i < numsSize; i++){
    if (nums[i] != *write) {
      write++;
      *write = nums[i];
    }
    // read++;
  }
  return (size_t)(write - nums) + 1;
}

// Copies `in` so the test data isn't modified, runs removeDuplicates, then
// checks both the returned k and the first k elements.
static void check(const int *in, size_t n, const int *want, size_t wantK) {
  int buf[64];
  memcpy(buf, in, n * sizeof(int));
  size_t k = removeDuplicates(buf, n);
  assert(k == wantK);
  assert(memcmp(buf, want, k * sizeof(int)) == 0);
}

int main(void) {
  // sizeof(arr) / sizeof(arr[0]) is the element count, valid only where
  // arr is still an array (not decayed to a pointer).
  int a[] = {1, 1, 2};
  int aw[] = {1, 2};
  check(a, sizeof(a) / sizeof(a[0]), aw, 2);

  int b[] = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
  int bw[] = {0, 1, 2, 3, 4};
  check(b, sizeof(b) / sizeof(b[0]), bw, 5);

  int c[] = {7, 7, 7, 7};             // all the same
  int cw[] = {7};
  check(c, sizeof(c) / sizeof(c[0]), cw, 1);

  int d[] = {-3, -1, 0, 5};           // no duplicates
  check(d, sizeof(d) / sizeof(d[0]), d, 4);

  int e[] = {42};                     // single element
  check(e, 1, e, 1);

  check(e, 0, e, 0);                  // empty: don't touch nums at all
  return 0;
}
