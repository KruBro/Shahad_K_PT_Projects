/**
 * @file  delete.c
 * @brief Red-Black Tree deletion: BST removal, transplant, and fix-up.
 *
 * The three phases are:
 *   1. Find target node z via search_t().
 *   2. Splice z out (transplant), tracking y (physically removed node)
 *      and x (node that fills y's spot). x may be NULL.
 *   3. If y was BLACK, call delete_fix_up() — removing a BLACK node
 *      creates a "Double Black" violation of Property 4.
 *
 * x_parent is tracked separately throughout because x is frequently
 * NULL (a NIL leaf replacement), making x->parent undefined behavior.
 *
 * See README § Algorithm Deep-Dives for the full 4-case fix-up analysis.
 */

#include "tree.h"

extern int status; /* Declared in main.c. 1 = deleted, 0 = not found. */

/* NULL-safe color read. NULL (NIL leaf) always returns BLACK. */
color_t get_color(node_t *node)
{
    return (node == NULL) ? BLACK : node->color;
}

/* NULL-safe color write. Writing to NIL (NULL) is a deliberate no-op. */
void set_color(node_t *node, color_t color)
{
    if (node != NULL) node->color = color;
}

/* Replaces subtree rooted at u with subtree rooted at v. */
void transplant(node_t **root, node_t *u, node_t *v)
{
    if      (u->parent == NULL)      *root           = v;
    else if (u == u->parent->left)   u->parent->left  = v;
    else                             u->parent->right = v;

    if (v != NULL) v->parent = u->parent;
}

/* Restores Property 4 after a BLACK node is removed.
 * x carries a "double black" credit that must be resolved.
 * w = sibling of x at each iteration. */
void delete_fix_up(node_t **root, node_t *x, node_t *x_parent)
{
    node_t *w;

    while (x != *root && get_color(x) == BLACK) {
        if (x == x_parent->left) {
            w = x_parent->right;

            /* Case 1: sibling is RED — rotate to convert to Case 2/3/4. */
            if (get_color(w) == RED) {
                set_color(w, BLACK);
                set_color(x_parent, RED);
                left_rotation(root, x_parent);
                w = x_parent->right;
            }

            /* Case 2: sibling BLACK, both nephews BLACK — push deficit up. */
            if (get_color(w->left) == BLACK && get_color(w->right) == BLACK) {
                set_color(w, RED);
                x = x_parent;
                x_parent = x->parent;
            } else {
                /* Case 3: sibling BLACK, far nephew BLACK — convert to Case 4. */
                if (get_color(w->right) == BLACK) {
                    set_color(w->left, BLACK);
                    set_color(w, RED);
                    right_rotation(root, w);
                    w = x_parent->right;
                }
                /* Case 4: sibling BLACK, far nephew RED — rotate and done. */
                set_color(w, get_color(x_parent));
                set_color(x_parent, BLACK);
                set_color(w->right, BLACK);
                left_rotation(root, x_parent);
                x = *root;
                break;
            }
        } else {
            /* Mirror: x is a right child. */
            w = x_parent->left;

            /* Case 1 mirror */
            if (get_color(w) == RED) {
                set_color(w, BLACK);
                set_color(x_parent, RED);
                right_rotation(root, x_parent);
                w = x_parent->left;
            }

            /* Case 2 mirror */
            if (get_color(w->right) == BLACK && get_color(w->left) == BLACK) {
                set_color(w, RED);
                x = x_parent;
                x_parent = x->parent;
            } else {
                /* Case 3 mirror */
                if (get_color(w->left) == BLACK) {
                    set_color(w->right, BLACK);
                    set_color(w, RED);
                    left_rotation(root, w);
                    w = x_parent->left;
                }
                /* Case 4 mirror */
                set_color(w, get_color(x_parent));
                set_color(x_parent, BLACK);
                set_color(w->left, BLACK);
                right_rotation(root, x_parent);
                x = *root;
                break;
            }
        }
    }
    set_color(x, BLACK); /* Absorb extra black if x is RED, or reaffirm root. */
}

node_t *delete_t(node_t *root, int key)
{
    if (root == NULL) { status = 0; return NULL; }

    node_t *z = search_t(root, key);
    if (z == NULL) { status = 0; return root; }

    status = 1;

    node_t  *y               = z;
    color_t  y_original_color = y->color;
    node_t  *x, *x_parent;

    /* Case A: z has no left child. */
    if (z->left == NULL) {
        x = z->right; x_parent = z->parent;
        transplant(&root, z, z->right);
    }
    /* Case B: z has no right child. */
    else if (z->right == NULL) {
        x = z->left; x_parent = z->parent;
        transplant(&root, z, z->left);
    }
    /* Case C: z has two children — replace with in-order successor. */
    else {
        y = find_min(z->right); /* Successor: minimum of right subtree. */
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z) {
            x_parent = y; /* Successor is z's direct child. */
        } else {
            x_parent = y->parent;
            transplant(&root, y, y->right); /* Splice successor out. */
            y->right = z->right;
            y->right->parent = y;
        }

        transplant(&root, z, y);   /* Move successor into z's position. */
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;       /* Inherit z's color to preserve black-height. */
    }

    free(z);

    /* Fix-up only needed when a BLACK node was physically removed. */
    if (y_original_color == BLACK)
        delete_fix_up(&root, x, x_parent);

    return root;
}