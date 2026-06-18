/**
 * @file  tree.h
 * @brief Central header: types, structs, and function prototypes.
 *
 * Red-Black Tree invariants (all balancing code exists to restore these):
 *   1. Every node is RED or BLACK.
 *   2. Root is always BLACK.
 *   3. No two consecutive RED nodes on any path (no RED parent + RED child).
 *   4. Every path from a node to a NIL leaf has the same number of BLACK nodes.
 *
 * NULL pointers represent NIL sentinel leaves and are always treated as BLACK.
 * See get_color() in delete.c for the safe NULL-tolerant color reader.
 */

#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>

typedef enum { RED = 0, BLACK = 1 } color_t;

typedef enum { SUCCESS, FAILURE, DATA_NOT_FOUND, TREE_EMPTY } Status;

typedef struct node {
    int         data;
    color_t     color;
    struct node *left;
    struct node *right;
    struct node *parent;
} node_t;

/* display.c */
void    display_tree(node_t *root);

/* insertion.c */
Status  insert(node_t **root, int data);
void    balance_t(node_t **root, node_t *new_node);

/* search.c */
node_t *search_t(node_t *root, int key);

/* find_min.c */
node_t *find_min(node_t *root);

/* find_max.c */
node_t *find_max(node_t *root);

/* tree_utils.c */
color_t recolor(node_t *node);
void    free_tree(node_t *root);

/* delete.c */
color_t get_color(node_t *node);
void    set_color(node_t *node, color_t color);
void    transplant(node_t **root, node_t *u, node_t *v);
node_t *delete_t(node_t *root, int key);
void    delete_fix_up(node_t **root, node_t *x, node_t *x_parent);

/* rotation.c */
void    left_rotation(node_t **root, node_t *x);
void    right_rotation(node_t **root, node_t *x);
void    rotate(node_t **root, node_t *n);

#endif /* TREE_H */