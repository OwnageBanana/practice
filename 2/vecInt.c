// Learning path step 2: dynamic array (growable array of int).
//
// A `struct vec` owns a heap buffer `data` with room for `cap` ints, of which
// the first `len` are in use. Pushing onto a full vec grows the buffer with
// realloc, doubling `cap` (start at some small cap, e.g. 8, when it's 0).
//
// Rules:
// - realloc can move the block, or fail and return NULL. On failure the old
//   block is still valid, so don't lose it: assign through a temporary.
// - If vec_push can't grow, return false and leave the vec unchanged.
// - The vec owns `data`. vec_free releases it and resets the vec to empty, so
//   it can be reused or freed again safely.
// - An empty vec ({NULL, 0, 0}) needs no allocation. realloc(NULL, n) acts
//   like malloc(n).

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

struct vec {
  int *data;
  size_t len;
  size_t cap;
};

// Makes `v` an empty vec. Allocates nothing.
void vec_init(struct vec *v) {
  v->data = NULL;
  v->len = 0;
  v->cap = 0;
}

// Appends `x`, growing the buffer if it's full. Returns false if the buffer
// couldn't grow, and leaves `v` unchanged in that case.
bool vec_push(struct vec *v, int x) {
  // grow
  if (v->len == v->cap){
    if (v->cap > SIZE_MAX / 2 / sizeof(*v->data)) return false;
    size_t newCap = v->cap;
    if (newCap == 0) newCap = 64;
    else newCap *= 2;
    int *p = realloc(v->data, sizeof(*v->data) * newCap);
    if (p == NULL) return false;

    v->data = p;
    v->cap = newCap;
  }
  v->data[v->len] = x;
  v->len++;

  return true;
}

int vec_get(const struct vec *v, size_t i) {
  assert (i < v->len);
   return v->data[i];
}

// Removes and returns the last element. Requires v->len > 0 (assert it).
// Doesn't shrink the buffer.
int vec_pop(struct vec *v) {
  assert(v->len > 0);
  return v->data[--v->len];
}

// Frees the buffer and resets `v` to empty.
void vec_free(struct vec *v) {
  // TODO
  free(v->data);
  v->data = NULL;
  v->cap = 0;
  v->len = 0;
}

int main(void) {
  struct vec v;
  vec_init(&v);
  assert(v.data == NULL && v.len == 0 && v.cap == 0);

  // Push enough to force several doublings.
  for (int i = 0; i < 1000; i++) {
    assert(vec_push(&v, i * 3));
    assert(v.len == (size_t)i + 1);
    assert(v.cap >= v.len);
  }
  for (size_t i = 0; i < 1000; i++) {
    assert(vec_get(&v, i) == (int)i * 3);
  }

  // Doubling means cap is never more than twice what's needed (once it
  // passes the starting cap).
  assert(v.cap < 2 * v.len);

  assert(vec_pop(&v) == 999 * 3);
  assert(vec_pop(&v) == 998 * 3);
  assert(v.len == 998);
  assert(vec_get(&v, 997) == 997 * 3);

  vec_free(&v);
  assert(v.data == NULL && v.len == 0 && v.cap == 0);

  // A freed vec can be used again, and freeing twice is safe.
  assert(vec_push(&v, 42));
  assert(vec_get(&v, 0) == 42);
  vec_free(&v);
  vec_free(&v);

  return 0;
}
