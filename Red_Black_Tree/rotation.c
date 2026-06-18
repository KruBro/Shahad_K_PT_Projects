/**
 * @file  rotation.c
 * @brief Left/right rotations and the LL/RR/LR/RL insert-fix router.
 *
 * Rotations are pure pointer swaps — they are colorblind by design.
 * Only rotate() (the router) handles recoloring, because the correct
 * color swap depends on the structural shape, not the rotation direction.
 *
 * See README § Algorithm Deep-Dives for full case diagrams.
 */

#include "tree.h"

/**
 * Left rotation around x.
 *
 *     x                y
 *    / \      →       / \
 *   A   y            x   C
 *      / \          / \
 *     B   C        A   B
 */
void left_rotation(node_t **root, node_t *x)
{
    if (x == NULL || x->right == NULL) return;

    node_t *y = x->right;

    x->right = y->left;                     /* B re-parents to x. */
    if (y->left) y->left->parent = x;

    y->parent = x->parent;                  /* y takes x's place. */
    if      (!x->parent)          *root          = y;
    else if (x == x->parent->left) x->parent->left  = y;
    else                           x->parent->right = y;

    y->left = x;                            /* x descends under y. */
    x->parent = y;
}

/**
 * Right rotation around x (mirror of left_rotation).
 *
 *       x              y
 *      / \    →       / \
 *     y   C          A   x
 *    / \                / \
 *   A   B              B   C
 */
void right_rotation(node_t **root, node_t *x)
{
    if (x == NULL || x->left == NULL) return;

    node_t *y = x->left;

    x->left = y->right;                     /* B re-parents to x. */
    if (y->right) y->right->parent = x;

    y->parent = x->parent;                  /* y takes x's place. */
    if      (!x->parent)           *root          = y;
    else if (x == x->parent->left)  x->parent->left  = y;
    else                            x->parent->right = y;

    y->right = x;                           /* x descends under y. */
    x->parent = y;
}

/* Routes an insert imbalance to the correct rotation(s) and recoloring.
 * n = new node, p = parent, g = grandparent. */
void rotate(node_t **root, node_t *n)
{
    node_t *p = n->parent;
    if (p == NULL || p->parent == NULL) return;
    node_t *g = p->parent;

    /* LL — single right rotation on g. */
    if (n == p->left && p == g->left) {
        right_rotation(root, g);
        g->color = recolor(g);
        p->color = recolor(p);
    }
    /* RR — single left rotation on g. */
    else if (n == p->right && p == g->right) {
        left_rotation(root, g);
        g->color = recolor(g);
        p->color = recolor(p);
    }
    /* LR — left on p to reduce to LL, then right on g. */
    else if (n == p->right && p == g->left) {
        left_rotation(root, p);
        right_rotation(root, g);
        n->color = recolor(n);
        g->color = recolor(g);
    }
    /* RL — right on p to reduce to RR, then left on g. */
    else if (n == p->left && p == g->right) {
        right_rotation(root, p);
        left_rotation(root, g);
        n->color = recolor(n);
        g->color = recolor(g);
    }
}