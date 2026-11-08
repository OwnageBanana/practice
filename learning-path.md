# C learning path

Sequenced so the algorithm work forces the machine-level concepts instead of postponing them. Assumes you can already program and want C specifically for the memory model.

## Setup and the memory model

Toolchain on Fedora:

```
sudo dnf install gcc clang gdb valgrind make perf
```

Compile everything during development with sanitizers on:

```
gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -g -fsanitize=address,undefined -o main.out main.c
```

ASan plus UBSan catches out-of-bounds, use-after-free, leaks, and signed overflow at the bug site rather than three calls later. Keep a second unsanitized build for valgrind and perf. Learn gdb the first time you segfault: `break`, `run`, `bt`, `frame`, `print`, `watch`. Never debug with printf.

Repo layout: one `.c` per problem, a Makefile, and a `main()` per file running assert-based tests. Exercism's C track is an alternative with a prebuilt Unity test harness.

### Reading

- Modern C by Jens Gustedt, levels 0 to 2. Free from the author: https://gustedt.gitlabpages.inria.fr/modern-c/
- K&R chapters 5 and 6 for pointers and structs.
- Beej's Guide to C as a readable backup: https://beej.us/guide/bgc/
- Effective C by Robert Seacord for a stricter modern treatment.

### What JS hides that you need to internalize

- Value semantics everywhere. Structs copy on assignment and on pass. A pointer is an integer holding an address.
- Stack vs heap. Automatics die at scope end. malloc/free is ownership, and you decide who frees.
- Arrays decay to pointers. `sizeof` on a parameter gives pointer size. Length always travels separately.
- Strings are char arrays with a NUL terminator. strlen is O(n). Buffer sizes include the terminator.
- Integers are fixed width with promotion rules. Signed overflow is undefined, unsigned wraps. Use stdint types and size_t.
- Struct layout and padding. Print sizeof and offsetof on a few structs, then reorder fields.
- Translation units, headers, static vs extern, what the linker does. Build a two-file program with `gcc -c` then link by hand.
- Undefined behavior means the compiler assumes it never happens. Read the LLVM blog series "What Every C Programmer Should Know About Undefined Behavior".

## Algorithms

LeetCode accepts C. Its harness expects malloc'd return arrays with the length written through an `int* returnSize` out-param, and the caller frees. That convention alone teaches ownership.

There is no container library, so you build the containers. That is the point.

1. Arrays and strings, no containers. Reverse string in place, remove duplicates from sorted array, move zeroes, rotate array, max subarray, valid palindrome, atoi with proper overflow handling, implement strstr.
   - **Outcomes:** Pointer and index arithmetic, the NUL terminator and buffer sizes, in-place mutation, two-pointer loops and their `<` vs `<=` boundaries, `strlen` and `size_t`, and detecting integer overflow *before* it happens.
2. Dynamic array. realloc, doubling growth, len and cap. String builder on top. Reuse both from here on.
   - **Outcomes:** Heap ownership, `realloc` semantics (the block can move or fail, so assign through a temp), amortized O(1) growth, and designing a small struct-plus-functions API.
3. Hash map. Open addressing, linear probing, FNV-1a, tombstones, resize at 0.7 load. Then two sum, group anagrams, longest substring without repeats, top k frequent.
   - **Outcomes:** Hashing, open addressing and why deletion needs tombstones, who owns key strings (copy or borrow), and rehashing on resize.
4. Linked lists. Reverse, merge sorted, middle, Floyd cycle detection, remove nth from end, add two numbers. Then LRU cache, which combines your hash map and a doubly linked list and is the best single exercise here.
   - **Outcomes:** Pointer-to-pointer (`Node **`) to insert and remove without special-casing the head, per-node malloc/free, freeing a whole structure, and combining two structures into one (LRU).
5. Stack and queue. Ring-buffer queue. Valid parentheses, min stack, evaluate RPN, daily temperatures, sliding window max.
   - **Outcomes:** Ring buffers with modular indexing, telling full from empty, and the monotonic stack/deque pattern.
6. Binary search variants. First/last occurrence, rotated array, 2D matrix, integer sqrt, koko eating bananas.
   - **Outcomes:** Loop invariants, choosing `<` vs `<=` and `mid` vs `mid ± 1`, overflow-safe midpoints (`lo + (hi - lo) / 2`), and unsigned underflow.
7. Sorting. Insertion, merge, quick with median of three, heap sort. Build a reusable min-heap. Compare with qsort and its function-pointer comparator. Kth largest, merge k sorted lists.
   - **Outcomes:** Recursion and stack depth, scratch buffers, heap index math in a flat array, function pointers with `const void *` (qsort comparators), and stability.
8. Trees. BST insert/search/delete, recursive and iterative traversals with an explicit stack, max depth, invert, validate, level order with your queue, lowest common ancestor, serialize/deserialize. Trie for word problems.
   - **Outcomes:** Recursive structs, recursion vs an explicit stack, postorder freeing, serializing to a buffer, and a trie's memory cost.
9. Graphs. Adjacency list in CSR layout, one offsets array plus one edges array, which is how real graph code is laid out. BFS, DFS, number of islands, course schedule, union-find with path compression, Dijkstra with your heap.
   - **Outcomes:** CSR layout and cache locality, visited arrays, union-find, and a priority queue with lazy deletion for Dijkstra.
10. Dynamic programming. Climbing stairs, house robber, coin change, LIS, LCS, edit distance, knapsack, unique paths. Lay 2D tables out as one flat allocation indexed `row * cols + col` and note why that beats an array of pointers.
    - **Outcomes:** Row-major flat 2D arrays, `calloc` for zeroed tables, time vs memory trade-offs, and rolling rows to cut memory.
11. Bits. Count bits, single number, power of two, reverse bits, subsets via bitmask. Implement a bitset.
    - **Outcomes:** Bitwise operators, two's complement, signed vs unsigned shifts, masks, and compiler builtins like `__builtin_popcount`.

After each, paste your solution into Compiler Explorer (https://godbolt.org) at `-O2`. You'll start seeing what the compiler does with loops, when it inlines, and when pointer aliasing blocks vectorization.

Further reading for this phase:

- C Interfaces and Implementations by David Hanson, for designing C data-structure APIs.
- Sean Barrett's stb headers (https://github.com/nothings/stb), idiomatic single-file C to read.

## Down to the machine

Computer Systems: A Programmer's Perspective by Bryant and O'Hallaron is the book for the stated goal. Chapters 2, 3, 5, 6, 9. The self-study labs are free at https://csapp.cs.cmu.edu/3e/labs.html and are the best part:

- Data Lab. Bit puzzles under operator constraints.
- Bomb Lab. Defuse a binary with gdb and objdump. Teaches reading x86-64.
- Attack Lab. Buffer overflows and ROP. Teaches the stack for real.
- Cache Lab. Write a cache simulator, then optimize a matrix transpose for it.
- Malloc Lab. Write malloc/free/realloc. Alignment, fragmentation, free lists.

Measure alongside:

```
perf stat -e cache-misses,instructions,cycles ./prog
```

Run that on your sorts and matrix code. Swap loop order in a matrix multiply and watch runtime change by an order of magnitude. Compare array-of-structs vs struct-of-arrays. Read Drepper's "What Every Programmer Should Know About Memory", sections 3 and 6 at minimum.

Then one larger project to consolidate, pick one:

- clox from Crafting Interpreters part 3 (https://craftinginterpreters.com). Bytecode VM with a GC, in C. Best fit for the goal.
- kilo from the snaptoken tutorial (https://viewsourcecode.org/snaptoken/kilo/). A text editor in about a thousand lines, raw terminal mode.
- A shell. fork, exec, pipe, dup2, wait.
- An HTTP server from Beej's Guide to Network Programming (https://beej.us/guide/bgnet/).

If you want to keep going after that: xv6 from MIT 6.1810 is a real kernel you can read in a weekend and modify over months. Nand2Tetris gives the bottom-up hardware view. Either closes the loop.

## Habits

These matter more than reading order.

- Sanitizers on every dev build. Valgrind on the unsanitized build before calling anything done.
- gdb for every crash. Learn `bt`, `frame n`, `p *ptr`, `x/16xb addr`, `info registers`.
- Every allocating function has a name or comment saying who frees.
- Check the return of malloc, fopen, and every syscall. Fail fast.
- `man 3 func` before a web search. Section 3 is libc, section 2 is syscalls.
