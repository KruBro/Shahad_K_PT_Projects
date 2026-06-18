/**
 * @file  display.c
 * @brief In-order (ascending) traversal with ANSI color-coded node output.
 */

#include "tree.h"
#include "color.h"

void display_tree(node_t *root)
{
    if (root == NULL) return;

    display_tree(root->left);

    if (root->color == RED)
        printf(CLR_NODE_RED   "(%d)--(RED->0)"   COLOR_RESET "  ", root->data);
    else
        printf(CLR_NODE_BLACK "(%d)--(BLACK->1)" COLOR_RESET "  ", root->data);

    display_tree(root->right);
}