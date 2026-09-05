#include "heap.h"

uintptr_t heap[HEAP_CAP_WORDS] = {0};

ChunkList alloc_chunks = {0};
ChunkList freed_chunks = {.chunks = {{.start = heap, .size = HEAP_CAP_WORDS}},
                          .count = 1};

// WARNING:have to check wether need to change the size to the pointer_size or
// not
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
  if (key->start > ele->start)
    return 1;
  if (key->start < ele->start)
    return -1;
  return 0;
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
  for (size_t i = index; i < list->count - 1; i++) {
    list->chunks[i] = list->chunks[i + 1];
  }
  list->count -= 1;
}

void chunk_list_dump(const ChunkList *list) {
  printf("Chunks(%zu):\n", list->count);
  for (size_t i = 0; i < list->count; i++) {
    printf("start:%p, size: %zu\n", (void *)list->chunks[i].start,
           list->chunks[i].size);
  }
}
// somehow the unimgable things are happening here
void chunk_list_merge(ChunkList *list) {
  if (list->count < 2) {
    return;
  }
  size_t write_idx = 0;
  for (size_t i = 1; i < list->count; i++) {
    Chunk *curr = &list->chunks[write_idx];
    Chunk const *next = &list->chunks[i];
    if (curr->start + curr->size == next->start) {
      curr->size += next->size;
    } else {
      write_idx++;
      // to avoid calling the remove again
      list->chunks[write_idx] = *next;
    }
  }
  list->count = write_idx + 1;
}

// WARNING:have to check wether need to change the size to the pointer_size or
// not
void *heap_alloc(size_t size_bytes) {
  uintptr_t size_words = (size_bytes + sizeof(void *) - 1) / sizeof(void *);
  if (size_words == 0) {
    return NULL;
  }
  for (size_t i = 0; i < freed_chunks.count; i++) {
    Chunk frag = freed_chunks.chunks[i];
    if (frag.size >= size_words) {
      chunk_list_remove(&freed_chunks, frag.start);
      chunk_list_insert(&alloc_chunks, frag.start, size_words);
      const size_t tail_size = frag.size - size_words;
      if (tail_size > 0) {
        // for remaining space
        chunk_list_insert(&freed_chunks, frag.start + size_words, tail_size);
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
    chunk_list_merge(&freed_chunks);
  }
}
