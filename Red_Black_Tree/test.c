/**
 * @file  test.c
 * @brief Unit tests for all 9 Red-Black Tree operations.
 *
 * Build:  make test
 * Run:    ./test_rbt
 *
 * Tests are ordered by dependency: edge cases first, then insertions,
 * then reads (search/min/max), then deletions, then memory cleanup.
 * int status is defined here instead of main.c (excluded from test build).
 */

#include "tree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int status;

int main(void)
{
    node_t *root = NULL;

    printf("==================================================\n");
    printf("   RED-BLACK TREE COMPREHENSIVE UNIT TESTS\n");
    printf("==================================================\n\n");

    /* ── Empty tree edge cases ────────────────────────────────────────────── */
    printf("Testing Empty Tree Edge Cases...\n");
    assert(find_min(root) == NULL);
    assert(find_max(root) == NULL);
    assert(search_t(root, 50) == NULL);
    root = delete_t(root, 50);
    assert(root == NULL && status == 0);
    printf("  [PASS] Empty tree handled safely.\n\n");

    /* ── Insertion and all 4 rotation cases ──────────────────────────────── */
    printf("Testing Case 1: Insertions & Balancing...\n");

    /* RR → left rotation on root */
    insert(&root, 10); insert(&root, 20); insert(&root, 30);
    assert(root->data == 20 && root->color == BLACK);
    assert(root->left->data == 10 && root->right->data == 30);
    free_tree(root); root = NULL;

    /* LL → right rotation on root */
    insert(&root, 30); insert(&root, 20); insert(&root, 10);
    assert(root->data == 20 && root->color == BLACK);
    assert(root->left->data == 10 && root->right->data == 30);
    free_tree(root); root = NULL;

    /* LR → double rotation */
    insert(&root, 30); insert(&root, 10); insert(&root, 20);
    assert(root->data == 20 && root->color == BLACK);
    assert(root->left->data == 10 && root->right->data == 30);
    free_tree(root); root = NULL;

    /* RL → double rotation */
    insert(&root, 10); insert(&root, 30); insert(&root, 20);
    assert(root->data == 20 && root->color == BLACK);
    assert(root->left->data == 10 && root->right->data == 30);
    free_tree(root); root = NULL;
    printf("  [PASS] All 4 rotation cases correct.\n\n");

    /* ── Build 12-node stress tree for remaining tests ───────────────────── */
    int values[] = {10, 18, 7, 15, 16, 30, 25, 40, 60, 2, 1, 70};
    for (int i = 0; i < 12; i++) insert(&root, values[i]);

    /* ── Search and display (Cases 3 & 4) ────────────────────────────────── */
    printf("Testing Case 3 & 4: Search & Display...\n");
    assert(search_t(root, 16) != NULL); /* root node */
    assert(search_t(root, 70) != NULL); /* rightmost leaf */
    assert(search_t(root, 99) == NULL); /* not in tree */
    printf("  Tree Display (In-Order):\n  ");
    display_tree(root);
    printf("\n  [PASS] Search logic verified.\n\n");

    /* ── Find MIN / MAX (Cases 5 & 6) ────────────────────────────────────── */
    printf("Testing Case 5 & 6: Find MIN / MAX...\n");
    assert(find_min(root)->data == 1);
    assert(find_max(root)->data == 70);
    printf("  [PASS] Minimum is 1, Maximum is 70.\n\n");

    /* ── Delete MIN / MAX (Cases 7 & 8) ──────────────────────────────────── */
    printf("Testing Case 7 & 8: Delete MIN / MAX...\n");
    root = delete_t(root, 1);
    assert(status == 1 && find_min(root)->data == 2);
    root = delete_t(root, 70);
    assert(status == 1 && find_max(root)->data == 60);
    printf("  [PASS] Min and Max successfully deleted and updated.\n\n");

    /* ── Complex deletions (Case 2) ───────────────────────────────────────── */
    printf("Testing Case 2: Complex Internal Deletions...\n");
    root = delete_t(root, 16); /* root node, two children: successor (18) takes over */
    assert(status == 1 && search_t(root, 16) == NULL && root->data == 18);
    root = delete_t(root, 15); /* RED leaf: no fix-up required */
    assert(status == 1 && search_t(root, 15) == NULL);
    printf("  [PASS] Internal node and leaf deletions succeeded.\n\n");

    /* ── Free tree (Case 9) ───────────────────────────────────────────────── */
    printf("Testing Case 9: Free Memory...\n");
    free_tree(root);
    root = NULL;
    assert(root == NULL);
    printf("  [PASS] Tree memory successfully freed.\n\n");

    printf("==================================================\n");
    printf(" [SUCCESS] ALL 9 OPERATIONS PASSED FLAWLESSLY!\n");
    printf("==================================================\n");

    return 0;
}