## Summary

What does this PR do? One paragraph maximum. If it is a bug fix, link the issue: `Fixes #<number>`.

---

## Type of Change

- [ ] Bug fix (non-breaking change that fixes an issue)
- [ ] New feature (non-breaking change that adds functionality)
- [ ] Breaking change (changes an existing interface or behavior)
- [ ] Documentation only
- [ ] Refactor (no functional change, no new features)

---

## Changes Made

List every file modified and briefly describe what changed in each:

- `file.c` — description of change
- `tree.h` — description of change (if the interface was updated)

---

## Testing

- [ ] `make all` builds without warnings (`-Wall -Wextra`)
- [ ] `make test && ./test_rbt` — all assertions pass
- [ ] Valgrind reports 0 bytes leaked on both `./rbt` and `./test_rbt`
- [ ] New `assert()` tests were added to `test.c` covering this change (if applicable)

Paste the final lines of `./test_rbt` output:

```
paste output here
```

---

## AI Assistance Disclosure

Did you use any AI tool (Claude, Copilot, ChatGPT, etc.) to write part of this PR?

- [ ] No
- [ ] Yes — briefly describe what was AI-generated and confirm you reviewed and understood it:

> Example: "Used Claude to draft the initial `delete_fix_up` comment block. Reviewed and corrected the description of Case 3."

---

## Checklist

- [ ] My code matches the existing style (4-space indent, same brace placement)
- [ ] I added a comment above every new public function
- [ ] I did not introduce any global variables without discussion
- [ ] I ran `make clean && make all` to verify a clean build
- [ ] I am prepared to explain every line of this PR in review