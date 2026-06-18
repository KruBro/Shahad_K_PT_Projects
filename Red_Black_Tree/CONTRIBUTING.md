# Contributing to Emertxe Red-Black Tree

Thank you for your interest in contributing. This is a focused C systems project, so contributions should be precise, well-tested, and respect the existing architecture.

> **On AI-assisted contributions:** Using AI tools (Copilot, Claude, ChatGPT, etc.) to help write code or documentation is allowed, but you must review, understand, and take responsibility for every line you submit. Do not paste generated code you cannot explain. If AI materially authored a section, note it briefly in your PR description. We are not opposed to AI tooling — we are opposed to unreviewed AI output.

---

## Getting Started

### Prerequisites

- GCC or Clang (C99 or later)
- GNU Make
- A POSIX terminal

### Build the Project

```bash
git clone https://github.com/your-username/red-black-tree.git
cd red-black-tree
make all
./rbt
```

### Run the Test Suite

```bash
make test
./test_rbt
```

All tests must pass before you open a pull request. The suite uses `assert()` — a failure causes an immediate abort with a clear line number.

---

## How to Contribute

### 1. Fork and Branch

```bash
git checkout -b fix/your-descriptive-branch-name
```

Use a short, lowercase branch name describing the change: `fix/null-guard-display`, `feat/level-order-traversal`, `docs/contributing-guide`.

### 2. Make Your Changes

Keep changes focused. One logical change per pull request. If you are fixing a bug and also refactoring unrelated code, split them into separate PRs.

### 3. Code Style

This project uses a consistent C style. Match it:

- Indent with **4 spaces** (no tabs).
- Opening braces on the **same line** for functions and control flow.
- Every public function must have a comment above it stating its purpose, inputs, and any side effects.
- No magic numbers — use named constants or enums.
- Keep functions short. If a function exceeds roughly 50 lines, consider splitting it.

### 4. Adding or Modifying Tests

All tests live in `test.c`. The file uses standard `assert()` checks inside a single `main()`. To add a test:

1. Find the relevant section (e.g., "CASE 1: Insertion & Balancing").
2. Add your `assert()` statements immediately after the existing ones in that section.
3. Include a comment explaining what property or edge case your assertion verifies.

Example:

```c
// Verify that inserting a duplicate key returns FAILURE
assert(insert(&root, 20) == FAILURE);
```

Do not introduce external test frameworks. The goal is a self-contained `test.c` that compiles with the same `makefile` target.

### 5. Makefile Targets

| Target       | Command        | Description                          |
|--------------|----------------|--------------------------------------|
| Build all    | `make all`     | Compiles the interactive binary      |
| Run tests    | `make test`    | Compiles and links `test.c`          |
| Clean        | `make clean`   | Removes all `.o` and binary files    |

If you add a new `.c` file, add it to the appropriate object list in the `makefile`. Do not break either the `all` or `test` targets.

### 6. Memory Safety

Run your changes under Valgrind before submitting:

```bash
valgrind --leak-check=full ./rbt
valgrind --leak-check=full ./test_rbt
```

The project currently reports **0 bytes leaked**. Any PR that introduces memory leaks will be rejected until fixed.

### 7. Open a Pull Request

Use the PR template in `.github/PULL_REQUEST_TEMPLATE.md`. Fill in every section. PRs with empty descriptions will not be reviewed.

---

## What We Welcome

- Bug fixes with a corresponding regression test
- New tree operations (e.g., level-order display, rank/select)
- Performance improvements with benchmarks
- Documentation improvements

## What We Will Reject

- Changes that break existing `assert()` tests
- Code with memory leaks confirmed by Valgrind
- Unreviewed AI-generated code pasted without understanding
- Style changes that don't match the existing conventions without prior discussion
- Refactors larger than ~50 lines without an accompanying issue discussion

---

## Reporting Bugs

Open an issue using the **Bug Report** template. Include the exact sequence of menu operations that reproduces the bug. A crash without reproduction steps will be deprioritized.

---

## Questions

Open a GitHub Discussion or tag your issue with `question`. We aim to respond within a few days.