#include "heap.h"
#include <stdio.h>

typedef struct Node Node;
struct Node {
  char value;
  Node *left;
  Node *right;
};

Node *generate_tree(size_t curr_level, size_t max_level) {
  if (curr_level < max_level) {
    Node *root = heap_alloc(sizeof(*root));
    assert(root->value <= 'z');
    root->value = curr_level + 'a';
    root->left = generate_tree(curr_level + 1, max_level);
    root->right = generate_tree(curr_level + 1, max_level);
    return root;
  }
  return NULL;
}

void print_tree(Node const *root) {
  if (!root) {
    return;
  }
  printf("%c", root->value);
  print_tree(root->left);
  print_tree(root->right);
}

/* int write_file(const char *filename, ) {} */

int main() {
  Node *root = generate_tree(0, 3);
  print_tree(root);
  printf("................\n");
  int count = 0;
  for (size_t i = 0; i < alloc_chunks.count; i++) {
    for (size_t j = 0; j < alloc_chunks.chunks[i].size; j++) {
      uintptr_t *p = (uintptr_t *)alloc_chunks.chunks[i].start[j];
      if (heap <= p && p < heap + HEAP_CAP_WORDS) {
        count++;
      };
    }
  }
  printf("count of allocated heap:%d\n", count);

  return 0;
}
