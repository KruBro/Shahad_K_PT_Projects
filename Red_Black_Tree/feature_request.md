---
name: Feature Request
about: Suggest a new tree operation, improvement, or enhancement
title: "[FEAT] <short description>"
labels: enhancement
assignees: ''
---

## Summary

One sentence: what do you want to add or change, and why?

---

## Motivation

What problem does this solve, or what capability does it add? Explain the use case concisely. "It would be cool" is not a motivation — explain the concrete benefit.

---

## Proposed Design

How should this be implemented? Consider:

- Which existing file(s) would be modified, or would a new `.c`/`.h` file be needed?
- What is the function signature?
- Does it affect the `tree.h` interface (new prototypes, struct changes)?
- Does it affect `main.c` (new menu option)?
- Does it require changes to the `makefile`?

---

## Test Plan

How would you verify this works correctly? List the `assert()` checks you would add to `test.c`.

Example:
```c
// Verify level-order traversal visits root first
level_order_traversal(root, output_array);
assert(output_array[0] == root->data);
```

---

## Alternatives Considered

Were there other ways to implement this? Why is your proposed approach better?

---

## Additional Context

Any references, papers, or prior art worth linking (e.g., CLRS chapter, a specific algorithm variant)?