# Security Policy

## Scope

This is a terminal-based, single-user C program intended for educational and training purposes. It does not run as a service, handle network traffic, authenticate users, or process untrusted external input in any deployment scenario.

That said, C is a language where memory safety issues (buffer overflows, use-after-free, NULL dereferences) are real and worth tracking seriously, even in educational code. We take these reports genuinely.

---

## Supported Versions

Only the latest commit on the `main` branch is actively maintained.

| Version     | Supported |
|-------------|-----------|
| `main` HEAD | Yes       |
| Older tags  | No        |

---

## Reporting a Vulnerability

**Do not open a public GitHub issue for security vulnerabilities.**

If you find a memory safety issue, undefined behavior, or any defect that could cause incorrect or dangerous behavior when this code is used as a dependency or adapted into a larger system, please report it privately:

1. Email the maintainer at: `[maintainer-email@example.com]` (replace with your actual contact)
2. Use the subject line: `[SECURITY] Red-Black Tree — <brief description>`
3. Include:
   - A description of the vulnerability and its potential impact
   - Steps to reproduce (exact input sequence or code path)
   - The version/commit hash you tested against
   - Whether you have a proposed fix

---

## What to Expect

- **Acknowledgement:** Within 72 hours of your report.
- **Assessment:** Within one week, we will confirm whether the issue is valid and classify its severity.
- **Resolution:** For confirmed issues, we aim to push a fix within two weeks. You will be credited in the commit message unless you request otherwise.
- **Disclosure:** We will coordinate public disclosure timing with you. We will not disclose without giving you a chance to review the fix first.

---

## Known Limitations

The following are known, documented design decisions — not vulnerabilities:

- **Integer overflow in input:** `scanf("%d", &data)` does not defend against extremely large or malformed integers. This is intentional given the educational scope of the project. If you are adapting this code for production use, replace all `scanf` calls with validated input routines.
- **No bounds checking on the menu input:** A non-numeric input to the menu will cause `scanf` to leave the input buffer in a bad state. The `default` case will catch the wrong `choice` value, but repeated non-numeric input can cause an infinite loop. This is a known educational limitation, not a security issue in this context.
- **Global `status` variable:** `status` in `main.c` / `delete.c` is a global `int`. This is not thread-safe. This program is single-threaded by design.

---

## Memory Safety

The project currently reports **0 bytes leaked** under Valgrind on the complete test suite. Any regression in this metric is treated as a bug. If you find a leak or use-after-free, please report it using the process above.