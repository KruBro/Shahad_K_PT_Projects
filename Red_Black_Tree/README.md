# Emertxe Red-Black Tree

A robust, menu-driven implementation of a self-balancing Red-Black Tree written in C. Built as part of the Emertxe Training Project, it strictly upholds all four Red-Black properties to guarantee **O(log n)** time complexity for insertion, deletion, and search.

> **On AI involvement:** This README, the CONTRIBUTING guide, SECURITY policy, and GitHub templates were drafted with Claude (Anthropic). The C implementation, algorithmic design, and debugging were done by the project authors. AI was used as a writing tool — not as the engineer.

---

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [Red-Black Tree Properties](#red-black-tree-properties)
- [Installation & Build](#installation--build)
- [Usage](#usage)
- [Algorithm Deep-Dives](#algorithm-deep-dives)
- [Testing](#testing)
- [Development Challenges](#development-challenges)
- [Performance](#performance)
- [Contributing](#contributing)
- [License](#license)

---

## Features

| # | Operation    | Description                                                    |
|---|--------------|----------------------------------------------------------------|
| 1 | Insert       | BST insert + automatic RB property restoration                 |
| 2 | Delete       | BST delete with transplant + Double Black fix-up               |
| 3 | Search       | Recursive key lookup                                           |
| 4 | Display      | In-order traversal with ANSI color-coded output                |
| 5 | Find MIN     | Leftmost node traversal                                        |
| 6 | Find MAX     | Rightmost node traversal                                       |
| 7 | Delete MIN   | Find + delete the minimum element                              |
| 8 | Delete MAX   | Find + delete the maximum element                              |
| 9 | Exit         | Frees all heap memory before quitting                          |

---

## Project Structure

```
red-black-tree/
├── tree.h          — Structs, enums, and all function prototypes
├── color.h         — ANSI escape code macros (two-layer: raw + semantic)
├── main.c          — Interactive menu and entry point
├── insertion.c     — BST insert + balance_t (Red-Red violation fix)
├── delete.c        — BST delete + transplant + delete_fix_up
├── rotation.c      — left_rotation, right_rotation, and LL/RR/LR/RL router
├── search.c        — Recursive search_t
├── find_min.c      — Leftmost node traversal
├── find_max.c      — Rightmost node traversal
├── display.c       — In-order traversal with ANSI color output
├── tree_utils.c    — recolor(), free_tree()
├── test.c          — Comprehensive unit test suite
└── makefile        — Targets: all, test, clean
```

---

## Red-Black Tree Properties

All balancing code in this project exists solely to restore these four properties after every insert and delete:

1. **Node Color** — Every node is either RED or BLACK.
2. **Root is BLACK** — After any rotation or recoloring, the root is explicitly reset to BLACK.
3. **No Red-Red** — A RED node may not have a RED parent. This is what insertion balancing repairs.
4. **Uniform Black-Height** — Every path from any node to a NIL leaf contains the same number of BLACK nodes. Deletion fix-up exists to restore this.

**NIL leaves** are represented as `NULL` pointers and are always treated as BLACK. The `get_color()` wrapper in `delete.c` enforces this: `get_color(NULL)` returns BLACK without a segfault.

---

## Installation & Build

### Prerequisites

- GCC (or any C99-compliant compiler)
- GNU Make
- A POSIX-compatible terminal (for ANSI color output)

```bash
git clone https://github.com/your-username/red-black-tree.git
cd red-black-tree
make all   # builds ./rbt
make test  # builds ./test_rbt
make clean # removes all .o files and binaries
```

---

## Usage

```
======================================
      RED-BLACK TREE  -  MAIN MENU
======================================
  1. Insert
  ...
  9. Exit
======================================
Enter your choice:
```

Sample display output after inserting `{10, 15, 20, 25, 30}`:

```
(10)--(BLACK->1)  (15)--(RED->0)  (20)--(BLACK->1)  (25)--(RED->0)  (30)--(BLACK->1)
```

RED nodes print in bold red; BLACK nodes in gray. `COLOR_RESET` is applied after every node so the prompt is unaffected.

---

## Algorithm Deep-Dives

### Insertion — `insertion.c` + `rotation.c`

**insert()** performs a standard iterative BST descent to find the insertion point, then links the new node and calls `balance_t()`. New nodes start as RED — this is deliberate. Inserting RED never changes the black-height of any path (Property 4 is safe), though it may create a Red-Red violation (Property 3), which `balance_t()` then resolves.

**balance_t()** walks upward from the new node. At each step it checks the uncle's color:

- **Uncle RED → Recolor.** Flip parent, uncle, and grandparent colors. The grandparent may now violate Property 3 with its own parent, so recurse upward from the grandparent.
- **Uncle BLACK (or NULL) → Rotate.** Call `rotate()` which determines the structural shape and dispatches:

| Shape | Structure | Fix |
|-------|-----------|-----|
| LL | n is left child of left child of grandparent | Single right rotation on grandparent |
| RR | n is right child of right child of grandparent | Single left rotation on grandparent |
| LR | n is right child of left child of grandparent | Left rotation on parent → right rotation on grandparent |
| RL | n is left child of right child of grandparent | Right rotation on parent → left rotation on grandparent |

After a rotation, the node that rises to the grandparent position takes BLACK; the demoted grandparent takes RED. For double rotations (LR/RL), the new node itself rises, so it takes BLACK.

---

### Deletion — `delete.c`

Deletion is the most complex operation. It has three phases:

**Phase 1 — Find:** locate target node `z` via `search_t()`.

**Phase 2 — Identify y and x:**
- If z has 0 or 1 child: `y = z`, `x = z's only child (or NULL)`.
- If z has 2 children: `y = find_min(z->right)` (the in-order successor). `y` has at most a right child. `x = y->right`.

**Phase 3 — Transplant:**
- If z has no left child: transplant `x` (z's right) into z's position.
- If z has no right child: transplant `x` (z's left) into z's position.
- If z has two children: splice `y` out of its current position, move `y` into z's position, and copy z's color to `y`. This color copy is critical — it preserves the black-height through y's new position.

**Phase 4 — Conditional Fix-Up:** If `y`'s original color was BLACK, a "Double Black" violation exists. Call `delete_fix_up()`.

---

### The Double Black Fix-Up — `delete_fix_up()`

Removing a BLACK node reduces the black-count on every path through it by 1. Conceptually, `x` carries an extra BLACK credit ("double black") that must be resolved. The fix-up loop runs until `x` reaches the root (credit is dropped) or `x` is RED (credit is absorbed by painting it BLACK).

At each step, let `w` be the sibling of `x`:

**Case 1 — w is RED.**
Rotate toward x at x's parent, recolor. This transforms into Case 2, 3, or 4 with a new (BLACK) sibling.

**Case 2 — w is BLACK, both nephews BLACK.**
Recolor `w` to RED. Push the double-black debt up to x's parent. The loop continues upward.

**Case 3 — w is BLACK, far nephew BLACK, near nephew RED.**
Rotate away from x at `w`, recolor. Converts into Case 4.

**Case 4 — w is BLACK, far nephew RED.**
Rotate toward x at x's parent, recolor. The rotation absorbs x's extra black in one step. Loop exits.

Each case has a mirror image for when `x` is a right child.

---

### The x_parent Tracking Problem

Inside `delete_fix_up()`, accessing the sibling requires `x->parent`. But `x` is frequently `NULL` (when the deleted node had no children, its replacement is a NIL leaf). `NULL->parent` is undefined behavior. The solution is to track `x_parent` as a separate pointer throughout the entire deletion routine — always kept valid even when `x` is `NULL`.

---

### Rotations — `rotation.c`

`left_rotation` and `right_rotation` are pure pointer swaps. They change tree shape without changing the in-order key sequence (BST ordering is preserved). They are O(1) and completely colorblind — no recoloring is done inside these functions. Only `rotate()` (the router called by `balance_t`) handles color changes, because the correct swap depends on which node rises to the grandparent position.

```
Left rotation around x:        Right rotation around x:

    x              y                 x              y
   / \    →       / \               / \    →       / \
  A   y          x   C             y   C          A   x
     / \        / \               / \                / \
    B   C      A   B             A   B              B   C
```

The sub-tree `B` re-parents from the elevated node (`y`) to the demoted node (`x`) — this is the only non-trivial pointer link in both rotations.

---

## Testing

```bash
make test && ./test_rbt
```

Expected output:

```
==================================================
   RED-BLACK TREE COMPREHENSIVE UNIT TESTS
==================================================

Testing Empty Tree Edge Cases...
  [PASS] Empty tree handled safely.

Testing Case 1: Insertions & Balancing...
  [PASS] All 4 rotation cases correct.

Testing Case 3 & 4: Search & Display...
  [PASS] Search logic verified.

Testing Case 5 & 6: Find MIN / MAX...
  [PASS] Minimum is 1, Maximum is 70.

Testing Case 7 & 8: Delete MIN / MAX...
  [PASS] Min and Max successfully deleted and updated.

Testing Case 2: Complex Internal Deletions...
  [PASS] Internal node and leaf deletions succeeded.

Testing Case 9: Free Memory...
  [PASS] Tree memory successfully freed.

==================================================
 [SUCCESS] ALL 9 OPERATIONS PASSED FLAWLESSLY!
==================================================
```

| Category                         | Result          |
|----------------------------------|-----------------|
| Empty tree edge cases            | 100% PASS       |
| All 4 rotation cases             | 100% PASS       |
| 12-node stress insertion         | 100% PASS       |
| Min/Max delete & update          | 100% PASS       |
| Root deletion (2-child node)     | 100% PASS       |
| Memory deallocation (Valgrind)   | 0 bytes leaked  |

---

## Development Challenges

### Handling NIL Leaves (Segmentation Faults)

Accessing `.color` directly on a `NULL` pointer caused segfaults in early builds. The fix was `get_color(node_t *)` and `set_color(node_t *, color_t)` wrappers that check for `NULL` before dereferencing. Every color check in `delete_fix_up()` goes through these wrappers.

### The Double Black Deletion Problem

Standard BST deletion is insufficient. When a BLACK node with a BLACK replacement is removed, the black-height property breaks — a condition called "Double Black." Resolving it required implementing `delete_fix_up()` with 4 cases and their 4 mirror images. Because the replacement node `x` is frequently `NULL`, the separate `x_parent` pointer had to be tracked throughout the entire deletion routine.

### Short-Circuit Logic Bugs

Writing `uncle->color == BLACK || uncle == NULL` dereferences `uncle` before checking whether it is NULL. The corrected form — `uncle == NULL || uncle->color == BLACK` — relies on C's guaranteed left-to-right short-circuit evaluation. The order of operands in boolean expressions is not cosmetic in C.

### Empty Tree Edge Cases

`find_min()` and `find_max()` initially crashed on empty trees because the NULL guard was placed after the first pointer dereference. The fix: `if (root == NULL) return NULL;` at the very top of each function.

### Switch-Case Fallthrough

A missing `break` between Case 8 (Delete Max) and Case 9 (Exit) caused `free_tree()` to run after every Delete Max, silently wiping the tree. The program did not crash — it simply behaved as if the tree were always empty afterward. This was only caught by running every menu path during integration testing.

---

## Performance

| Operation     | Time Complexity |
|---------------|-----------------|
| Insert        | O(log n)        |
| Delete        | O(log n)        |
| Search        | O(log n)        |
| Find MIN/MAX  | O(log n)        |
| Display       | O(n)            |
| Free Tree     | O(n)            |

Space complexity is O(n). Rotations and recoloring are O(1) per step.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

---

## License

MIT License. See [LICENSE](LICENSE).