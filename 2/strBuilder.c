// Learning path step 2: string builder (growable, always '\0'-terminated).
//
// Same idea as struct vec, but for chars, with one extra rule: the text must
// always end in '\0' so it can be passed to printf, strcmp, etc.
//
// - `len` is the number of chars in the string, not counting the '\0'
//   (like strlen).
// - `cap` is the number of bytes allocated, so it includes room for the '\0'.
//   Whenever data != NULL: len + 1 <= cap and data[len] == '\0'.
// - An empty builder ({NULL, 0, 0}) allocates nothing.
//
// Rules carried over from vec:
// - Grow with realloc through a temporary. On failure return false and leave
//   the builder unchanged.
// - Check for size_t overflow before doing size arithmetic.
// - Never call realloc with size 0.

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct sb {
  char *data;
  size_t len;
  size_t cap;
};

// Makes `s` an empty builder. Allocates nothing.
void sb_init(struct sb *s) {
  // TODO
  s->data = NULL;
  s->len = 0;
  s->cap = 0;
}

// Makes sure there's room for `extra` more chars plus the '\0'. Grows by
// doubling (start at 16 when cap is 0), and keeps doubling until it's big
// enough. Returns false if it can't, leaving `s` unchanged.
// Both append functions call this, so the growth logic lives in one place.
static bool sb_reserve(struct sb *s, size_t extra) {
  // need to check we don't over allocate -> allocated mem won't exceed size max
  if ( s->len >= SIZE_MAX - extra) return false;
  size_t newLen = s->len + extra;

  if (s->cap < newLen + 1) {
    size_t newCap = s->cap;
    do {
      if (newCap > SIZE_MAX / 2) return false;
      if (newCap == 0) newCap = 16;
      else newCap *= 2;
    } while (newCap - s->len < extra + 1);
    char *p = realloc(s->data, newCap);
    if (p == NULL) return false;
    s->data = p;
    s->cap = newCap;
  }

  return true;
}

// Appends the string `str`. Returns false on failure, leaving `s` unchanged.
bool sb_append(struct sb *s, const char *str) {
  size_t len = strlen(str);
  bool cando = sb_reserve(s,len);
  if (!cando) return false;
  memcpy(s->data + s->len, str, len + 1);
  s->len += len;
  return true;
}

// Appends one char. Returns false on failure, leaving `s` unchanged.
bool sb_append_char(struct sb *s, char c) {
  bool cando = sb_reserve(s,1);
  if (!cando) return false;
  s->data[s->len] = c;
  s->data[s->len + 1] = '\0';
  s->len += 1;
  return true;
}

// Returns the current text. Never returns NULL: an empty builder that hasn't
// allocated yet returns "". The pointer is only valid until the next append
// or free (an append may realloc).
const char *sb_str(const struct sb *s) {
  // TODO
  if (s->data==NULL) return "";
  return s->data;
}
// Frees the buffer and resets `s` to empty. Safe to call twice.
void sb_free(struct sb *s) {
  free(s->data);
  s->data = NULL;
  s->cap = 0;
  s->len = 0;
}
// Hands the string over to the caller and resets `s` to empty. The caller
// must free() the result. Returns NULL only if memory runs out (an empty
// builder still has to return an allocated "").
char *sb_finish(struct sb *s) {
  char *p = s->data;
  if (p==NULL) {
    p = malloc(1);
    if (p == NULL) return p;
    *p = '\0';
    return p;

  }
  // p = realloc(p, s->len + 1);
  s->data = NULL;
  s->cap = 0;
  s->len = 0;
  return p;
}



int main(void) {
  struct sb s;
  sb_init(&s);
  assert(s.data == NULL && s.len == 0 && s.cap == 0);
  assert(strcmp(sb_str(&s), "") == 0);

  assert(sb_append(&s, "hello"));
  assert(sb_append(&s, ", "));
  assert(sb_append(&s, "world"));
  assert(sb_append_char(&s, '!'));
  assert(strcmp(sb_str(&s), "hello, world!") == 0);
  assert(s.len == 13);
  assert(s.cap >= s.len + 1);

  // Appending "" changes nothing.
  assert(sb_append(&s, ""));
  assert(s.len == 13);

  // Force several doublings, then check every char and the terminator.
  sb_free(&s);
  for (int i = 0; i < 1000; i++) {
    assert(sb_append(&s, "ab"));
    assert(s.data[s.len] == '\0');
  }
  assert(s.len == 2000);
  assert(strlen(sb_str(&s)) == 2000);
  for (size_t i = 0; i < s.len; i++) {
    assert(s.data[i] == (i % 2 == 0 ? 'a' : 'b'));
  }

  // Exactly 16 chars into an empty builder: the '\0' needs a 17th byte.
  sb_free(&s);
  assert(sb_append(&s, "0123456789abcdef"));
  assert(s.len == 16 && s.cap >= 17 && s.data[16] == '\0');

  // One append much bigger than the current cap.
  sb_free(&s);
  assert(sb_append(&s, "x"));
  char big[100];
  memset(big, 'y', sizeof big - 1);
  big[sizeof big - 1] = '\0';
  assert(sb_append(&s, big));
  assert(s.len == 100);
  assert(s.data[0] == 'x' && s.data[99] == 'y' && s.data[100] == '\0');

  // sb_finish transfers ownership: the caller frees the result.
  char *out = sb_finish(&s);
  assert(out != NULL && strlen(out) == 100);
  assert(s.data == NULL && s.len == 0 && s.cap == 0);
  free(out);

  // Finishing an empty builder gives an allocated "".
  out = sb_finish(&s);
  assert(out != NULL && strcmp(out, "") == 0);
  free(out);

  // Reusable after finish, and freeing twice is safe.
  assert(sb_append(&s, "again"));
  assert(strcmp(sb_str(&s), "again") == 0);
  sb_free(&s);
  sb_free(&s);

  return 0;
}
