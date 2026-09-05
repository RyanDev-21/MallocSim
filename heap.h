#ifndef HEAP_H
#define HEAP_H
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_CAP_BYTES 64000
static_assert(HEAP_CAP_BYTES % sizeof(uintptr_t) == 0,
              "The heap cap is not divisible by the pointer size");
#define HEAP_CAP_WORDS (HEAP_CAP_BYTES / sizeof(uintptr_t))
#define HEAP_ALLOC_CAP 1024

#define UNIMPLEMENTED                                                          \
  do {                                                                         \
    fprintf(stderr, "%s,%d ,TODO: %s not yet implmented\n", __FILE__,          \
            __LINE__, __func__);                                               \
  } while (0);

extern uintptr_t heap[HEAP_CAP_WORDS];

typedef struct {
  uintptr_t *start;
  size_t size;
} Chunk;

typedef struct {
  size_t count;
  Chunk chunks[HEAP_ALLOC_CAP]; // has to consider this again
} ChunkList;

extern ChunkList alloc_chunks;
extern ChunkList freed_chunks;

void *heap_alloc(size_t size_bytes);
void alloc_free(void *ptr);
void heap_collect();
void chunk_list_dump(const ChunkList *list);
int comparePTR(const void *a, const void *b);
void chunk_list_insert(ChunkList *list, void *ptr, size_t size);
int chunk_list_findp(const ChunkList *list, void *ptr);
void chunk_list_remove(ChunkList *list, void *ptr);
void chunk_list_dump(const ChunkList *list);
void chunk_list_merge(ChunkList *list);
#endif // HEAP_H
