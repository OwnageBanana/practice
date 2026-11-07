#include <stdio.h>
#include <string.h>
// Arrays and strings, no containers. Reverse string in place, remove duplicates from sorted array,
// move zeroes, rotate array, max subarray, valid palindrome, atoi with proper overflow handling,
// implement strstr.

void revStr(char *a, int len) {
  if (len == 0) return;
  if (a[len - 1] == '\0') {len -= 1;}
  int i = 0;
  char store = '\0';
  while (i < len / 2) {
    store = a[len - i - 1];
    a[len - i - 1] = a[i];
    a[i] = store;
    i++;
  }
}

int main() {
  char a[] = "Hello World!";
  char b[] = "Hello Mr.World Wide!";
  char c[] = "Channel Five! World Wide!";

  revStr(a, sizeof(a));
  printf("%s\n", a);
  revStr(b, sizeof(b));
  printf("%s\n", b);
  revStr(c, sizeof(c));
  printf("%s\n", c);
  return 0;
}
W