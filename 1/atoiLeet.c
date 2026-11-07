// LeetCode 8. String to Integer (atoi)
//
// Convert the start of s to a 32-bit signed int:
//   1. Skip leading spaces (' ' only).
//   2. Read an optional '+' or '-'. No sign means positive.
//   3. Read digits until a non-digit or the end. Leading zeros are fine.
//      No digits at all means the result is 0.
//   4. If the value is out of range, clamp it to INT_MIN or INT_MAX.
// Example: "   -042"   ->  -42
//          "1337c0d3"  ->  1337
//          "-91283472332"  ->  INT_MIN
//
// Don't hold the result in a wider type like long: detect that the next
// `result * 10 + digit` would overflow *before* you compute it.
// (Signed overflow is UB, and UBSan will report it.)
//
// LeetCode's real signature is `int myAtoi(char* s)`.

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

int myAtoi(const char *s) {
  const char *p = s;
  int val = 0;
  while (*p != '\0' && *p == ' '){
    p++;
  }
  bool isNeg = *p == '-';
  if (*p == '-' || *p == '+') p++;
  // after skipping whitespace and a potential sign if the numbers not a digit its invalid
  if (!isdigit((unsigned char) *p)) return 0;
  // remove leading zeros
  while (*p != '\0' && *p == '0') {
    p++;
  }
  while (*p != '\0' && isdigit((unsigned char) *p)) {
    int digit = (*p - '0');
    if ((INT_MAX - digit) / 10  < val){
      return isNeg ? INT_MIN : INT_MAX;
    }
    val = (val * 10) + digit;
    p++;
  }
  return isNeg ? -val : val;
}

// Copies `in` into a heap buffer of exactly strlen + 1 bytes, so ASan reports
// any read past the '\0', then checks the result.
static void check(const char *in, int want) {
  size_t n = strlen(in) + 1;
  char *buf = malloc(n);
  assert(buf != NULL);
  memcpy(buf, in, n);
  assert(myAtoi(buf) == want);
  free(buf);
}

int main(void) {
  check("42", 42);
  check("   -042", -42);
  check("1337c0d3", 1337);            // stop at the first non-digit
  check("0-1", 0);
  check("words and 987", 0);          // no leading digits

  check("", 0);
  check("   ", 0);
  check("+", 0);                      // sign with no digits
  check("-", 0);
  check("+-12", 0);                   // only one sign allowed
  check(" + 1", 0);                   // no space after the sign
  check("+7", 7);
  check("00000000000012345678", 12345678);

  check("2147483647", INT_MAX);       // exactly fits
  check("2147483648", INT_MAX);       // one too big
  check("-2147483648", INT_MIN);      // exactly fits, but -INT_MIN doesn't
  check("-2147483649", INT_MIN);      // one too small
  check("21474836460", INT_MAX);      // overflows on the last digit
  check("-91283472332", INT_MIN);
  check("99999999999999999999", INT_MAX);

  return 0;
}
