// Learning path step 3: hash map from string keys to int values.
//
// Layout: one flat array of `cap` slots (open addressing). Each slot is
// EMPTY, FULL (holds a key and value), or a TOMBSTONE (held a key that was
// removed).
//
// Hashing: FNV-1a over the key's bytes, 64-bit:
//   h = 14695981039346656037
//   for each byte b:  h ^= b;  h *= 1099511628211;
// `cap` is always a power of two, so the starting slot is `h & (cap - 1)`
// (the same as `h % cap`, but cheaper).
//
// Linear probing: if the starting slot isn't the one you want, try the next
// slot, wrapping around at the end: `i = (i + 1) & (cap - 1)`.
// - Lookups keep probing past FULL slots with other keys and past
//   TOMBSTONEs. They stop at an EMPTY slot: the key isn't in the map.
// - That's why remove can't just mark a slot EMPTY. A later key may have
//   probed past this slot when it was inserted, and an EMPTY here would hide
//   it. Remove marks the slot TOMBSTONE instead.
//
// Load factor: grow before an insert would make (len + tombstones) exceed 70%
// of cap. Tombstones count because they make probes longer just like FULL
// slots do. Integer form, no floats: (len + tombstones + 1) * 10 > cap * 7.
// Growing allocates a new slot array and reinserts every FULL entry (not the
// tombstones, which disappear). If tombstones >= len, rehash at the same cap
// instead of doubling, since the table is mostly dead space. Start at cap 8.
//
// Ownership: the map copies each key on insert (malloc + memcpy) and frees
// its copies on remove and in map_free. The caller's string can change or be
// freed right after map_put returns.
//
// Rules carried over: an empty map ({NULL, 0, 0, 0}) allocates nothing,
// failures return false and leave the map unchanged, and map_free is safe to
// call twice.

// A good order to work in:

// 1. map_init and fnv1a. The first asserts check your hash against the
// official FNV-1a test values, so you'll know straight away whether it's right.
// 2. map_get, which is the probe loop. Everything else is built around it.
// 3. map_put and map_grow.
// 4. map_remove (tombstones) and map_free.

// The last test inserts and removes 10,000 keys. It checks that tombstones don't pile up until there are no empty slots left (then a probe for a missing key would loop forever), and that they don't make the table keep doubling.

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum slot_state { EMPTY, FULL, TOMBSTONE };

struct slot {
  enum slot_state state;
  char *key;  // owned by the map when FULL; NULL otherwise
  int value;
};

struct map {
  struct slot *slots;
  size_t cap;         // number of slots, 0 or a power of two
  size_t len;         // FULL slots
  size_t tombstones;  // TOMBSTONE slots
};

// Makes `m` an empty map. Allocates nothing.
void map_init(struct map *m) {
  m->slots = NULL;
  m->cap = 0;
  m->len = 0;
  m->tombstones = 0;
}

// FNV-1a hash of the string `key` (not including its '\0').
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
static uint64_t fnv1a(const char *key) {
  uint64_t h = 14695981039346656037u; //ofset basis
  while (*key != '\0') {
    // for an explaination on why this prime was chosen
    //https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV_hash_parameters
    h ^= (unsigned char)*key; h *= 1099511628211u; // FNV 1a prime
    // h *= 1099511628211; h ^= (unsigned char)*key;
    // // FNV 1 reverses the order of when we xor/prime multiply
    // "The change in order [of fnv 1a from fnv 1] leads to slightly better avalanche characteristics."
    // aka better randomness between keys when changing 1 bit
    key++;
  }
  return h;
}

// Rebuilds the table at a new capacity (see "Load factor" above) and
// reinserts every FULL entry. The keys move over as pointers: no copying.
// Returns false if the allocation fails, leaving `m` unchanged.
// Hint: calloc zeroes memory, and EMPTY is 0, so calloc gives all-EMPTY slots.
static bool map_grow(struct map *m) {

  if (m->cap >  SIZE_MAX / 2 / sizeof(struct slot)) return false;

  size_t newCap = 0;
  if (m->len == 0) newCap = 16;
  else {
    newCap = 1;
    while(newCap * 7 < (m->len + 1) * 10) newCap *= 2;
  }

  struct slot *p = calloc(newCap, sizeof(struct slot));
  if (p == NULL) return false; // failed alloc

  struct slot *s = m->slots;
  size_t newLen = 0;

  while(s < m->slots + m->cap) {
    if (s->state != FULL) { s++; continue; }

    uint64_t h = fnv1a(s->key);
    // do put of slot info in new array of slots
    size_t capMask = newCap - 1;
    for(size_t j = 0, i = h & capMask;
      j < newCap;
      j++, i = (i + 1) & capMask) {
        if (p[i].state == FULL) continue;
        p[i].state = FULL;
        p[i].key = s->key; // reuse key pointer, no copy
        p[i].value = s->value;
        newLen++;
        break;
    }
    s++;
  }

  free(m->slots);
  m->slots = p;
  m->cap = newCap;
  m->len = newLen;
  m->tombstones = 0;
  return true;
}

// Sets `key` to `value`, inserting it if it's new or updating it if it's
// already there. Returns false if memory runs out, leaving `m` unchanged.
bool map_put(struct map *m, const char *key, int value) {

  if ((m->len + 1 + m->tombstones) * 10 >= m->cap * 7) {
    if (!map_grow(m)) return false;
  }
  // hash is some random string of bits, & (cap - 1) limits it to the indexable range of the array of slots.
  // this is our starting point in the hash indexing. its random enough that it is a decent way to avoid linear searching
  // a whole array but its still O(n)
  uint64_t h = fnv1a(key);
  size_t capMask = (m->cap - 1);

  for(size_t j = 0, i = h & capMask;
      j < m->cap;
      j++, i = (i + 1) & capMask) {
    if (m->slots[i].state == TOMBSTONE) continue;
    // update value
    if (m->slots[i].state == FULL && 0 == strcmp(m->slots[i].key, key)) {
      m->slots[i].value = value;
      return true;
    // insert value
    } else if(m->slots[i].state == EMPTY) {
      size_t slen = strlen(key) + 1;
      m->slots[i].key = malloc(slen);
      if (m->slots[i].key == NULL) {
        return false;
      }
      m->slots[i].state = FULL;
      m->slots[i].key = memcpy(m->slots[i].key, key, slen);
      m->slots[i].value = value;
      m->len++;
      return true;
    }

  }
  return false;
}

// If `key` is in the map, stores its value in *out and returns true.
// Otherwise returns false and leaves *out alone.
// linear search
bool map_get(const struct map *m, const char *key, int *out) {
  uint64_t h = fnv1a(key);
  size_t capMask =(m->cap - 1);

  // search has to loop back around
  for(size_t j = 0, i = h & capMask;
      j < m->cap;
      j++, i = (i + 1) & capMask) {
    if      (m->slots[i].state == EMPTY) return false;
    else if (m->slots[i].state == TOMBSTONE) continue;
    else if (0 == strcmp(m->slots[i].key, key)) {
      *out = m->slots[i].value;
      return true;
    }
  }
  return false;
}

// Removes `key` if present. Returns true if it was there.
bool map_remove(struct map *m, const char *key) {
  uint64_t h = fnv1a(key);
  size_t capMask = (m->cap - 1);

  // search has to loop back around
  for(size_t j = 0, i = h & capMask;
      j < m->cap;
      j++, i = (i + 1) & capMask) {
    if      (m->slots[i].state == EMPTY) return false;
    else if (m->slots[i].state == TOMBSTONE) continue;
    else if (m->slots[i].state == FULL && 0 == strcmp(m->slots[i].key, key)) {
      m->slots[i].state = TOMBSTONE;
      free(m->slots[i].key); // free the key for a tombstone. that memory doesn't need to persist
      m->slots[i].key = NULL;
      m->tombstones++;
      m->len--;
      return true;
    }
  }
  return false;
}

// Frees every key and the slot array, and resets `m` to empty.
void map_free(struct map *m) {
  // TODO
  for (size_t i = 0; i < m->cap; i++) {
    if (m->slots[i].state == FULL){
      free(m->slots[i].key);
    }
  }
  free (m->slots);
  map_init(m);
}

// The tests below are skipped when another file includes this one with
// HASHMAP_NO_MAIN defined (e.g. twoSumLeet.c), so it gets the map without main.
#ifndef HASHMAP_NO_MAIN

static bool load_ok(const struct map *m) {
  return (m->len + m->tombstones) * 10 <= m->cap * 7;
}

int main(void) {
  struct map m;
  map_init(&m);
  assert(m.slots == NULL && m.cap == 0 && m.len == 0 && m.tombstones == 0);

  // Hash values from the FNV-1a reference test vectors.
  assert(fnv1a("") == 14695981039346656037u);
  assert(fnv1a("a") == 0xaf63dc4c8601ec8cu);
  assert(fnv1a("foobar") == 0x85944171f73967e8u);

  int v = -1;
  assert(!map_get(&m, "missing", &v));  // lookup in an empty map
  assert(v == -1);
  assert(!map_remove(&m, "missing"));

  assert(map_put(&m, "apple", 1));
  assert(map_put(&m, "banana", 2));
  assert(map_get(&m, "apple", &v) && v == 1);
  assert(map_get(&m, "banana", &v) && v == 2);
  assert(!map_get(&m, "cherry", &v));
  assert(m.len == 2);

  // Updating an existing key doesn't add a new entry.
  assert(map_put(&m, "apple", 10));
  assert(map_get(&m, "apple", &v) && v == 10);
  assert(m.len == 2);

  // The map keeps its own copy of the key.
  char buf[16] = "grape";
  assert(map_put(&m, buf, 3));
  strcpy(buf, "XXXXX");
  assert(map_get(&m, "grape", &v) && v == 3);
  assert(!map_get(&m, "XXXXX", &v));

  // Remove, then the key is gone but the others are still found.
  assert(map_remove(&m, "banana"));
  assert(!map_remove(&m, "banana"));
  assert(!map_get(&m, "banana", &v));
  assert(map_get(&m, "apple", &v) && v == 10);
  assert(m.len == 2);
  assert(map_put(&m, "banana", 20));
  assert(map_get(&m, "banana", &v) && v == 20);
  map_free(&m);
  assert(m.slots == NULL && m.cap == 0 && m.len == 0 && m.tombstones == 0);

  // Many keys: forces several resizes. Every key must still be found.
  char key[32];
  for (int i = 0; i < 2000; i++) {
    snprintf(key, sizeof key, "key%d", i);
    assert(map_put(&m, key, i));
    assert(load_ok(&m));
  }
  assert(m.len == 2000);
  for (int i = 0; i < 2000; i++) {
    snprintf(key, sizeof key, "key%d", i);
    assert(map_get(&m, key, &v) && v == i);
  }

  // Remove every even key. With this many keys, plenty of odd keys probed
  // past an even key's slot on insert, so this checks that lookups continue
  // past tombstones.
  for (int i = 0; i < 2000; i += 2) {
    snprintf(key, sizeof key, "key%d", i);
    assert(map_remove(&m, key));
  }
  assert(m.len == 1000);
  for (int i = 0; i < 2000; i++) {
    snprintf(key, sizeof key, "key%d", i);
    bool found = map_get(&m, key, &v);
    assert(found == (i % 2 == 1));
    if (found) assert(v == i);
  }
  map_free(&m);

  // Insert/remove churn: tombstones must not pile up until the table is
  // full (an endless probe) or keep doubling the table.
  for (int i = 0; i < 10000; i++) {
    snprintf(key, sizeof key, "tmp%d", i);
    assert(map_put(&m, key, i));
    assert(map_remove(&m, key));
    assert(load_ok(&m));
  }
  assert(m.len == 0);
  assert(m.cap <= 64);

  map_free(&m);
  map_free(&m);
  return 0;
}
#endif  // HASHMAP_NO_MAIN
