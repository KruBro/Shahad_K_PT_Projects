# Changelog
All notable changes to this project will be documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Versions follow Semantic Versioning.

## [1.0.0] — 2025-07-01
Initial release. Complete, tested implementation of a menu-driven Red-Black Tree in C.

### Added
**Core tree operations**
* `insert()` — iterative BST insert with `balance_t()` fix-up (recolor + LL/RR/LR/RL rotations)
* `delete_t()` — BST delete with transplant, in-order successor handling, and `delete_fix_up()` for complex Double Black violation resolution
* `search_t()` — recursive BST key lookup
* `find_min()` / `find_max()` — leftmost/rightmost node traversal
* `display_tree()` — in-order traversal with ANSI color-coded terminal output
* `free_tree()` — post-order full deallocation (Valgrind: 0 bytes leaked)

**Safety primitives**
* `get_color()` / `set_color()` — NULL-safe color wrappers (prevents NIL-leaf segfaults)
* `transplant()` — isolated pointer-swap primitive used by `delete_t()`
* `recolor()` — pure functional color toggle; does not mutate in place

**Correctness fixes over development**
* Fixed use-after-free in Cases 7 & 8: target value now saved before `delete_t()` frees the node
* Fixed switch-case fallthrough between Case 8 (Delete Max) and Case 9 (Exit)
* Fixed short-circuit evaluation bug: `uncle == NULL` check moved before `uncle->color` access
* Added NULL guards to `find_min()` and `find_max()` to handle empty-tree queries safely

**Project infrastructure**
* `color.h` — two-layer ANSI macro system (raw codes + semantic aliases)
* `makefile` — all, test, and clean targets; `main.c` correctly excluded from test build
* `test.c` — unit test suite covering all 9 operations with `assert()` checks
* GitHub Actions CI/CD pipeline (`.github/workflows/ci-pipeline.yml`) to automatically compile and run the unit test suite on every push
* Full documentation suite: README, CONTRIBUTING, SECURITY, CODE_OF_CONDUCT, issue templates, PR template

## [Unreleased]
Nothing pending.