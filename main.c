#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_CAP 64000
#define HEAP_ALLOC_CAP 1024

#define UNIMPLEMENTED                                                          \
  do {                                                                         \
    fprintf(stderr, "%s,%d ,TODO: %s not yet implmented\n", __FILE__,          \
            __LINE__, __func__);                                               \
  } while (0);

typedef struct {
  void *start;
  size_t size;
} Chunk;

typedef struct {
  size_t count;
  Chunk chunks[HEAP_ALLOC_CAP]; // has to consider this again
} ChunkList;

char heap[HEAP_CAP] = {0};
ChunkList alloc_chunks = {0};
ChunkList freed_chunks = {.chunks = {{.start = heap, .size = HEAP_CAP}},
                          .count = 1};

void chunk_list_insert(ChunkList *list, void *ptr, size_t size) {
  assert(list->count + 1 <= HEAP_ALLOC_CAP);
  Chunk chunk = {.start = ptr, .size = size};
  list->chunks[list->count] = chunk;
  for (int i = list->count;
       i > 0 && list->chunks[i].start < list->chunks[i - 1].start; i--) {
    Chunk t = list->chunks[i];
    list->chunks[i] = list->chunks[i - 1];
    list->chunks[i - 1] = t;
  };
  list->count++;
}
// compare function for bsearch
int comparePTR(const void *a, const void *b) {
  const Chunk *key = a;
  const Chunk *ele = b;
  return (char *)key->start - (char *)ele->start;
}
int chunk_list_find(const ChunkList *list, void *ptr) {
  Chunk key = {
      .start = ptr,
  };
  Chunk *result = bsearch(&key, list->chunks, list->count,
                          sizeof(list->chunks[0]), comparePTR);

  if (result != 0) {
    assert(list->chunks <= result);
    return result - list->chunks;
  } else {
    return -1;
  }
}
void chunk_list_remove(ChunkList *list, void *ptr) {
  int index = chunk_list_find(list, ptr);
  if (index < 0) {
    return;
  }
  assert((size_t)index < list->count);
  for (size_t i = index; i <= list->count - 1; i++) {
    list->chunks[i] = list->chunks[i + 1];
  }
  list->count -= 1;
}

void chunk_list_dump(const ChunkList *list) {
  printf("Chunks(%zu):\n", list->count);
  for (size_t i = 0; i < list->count; i++) {
    printf("start:%p, size: %zu\n", list->chunks[i].start,
           list->chunks[i].size);
  }
}

void chunk_list_merge(ChunkList *list) {
  for (size_t i = 0; i < list->count - 1; i++) {
    Chunk *chunk_1 = list->chunks[i].start;
    Chunk const *chunk_2 = list->chunks[i + 1].start;
    if ((char *)chunk_1->start + chunk_1->size == chunk_2->start) {
      list->chunks[i].size += list->chunks[i + 1].size;
    }
    chunk_list_remove(list, list->chunks[i + 1].start);
  }
}

void *heap_alloc(size_t size) {
  if (size == 0) {
    return NULL;
  }
  chunk_list_merge(&freed_chunks);
  for (size_t i = 0; i < freed_chunks.count; i++) {
    Chunk frag = freed_chunks.chunks[i];
    if (frag.size >= size) {
      chunk_list_remove(&freed_chunks, frag.start);
      chunk_list_insert(&alloc_chunks, frag.start, size);
      const size_t tail_size = frag.size - size;
      if (tail_size > 0) {
        // for remaining space
        chunk_list_insert(&freed_chunks, (char *)frag.start + size, tail_size);
      }
      return frag.start;
    }
  }
  return NULL;
}
void alloc_free(void *ptr) {
  if (ptr != NULL) {
    int index = chunk_list_find(&alloc_chunks, ptr);
    assert(index >= 0);
    chunk_list_insert(&freed_chunks, alloc_chunks.chunks[index].start,
                      alloc_chunks.chunks[index].size);
    chunk_list_remove(&alloc_chunks, ptr);
  }
}

int main() {
  for (int i = 0; i < 10; i++) {
    void *res = heap_alloc(i);
    heap_alloc(240);
    if (i % 2 == 0) {
      alloc_free(res);
    }
  }

  chunk_list_dump(&alloc_chunks);
  chunk_list_dump(&freed_chunks);
  return 0;
}
