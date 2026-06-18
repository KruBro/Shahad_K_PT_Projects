/**
 * @file  search.c
 * @brief Recursive BST key lookup. Color is irrelevant — only key order matters.
 */

#include "tree.h"

node_t *search_t(node_t *root, int key)
{
    if (root == NULL || root->data == key)
        return root;

    return (key > root->data)
        ? search_t(root->right, key)
        : search_t(root->left,  key);
}