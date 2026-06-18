---
name: Bug Report
about: Report a crash, incorrect output, memory leak, or behavioral defect
title: "[BUG] <short description>"
labels: bug
assignees: ''
---

## Summary

A clear, one-sentence description of what went wrong.

---

## Reproduction Steps

Provide the **exact sequence of menu operations** needed to reproduce the bug. Be precise — "it crashes on delete" is not enough.

Example:
1. Run `./rbt`
2. Insert: `10`, `20`, `30`
3. Delete: `20`
4. Display → observe incorrect output / crash

---

## Expected Behavior

What should have happened?

---

## Actual Behavior

What actually happened? Include the full terminal output, error message, or crash output. If there was a segfault, include the backtrace from `gdb` or `valgrind` output if possible.

```
paste output here
```

---

## Environment

- OS:
- Compiler & version (e.g., `gcc --version`):
- Commit hash or branch (`git log --oneline -1`):

---

## Valgrind Output (if applicable)

If this is a memory issue, paste `valgrind --leak-check=full ./rbt` output:

```
paste valgrind output here
```

---

## Proposed Fix (optional)

If you already know what is wrong, describe it or link to the relevant lines of code. A patch is even better — consider opening a PR alongside this issue.