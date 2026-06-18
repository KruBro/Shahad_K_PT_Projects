/**
 * @file  main.c
 * @brief Interactive terminal menu for the Red-Black Tree (9 operations).
 *
 * global `status`: written by delete_t() to signal found(1)/not-found(0).
 * Cases 7 & 8 save the target value before deletion to avoid use-after-free.
 */

#include "tree.h"
#include "color.h"

int status; /* extern'd in delete.c */

static void print_menu(void)
{
    printf(CLR_DIVIDER "======================================\n" COLOR_RESET);
    printf(CLR_TITLE   "      RED-BLACK TREE  -  MAIN MENU   \n" COLOR_RESET);
    printf(CLR_DIVIDER "======================================\n" COLOR_RESET);
    const char *opts[] = {
        "  1. Insert",    "  2. Delete",  "  3. Search",
        "  4. Display",   "  5. Find MIN","  6. Find MAX",
        "  7. Delete MIN","  8. Delete MAX","  9. Exit"
    };
    for (int i = 0; i < 9; i++)
        printf(CLR_OPTION "%s\n" COLOR_RESET, opts[i]);
    printf(CLR_DIVIDER "======================================\n" COLOR_RESET);
    printf(CLR_PROMPT  "Enter your choice: " COLOR_RESET);
}

int main(void)
{
    node_t *root = NULL;
    int choice;

    do {
        printf("\n");
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int data;
                printf(CLR_PROMPT "Enter the data: " COLOR_RESET);
                scanf("%d", &data);
                if (insert(&root, data) == SUCCESS)
                    printf(CLR_SUCCESS "[INFO] : %d inserted successfully.\n" COLOR_RESET, data);
                else
                    printf(CLR_ERROR  "[INFO] : Insert failed (duplicate or memory error).\n" COLOR_RESET);
                break;
            }
            case 2: {
                int key;
                printf(CLR_PROMPT "Enter the data to delete: " COLOR_RESET);
                scanf("%d", &key);
                if (root) {
                    root = delete_t(root, key);
                    if (status) printf(CLR_SUCCESS "[INFO] : Element deleted successfully.\n" COLOR_RESET);
                    else        printf(CLR_ERROR   "[INFO] : Element not found.\n" COLOR_RESET);
                } else {
                    printf(CLR_ERROR "[INFO] : Tree is empty.\n" COLOR_RESET);
                }
                break;
            }
            case 3: {
                int key;
                printf(CLR_PROMPT "Enter the data to search: " COLOR_RESET);
                scanf("%d", &key);
                if (search_t(root, key))
                    printf(CLR_SUCCESS "[INFO] : %d found in the tree.\n" COLOR_RESET, key);
                else
                    printf(CLR_ERROR  "[INFO] : %d not found.\n" COLOR_RESET, key);
                break;
            }
            case 4:
                printf(CLR_INFO "\n[Tree -- In-Order Traversal]\n" COLOR_RESET);
                root ? display_tree(root) : printf(CLR_ERROR "(empty tree)\n" COLOR_RESET);
                printf("\n");
                break;
            case 5: {
                node_t *min = find_min(root);
                min ? printf(CLR_SUCCESS "[INFO] : MIN = %d\n" COLOR_RESET, min->data)
                    : printf(CLR_ERROR   "[INFO] : Tree is empty.\n" COLOR_RESET);
                break;
            }
            case 6: {
                node_t *max = find_max(root);
                max ? printf(CLR_SUCCESS "[INFO] : MAX = %d\n" COLOR_RESET, max->data)
                    : printf(CLR_ERROR   "[INFO] : Tree is empty.\n" COLOR_RESET);
                break;
            }
            case 7: {
                node_t *min = find_min(root);
                if (min) {
                    int val = min->data; /* Save before delete_t frees the node. */
                    root = delete_t(root, val);
                    printf(CLR_SUCCESS "[INFO] : MIN node (%d) deleted.\n" COLOR_RESET, val);
                } else {
                    printf(CLR_ERROR "[INFO] : Tree is empty.\n" COLOR_RESET);
                }
                break;
            }
            case 8: {
                node_t *max = find_max(root);
                if (max) {
                    int val = max->data; /* Save before delete_t frees the node. */
                    root = delete_t(root, val);
                    printf(CLR_SUCCESS "[INFO] : MAX node (%d) deleted.\n" COLOR_RESET, val);
                } else {
                    printf(CLR_ERROR "[INFO] : Tree is empty.\n" COLOR_RESET);
                }
                break;
            }
            case 9:
                free_tree(root);
                printf(CLR_SUCCESS "\n[INFO] : Memory freed. Goodbye!\n\n" COLOR_RESET);
                break;
            default:
                printf(CLR_ERROR "[ERROR]: Invalid option. Choose 1-9.\n" COLOR_RESET);
        }
    } while (choice != 9);

    return 0;
}