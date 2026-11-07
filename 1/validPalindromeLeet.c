// LeetCode 125. Valid Palindrome
//
// Return true if s reads the same forwards and backwards after lowercasing
// it and ignoring every character that isn't a letter or a digit.
// Example: "A man, a plan, a canal: Panama"  ->  true ("amanaplanacanalpanama")
//          "race a car"                     ->  false
//
// Aim for O(n) time and O(1) extra memory: don't build a cleaned-up copy.
// <ctype.h> has helpers for classifying and lowercasing characters.
//
// LeetCode's real signature is `bool isPalindrome(char* s)`.


#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

bool isPalindrome(const char *s) {
  size_t len = strlen(s);
  if (len <= 1) {
    return true;
  }

  const char *a = s;
  const char *b = s + len - 1;
  while (a < b) {
    while (a < b && !isalnum((unsigned char)*a)){
      a++;
    }
    while (b >= a && !isalnum((unsigned char)*b)){
      b--;
    }
    if (a >= b) {
      return true;
    }
    if(tolower((unsigned char) *a) != tolower((unsigned char)*b)){
      return false;
    }
    a++; b--;
  }
  return true;
}

// Copies `in` into a heap buffer of exactly strlen + 1 bytes, so ASan reports
// any read past the '\0', then checks the result.
static void check(const char *in, bool want) {
  size_t n = strlen(in) + 1;
  char *buf = malloc(n);
  assert(buf != NULL);
  memcpy(buf, in, n);
  assert(isPalindrome(buf) == want);
  free(buf);
}

int main(void) {
  check("aa", true);
  check("abab", false);

  check("A man, a plan, a canal: Panama", true);
  check("race a car", false);

  check("", true);                    // empty string
  check(" ", true);                   // nothing left after filtering
  check(".,", true);                  // only punctuation
  check("a", true);
  check("a.", true);                  // skip at one end only
  check("ab_a", true);                // '_' is not alphanumeric
  check("0P", false);                 // digits and letters aren't the same
  check("Aa", true);                  // case-insensitive
  check("abba", true);                // even length
  check("abcba", true);               // odd length
  check("abca", false);

  return 0;
}
