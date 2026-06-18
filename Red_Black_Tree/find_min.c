/**
 * @file  find_min.c
 * @brief Returns the leftmost (minimum) node in a BST subtree.
 *        Also used by delete_t() to find the in-order successor.
 */

#include "tree.h"

node_t *find_min(node_t *root)
{
    if (root == NULL)       return NULL;
    if (root->left == NULL) return root;
    return find_min(root->left);
}