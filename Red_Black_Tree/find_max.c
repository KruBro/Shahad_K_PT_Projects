/**
 * @file  find_max.c
 * @brief Returns the rightmost (maximum) node in a BST subtree.
 */

#include "tree.h"

node_t *find_max(node_t *root)
{
    if (root == NULL)        return NULL;
    if (root->right == NULL) return root;
    return find_max(root->right);
}