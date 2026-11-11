// gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -g -fsanitize=address,undefined -o twoSums.out twoSumLeet.c
// LeetCode 1. Two Sum
//
// Given an array of ints `nums` and an int `target`, return the indices of
// the two elements that add up to `target`. Each input has exactly one
// answer, and you can't use the same element twice. The indices can come
// back in either order.
// Example: nums [2, 7, 11, 15], target 9  ->  [0, 1]   (2 + 7 = 9)
//          nums [3, 2, 4],      target 6  ->  [1, 2]
//
// Checking every pair is O(n^2). Do it in one pass, O(n), with your hash map
// from hashMap.c, which this file includes.
//
// Your map has string keys, so turn each number into a key with snprintf.
// Size the key buffer for the longest int, "-2147483648", plus its '\0'.
//
// LeetCode's limits: 2 <= numsSize <= 10^4, and every nums[i] and target is
// between -10^9 and 10^9.
//
// Return convention (the same for every LeetCode problem that returns an
// array): malloc the result, write its length through `returnSize`, and the
// caller frees it. Free your map before returning.
//
// LeetCode's real signature is
// `int* twoSum(int* nums, int numsSize, int target, int* returnSize)`.

#define HASHMAP_NO_MAIN
#include "hashMap.c"
#include <stdio.h>

// find the number of caracters required to represent a standard decimal number
// includes space for negative sign and \0
size_t count_str_size(int n) {
  if (n == 0) return 2;
  size_t size = 1;
  if (n < 0) {
    size += 1;
  };
  while (n != 0){
    n /= 10;
    size++;
  }
  return size;
}


int *twoSum(const int *nums, int numsSize, int target, int *returnSize) {
  assert(numsSize > 1 && "Nums array needs at least 2 elements");

  *returnSize = 0;
  if (numsSize == 2) {
    int *ret = malloc(2 * sizeof(int));
    *returnSize = 2;
    ret[0] = 0; ret[1] = 1;
    return ret;
  }
  size_t keySize = count_str_size(target);
  const int *p = nums;
  while (p < nums + numsSize) {
    size_t newSize = count_str_size(*p);
    if (keySize < newSize) keySize = newSize;
    p++;
  }
  char *key = malloc(keySize);
  assert(key != NULL);

  struct map m;
  map_init(&m);
  p = nums;
  while (p < nums + numsSize) {

    int tDiff = target - *p;
    //snprintf setup
    int val;
    snprintf(key,keySize, "%d", tDiff);
    // get to see if the diff value is in the map. c - a = b
    bool get = map_get(&m, key, &val);
    if (get) {
      *returnSize = 2;
      int *ret = malloc(2 * sizeof(int));
      ret[0] = val; ret[1] = (int)(p - nums);
      map_free(&m);
      free(key);
      return ret;
    }
    // if failed get, insert *p
    snprintf(key, keySize, "%d", *p);
    bool put = map_put(&m, key, (int)(p - nums));
    assert(put && "your map couldn't insert, go check your broken code :)");
    p++;
  }
  map_free(&m);
  free(key);
  return NULL;
}
int *twoSum_Brute(const int *nums, int numsSize, int target, int *returnSize) {
  *returnSize = 0;
  if (numsSize == 2) {
  int *ret = malloc(2 * sizeof(int));
    *returnSize = 2;
    ret[0] = 0; ret[1] = 1;
    return ret;
  }
  const int *pin = nums;
  const int *scan = nums+1;
  while (pin < nums+numsSize - 1){
    scan = pin + 1;
    while (scan < nums+numsSize){
      if (*pin + *scan == target) {
        int *ret = malloc(2 * sizeof(int));
        *returnSize = 2;
        ret[0] = (int)(pin - nums); ret[1] = (int)(scan - nums);
        return ret;
      }
      scan++;
    }
    pin++;
  }
  return NULL;
}
// Copies `nums` into a heap buffer of exactly n ints, so ASan reports any
// read past the end.
static int *copy(const int *nums, int n) {
  int *buf = malloc((size_t)n * sizeof *buf);
  assert(buf != NULL);
  memcpy(buf, nums, (size_t)n * sizeof *buf);
  return buf;
}

#define ALGO twoSum
#ifndef ALGO
#define ALGO twoSum_Brute
#endif

// Checks that twoSum returns {i, j} in either order.
static void check(const int *nums, int n, int target, int i, int j) {
  int *in = copy(nums, n);
  int size = -1;
  int *got = ALGO(in, n, target, &size);
  assert(got != NULL);
  assert(size == 2);
  assert((got[0] == i && got[1] == j) || (got[0] == j && got[1] == i));
  free(got);
  free(in);
}

int main(void) {
  assert(count_str_size(0) == 2);
  assert(count_str_size(1) == 2);
  assert(count_str_size(10) == 3);
  assert(count_str_size(-1) == 3);
  assert(count_str_size(-10) == 4);
  assert(count_str_size(123456789) == 10);
  check((int[]){2, 7, 11, 15}, 4, 9, 0, 1);
  check((int[]){3, 2, 4}, 3, 6, 1, 2);

  check((int[]){3, 3}, 2, 6, 0, 1);                     // equal values
  check((int[]){3, 2, 3}, 3, 6, 0, 2);                  // 3 + 3, not 3 used twice
  check((int[]){-1, -2, -3, -4, -5}, 5, -8, 2, 4);      // negatives
  check((int[]){0, 4, 3, 0}, 4, 0, 0, 3);               // zeros
  check((int[]){5, 75, 25}, 3, 100, 1, 2);              // answer at the end
  check((int[]){1000000000, -1000000000}, 2, 0, 0, 1);  // limits
  check((int[]){-1000000000, -1000000000}, 2, -2000000000, 0, 1);

  // 10,000 numbers: forces the map to resize many times.
  enum { N = 10000 };
  static int big[N];
  for (int k = 0; k < N; k++) big[k] = k;
  check(big, N, (N - 2) + (N - 1), N - 2, N - 1);

  return 0;
}
