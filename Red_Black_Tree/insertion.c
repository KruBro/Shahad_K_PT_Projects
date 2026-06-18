/**
 * @file  insertion.c
 * @brief BST insertion + Red-Black rebalancing.
 *
 * insert()    — standard iterative BST insert, then calls balance_t().
 * balance_t() — resolves Red-Red violations by recoloring or rotating.
 *
 * See README § Algorithm Deep-Dives for the full case analysis.
 */

#include "tree.h"

Status insert(node_t **root, int data)
{
    if (root == NULL)
        return FAILURE;

    node_t *new = malloc(sizeof(node_t));
    if (new == NULL)
        return FAILURE;

    new->data   = data;
    new->left   = NULL;
    new->right  = NULL;
    new->parent = NULL;
    new->color  = RED; /* New nodes start RED to preserve black-height (Property 4). */

    /* Empty tree: new node becomes the BLACK root. */
    if (*root == NULL) {
        new->color = BLACK;
        *root = new;
        return SUCCESS;
    }

    /* Standard BST descent to find the insertion point. */
    node_t *temp = *root, *prev = NULL;
    while (temp) {
        prev = temp;
        if      (data < temp->data) temp = temp->left;
        else if (data > temp->data) temp = temp->right;
        else { free(new); return FAILURE; } /* Duplicate key. */
    }

    new->parent = prev;
    if (data < prev->data) prev->left  = new;
    else                   prev->right = new;

    balance_t(root, new);
    return SUCCESS;
}

void balance_t(node_t **root, node_t *new_node)
{
    /* Base cases: root reached, or no Red-Red violation. */
    if (new_node->parent == NULL)          { new_node->color = BLACK; return; }
    if (new_node->parent->color == BLACK)  return;
    if (new_node->parent->parent == NULL)  return;

    node_t *parent      = new_node->parent;
    node_t *grand       = parent->parent;
    node_t *uncle       = (parent == grand->left) ? grand->right : grand->left;

    /* Uncle RED → recolor and recurse upward. */
    if (uncle != NULL && uncle->color == RED) {
        uncle->color  = recolor(uncle);
        grand->color  = recolor(grand);
        parent->color = recolor(parent);
        balance_t(root, grand);
    }
    /* Uncle BLACK → rotate to fix the violation in place. */
    else {
        rotate(root, new_node);
        (*root)->color = BLACK; /* Rotation may have elevated a RED node to root. */
    }
}