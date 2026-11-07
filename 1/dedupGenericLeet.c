// Generic Remove Duplicates (extends LeetCode 26)
//
// Same task as deDupLeet.c, but for any element type, in the style of qsort:
//   base  points at the first element
//   n     is the number of elements
//   size  is the size of one element in bytes
//   eq    returns non-zero if the two elements it's given are equal
// Elements equal under eq are adjacent. Keep the first of each run, in order,
// and return k. Use O(1) extra memory.

#include <stdio.h>
#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>

size_t dedup(void *base, size_t n, size_t size,
             int (*eq)(const void *, const void *)) {

  if (n <= 1) return n;
  // unsigned char acts as a byte type allowing pointer arethmetic that void * does not permit because void has no size.
  unsigned char *b = base;

  unsigned char *read = b + size;
  unsigned char *write = b;
  while (read < b + n * size){
    if ((*eq)(write, read) == 0) {
      write += size;
      if (write != read){
        memmove(write, read, size);
      }
    }
    read+=size;
  }
  return ((size_t)(write - b) + size) / size;
}

// ---- Equality callbacks ----
// Each gets pointers to two elements, as const void *. Convert them back to
// the real element type before comparing.

static int eqInt(const void *a, const void *b) {
  const int *x = a;
  const int *y = b;
  return *x == *y;
}

static int eqChar(const void *a, const void *b) {
  const char *x = a;
  const char *y = b;
  return *x == *y;
}

// Each element is a `const char *`, so a and b point AT pointers.
static int eqStr(const void *a, const void *b) {
  const char *const *pa = a;
  const char *x = *pa;
  const char *const *pb = b;
  const char *y = *pb;

  while (*x != '\0' && *y != '\0'){
    if (*x != *y) return 0;
    x++; y++;
  }
  return *x == *y;
}

struct item {
  char tag;     // 1 byte, then padding before x
  int count;
};

static int eqItem(const void *a, const void *b) {
  // TODO: compare field by field (why not memcmp?)
  const struct item *x = a;
  const struct item *y = b;
  if (x->tag != y->tag || x->count != y->count) return 0;
  return 1;
}

// ---- Test harness ----

// Copies `in` into an aligned buffer, runs dedup, then checks k and the first
// k elements using eq.
static void check(const void *in, size_t n, size_t size,
                  int (*eq)(const void *, const void *),
                  const void *want, size_t wantK) {
  // alignas(max_align_t) makes buf safe to use as an array of any type.
  alignas(max_align_t) unsigned char buf[512];
  assert(n * size <= sizeof(buf));
  memcpy(buf, in, n * size);

  size_t k = dedup(buf, n, size, eq);
  assert(k == wantK);

  const unsigned char *got = buf;
  const unsigned char *exp = want;
  for (size_t i = 0; i < k; i++)
    assert(eq(got + i * size, exp + i * size));
}

int main(void) {
  int nums[] = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
  int numsW[] = {0, 1, 2, 3, 4};
  check(nums, sizeof(nums) / sizeof(nums[0]), sizeof(int), eqInt, numsW, 5);
  nums
  check(nums, 0, sizeof(int), eqInt, nums, 0);   // empty: don't touch base

  char letters[] = {'a', 'a', 'b', 'c', 'c', 'c'};
  char lettersW[] = {'a', 'b', 'c'};
  check(letters, sizeof(letters), sizeof(char), eqChar, lettersW, 3);

  const char *words[] = {"apple", "apple", "fig", "kiwi", "kiwi"};
  const char *wordsW[] = {"apple", "fig", "kiwi"};
  check(words, sizeof(words) / sizeof(words[0]), sizeof(const char *), eqStr,
        wordsW, 3);

  struct item items[] = {{'a', 1}, {'a', 1}, {'b', 2}, {'b', 3}, {'b', 3}};
  struct item itemsW[] = {{'a', 1}, {'b', 2}, {'b', 3}};
  check(items, sizeof(items) / sizeof(items[0]), sizeof(struct item), eqItem,
        itemsW, 3);

  return 0;
}
