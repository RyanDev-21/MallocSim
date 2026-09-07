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
  stackAddr = (const uintptr_t *)__builtin_frame_address(0);
  for (int i = 0; i < 10; i++) {
    heap_alloc(i);
  }
  Node *root = generate_tree(0, 3);
  print_tree(root);
  printf("\nroot pointer value:%p\n", (void *)root);
  printf("\n");
  heap_collect();
  root = NULL;
  heap_collect();
  printf("\n-----------------\n");
  return 0;
}
