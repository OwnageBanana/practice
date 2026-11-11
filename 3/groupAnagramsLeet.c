// gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -g -fsanitize=address,undefined -o groupAnagrams.out groupAnagramsLeet.c
// LeetCode 49. Group Anagrams
//
// Given an array of strings `strs`, group the strings that are anagrams of
// each other (same letters, same counts, any order). Return the groups in
// any order, with the strings inside each group in any order.
// Example: ["eat","tea","tan","ate","nat","bat"]
//          -> [["bat"], ["nat","tan"], ["ate","eat","tea"]]
//          [""]  -> [[""]]
//          ["a"] -> [["a"]]
//
// Two strings are anagrams exactly when sorting their letters gives the same
// string ("eat", "tea", "ate" all sort to "aet"). Use that sorted form as the
// key in your hash map from hashMap.c, which this file includes. The value
// can be the group's index.
// Sort a copy, not strs[i] itself: the result has to hold the original
// strings.
//
// LeetCode's limits: 1 <= strsSize <= 10^4, 0 <= strlen(strs[i]) <= 100, and
// every char is a lowercase letter 'a' to 'z'.
//
// Return convention, one level deeper than two sum:
// - The result is a malloc'd array of `*returnSize` groups.
// - Each group is a malloc'd array of `char *`, and its length goes in
//   `(*returnColumnSizes)[i]`.
// - `*returnColumnSizes` is itself a malloc'd array of `*returnSize` ints.
// - The groups point at the strings in `strs`. They don't copy them.
// The caller frees each group, the outer array, and the sizes array, but not
// the strings. Free your map before returning.
//
// You don't know how many groups there are, or how big each one is, until
// you've seen every string. Work out how you'll deal with that before you
// write any code.
//
// LeetCode's real signature is
// `char*** groupAnagrams(char** strs, int strsSize, int* returnSize, int** returnColumnSizes)`.

#define HASHMAP_NO_MAIN
#include "hashMap.c"

char ***groupAnagrams(char **strs, int strsSize, int *returnSize,
                      int **returnColumnSizes) {
  // TODO
  (void)strs;
  (void)strsSize;
  *returnSize = 0;
  *returnColumnSizes = NULL;
  return NULL;
}

// ---- Tests ----

// Copies `s` into a heap buffer of exactly strlen(s) + 1 bytes, so ASan
// reports any read past the '\0'.
static char *dup(const char *s) {
  size_t n = strlen(s) + 1;
  char *p = malloc(n);
  assert(p != NULL);
  memcpy(p, s, n);
  return p;
}

static bool is_anagram(const char *a, const char *b) {
  int counts[26] = {0};
  for (; *a != '\0'; a++) counts[*a - 'a']++;
  for (; *b != '\0'; b++) counts[*b - 'a']--;
  for (int i = 0; i < 26; i++)
    if (counts[i] != 0) return false;
  return true;
}

static int cmp_ptr(const void *a, const void *b) {
  uintptr_t x = (uintptr_t)*(char *const *)a;
  uintptr_t y = (uintptr_t)*(char *const *)b;
  return (x > y) - (x < y);
}

// Runs groupAnagrams on heap copies of `words` and checks that:
// - there are exactly `groups` groups, none empty,
// - every string in a group is an anagram of the group's first string,
// - the groups hold every input pointer exactly once.
// Together these mean the groups are exactly the anagram classes.
static void check(const char *const *words, int n, int groups) {
  char **strs = malloc((size_t)n * sizeof *strs);
  assert(strs != NULL);
  for (int i = 0; i < n; i++) strs[i] = dup(words[i]);

  int size = -1;
  int *cols = NULL;
  char ***got = groupAnagrams(strs, n, &size, &cols);
  assert(got != NULL);
  assert(cols != NULL);
  assert(size == groups);

  char **seen = malloc((size_t)n * sizeof *seen);
  assert(seen != NULL);
  int total = 0;
  for (int g = 0; g < size; g++) {
    assert(cols[g] > 0);
    for (int j = 0; j < cols[g]; j++) {
      assert(is_anagram(got[g][0], got[g][j]));
      assert(total < n);
      seen[total++] = got[g][j];
    }
  }
  assert(total == n);

  // Same pointers as the input, each exactly once: sort both and compare.
  char **want = malloc((size_t)n * sizeof *want);
  assert(want != NULL);
  memcpy(want, strs, (size_t)n * sizeof *want);
  qsort(seen, (size_t)n, sizeof *seen, cmp_ptr);
  qsort(want, (size_t)n, sizeof *want, cmp_ptr);
  for (int i = 0; i < n; i++) assert(seen[i] == want[i]);

  for (int g = 0; g < size; g++) free(got[g]);
  free(got);
  free(cols);
  free(seen);
  free(want);
  for (int i = 0; i < n; i++) free(strs[i]);
  free(strs);
}

#define CHECK(groups, ...)                                         \
  do {                                                             \
    const char *w[] = {__VA_ARGS__};                               \
    check(w, (int)(sizeof w / sizeof *w), (groups));               \
  } while (0)

int main(void) {
  CHECK(3, "eat", "tea", "tan", "ate", "nat", "bat");
  CHECK(1, "");
  CHECK(1, "a");

  CHECK(1, "", "");                          // empty strings group together
  CHECK(1, "a", "a");                        // duplicates
  CHECK(2, "ab", "a");                       // a prefix isn't an anagram
  CHECK(2, "aab", "abb");                    // same letters, different counts
  CHECK(4, "abc", "def", "ghi", "jkl");      // no anagrams at all
  CHECK(1, "listen", "silent", "enlist", "tinsel", "inlets");
  CHECK(2, "abcdefghijklmnopqrstuvwxyz", "zyxwvutsrqponmlkjihgfedcba", "z");

  // 100-char strings, the longest allowed.
  {
    char a[101], b[101];
    for (int i = 0; i < 100; i++) {
      a[i] = (char)('a' + i % 26);
      b[99 - i] = a[i];
    }
    a[100] = b[100] = '\0';
    CHECK(1, a, b);
  }

  // 10,000 strings in 5,000 pairs: forces the map to resize many times.
  // Pair k is a^x b^y c^z and its reverse, with (x, y, z) unique per k.
  {
    enum { PAIRS = 5000, N = 2 * PAIRS };
    static char buf[N][64];
    static const char *words[N];
    for (int k = 0; k < PAIRS; k++) {
      int counts[3] = {k % 20, (k / 20) % 20, k / 400};
      char *fwd = buf[2 * k], *rev = buf[2 * k + 1];
      int len = 0;
      for (int c = 0; c < 3; c++)
        for (int r = 0; r < counts[c]; r++) fwd[len++] = (char)('a' + c);
      for (int i = 0; i < len; i++) rev[i] = fwd[len - 1 - i];
      fwd[len] = rev[len] = '\0';
      words[2 * k] = fwd;
      words[2 * k + 1] = rev;
    }
    check(words, N, PAIRS);
  }

  return 0;
}
