// LeetCode 344. Reverse String
//
// Reverse the array of characters s in place. s is NOT NUL-terminated:
// sSize is the number of characters. Use O(1) extra memory.
//
// LeetCode's real signature is `void reverseString(char* s, int sSize)`.
// This version takes size_t for practice.

#include <assert.h>
#include <stddef.h>
#include <string.h>

void reverseString(char *s, size_t sSize) {
  if (sSize <= 1) return;
  char *left, *right;
  char swap = '\0';
  left = s;
  right = s + sSize - 1;
  while (asright < left) {
    swap = *left;
    *left = *right;
    *right = swap;
    left++;
    right--;
  }
}

// Reverses `in`, then checks that it matches `want`. Both are written as
// string literals for convenience, but only the first n chars are compared.
static void check(const char *in, const char *want) {
  char buf[64];
  size_t n = strlen(in);
  memcpy(buf, in, n);
  reverseString(buf, n);
  assert(memcmp(buf, want, n) == 0);
}

int main(void) {
  check("hello", "olleh");          // odd length
  check("Hannah", "hannaH");        // even length
  check("ab", "ba");
  check("a", "a");
  check("", "");                    // empty: watch your unsigned math
  return 0;
}
