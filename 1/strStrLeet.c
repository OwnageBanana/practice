// LeetCode 28. Find the Index of the First Occurrence in a String
//
// Return the index of the first place needle appears in haystack, or -1 if
// it doesn't appear. An empty needle matches at index 0 (like C's strstr).
// Example: haystack "sadbutsad", needle "sad"   ->  0
//          haystack "leetcode",  needle "leeto" ->  -1
//
// Don't call strstr or memcmp. The straightforward approach, trying each
// start position in haystack, is O(n * m) and is fine here. Make sure no
// read ever goes past either string's '\0'.
//
// LeetCode's real signature is `int strStr(char* haystack, char* needle)`.

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int strStr(const char *haystack, const char *needle) {

  if (*haystack == '\0' && *needle == '\0') return 0;
  int index = 0;
  const char *n = needle;
  while (*haystack != '\0') {
    while(*haystack != '\0' && *n != '\0' && *n == *haystack){
      n++; haystack++;
    }
    if (*n == '\0') return index; // iterated through needle - its a match
    else if (*haystack == '\0') return -1;
    else { haystack -= (n - needle); n = needle;}
    index++; haystack++;
  }
  return -1;
}

// Copies `s` into a heap buffer of exactly strlen + 1 bytes, so ASan reports
// any read past the '\0'.
static char *copy(const char *s) {
  size_t n = strlen(s) + 1;
  char *buf = malloc(n);
  assert(buf != NULL);
  memcpy(buf, s, n);
  return buf;
}

static void check(const char *haystack, const char *needle, int want) {
  char *h = copy(haystack);
  char *n = copy(needle);
  assert(strStr(h, n) == want);
  free(h);
  free(n);
}

int main(void) {
  check("sadbutsad", "sad", 0);
  check("leetcode", "leeto", -1);

  check("hello", "ll", 2);
  check("hello", "o", 4);             // match at the very end
  check("hello", "lo", 3);
  check("hello", "loo", -1);          // runs off the end of haystack
  check("abc", "abcd", -1);           // needle longer than haystack
  check("abc", "abc", 0);             // whole string
  check("a", "a", 0);
  check("abc", "", 0);                // empty needle
  check("", "", 0);
  check("", "a", -1);                 // empty haystack

  check("aaab", "aab", 1);            // partial match, then restart one later
  check("mississippi", "issip", 4);   // false start at index 1
  check("abababc", "ababc", 2);

  return 0;
}
