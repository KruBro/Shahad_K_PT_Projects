/**
 * @file  tree_utils.c
 * @brief Shared utilities: recolor() toggle and post-order free_tree().
 */

#include "tree.h"

/* Returns the opposite color. Does NOT mutate the node — caller must assign.
 * Example: uncle->color = recolor(uncle); */
color_t recolor(node_t *node)
{
    return (node->color == RED) ? BLACK : RED;
}

/* Post-order traversal: children freed before parent to avoid losing pointers. */
void free_tree(node_t *root)
{
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}