/**
 * @file  color.h
 * @brief ANSI escape-code macros for terminal color theming.
 *
 * Two layers:
 *   Layer 1 — Raw codes:    COLOR_RED, COLOR_RESET, etc.
 *   Layer 2 — Semantic aliases: CLR_SUCCESS, CLR_ERROR, etc.
 *
 * main.c and display.c use only semantic aliases. To retheme the whole
 * program, change only the Layer 2 assignments below.
 *
 * Note: ANSI codes work on POSIX terminals (Linux, macOS, WSL).
 * For bare Windows cmd.exe, replace macro bodies with empty strings "".
 */

#ifndef COLOR_H
#define COLOR_H

/* ── Layer 1: Raw ANSI codes ─────────────────────────────────────────────── */
#define COLOR_RESET         "\033[0m"
#define COLOR_RED           "\033[0;31m"
#define COLOR_GREEN         "\033[0;32m"
#define COLOR_YELLOW        "\033[0;33m"
#define COLOR_CYAN          "\033[0;36m"
#define COLOR_GRAY          "\033[0;90m"
#define COLOR_BOLD_RED      "\033[1;31m"
#define COLOR_BOLD_GREEN    "\033[1;32m"
#define COLOR_BOLD_YELLOW   "\033[1;33m"
#define COLOR_BOLD_CYAN     "\033[1;36m"
#define COLOR_BOLD_WHITE    "\033[1;37m"

/* ── Layer 2: Semantic aliases ───────────────────────────────────────────── */
#define CLR_TITLE       COLOR_BOLD_CYAN
#define CLR_DIVIDER     COLOR_CYAN
#define CLR_OPTION      COLOR_BOLD_YELLOW
#define CLR_PROMPT      COLOR_BOLD_WHITE
#define CLR_SUCCESS     COLOR_BOLD_GREEN
#define CLR_ERROR       COLOR_BOLD_RED
#define CLR_INFO        COLOR_CYAN
#define CLR_NODE_RED    COLOR_BOLD_RED
#define CLR_NODE_BLACK  COLOR_GRAY

#endif /* COLOR_H */