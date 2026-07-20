# CLI Text Editor

## Overview
A purely console-based, line-oriented text editor implemented in C. Designed with systems-level programming principles, it features dynamic memory management, a doubly linked list for document representation, and dynamic array stacks for history and clipboard management.

## Architecture and Core Data Structures
The project relies on specific memory and state management strategies to handle dynamic text safely:

*   **Document State (Doubly Linked List):** The core document is represented as a doubly linked list where each `Node` contains a heap-allocated string (`char *text`). This allows O(1) line insertions and deletions once the cursor is positioned.
*   **Undo/Redo History (Dynamic Array Stack):** Edit history is maintained using a custom stack (`DynamicArrayStack`). Every insertion or deletion captures an `Action` struct detailing the operation type (`OP_INSERT`, `OP_DELETE`), the string payload, and the exact `cursorLine` and `cursorPos`. Reverting an action pops it from the undo stack, applies the inverse operation via `memmove`/`realloc`, and pushes it to the redo stack.
*   **Clipboard Buffer (Pointer Stack):** The clipboard (`ClipboardStack`) dynamically holds copied or cut strings (array of `char *`). The stack doubles in capacity upon reaching limits, allowing multiple historical clips to be retained.

## Module Breakdown

### Core Engine
*   **`main.c` / `main_utils.c`:** Houses the primary REPL (Read-Eval-Print Loop). Handles user input tokenization using `strtok`, parsing commands, and routing execution through a central switch-case (`executeCommand`).
*   **`text_editor.c`:** Manages the linked list. Contains logic for memory allocation of line nodes, appending lines, deleting full lines, and safely freeing the entire document structure to prevent memory leaks (`freeEditor`).
*   **`insert_delete.c`:** Handles inline text modification. `insertText` dynamically reallocates line buffers and shifts existing characters using `memmove`. `deleteCharacters` excises characters behind the cursor and updates the undo stack.

### Navigation & Search
*   **`cursor_navigations.c`:** Tracks the X/Y cursor coordinate (`cursorLine`, `cursorPos`). Includes bounding logic (`clampCursorPos`) to prevent out-of-bounds memory access when moving between lines of varying lengths.
*   **`search.c`:** Utilizes `strstr` to locate substring matches either from the current cursor position (Find Next) or across the entire document (Find All), automatically jumping the cursor to the exact hit coordinate.
*   **`replace.c`:** Combines search, delete, and insert operations to swap text. Supports both single-instance and global replacements.

### I/O and Advanced Editing
*   **`file_ops.c`:** Handles file ingestion and extraction. `openFile` reads characters into a dynamically doubling buffer until a newline is hit, translating raw files into the linked-list node structure. `saveFile` iterates through the list, writing `node->text` back to `fileSave.txt`.
*   **`copy_paste.c`:** Implements `copy`, `cut`, and `paste` utilizing the `ClipboardStack`. Features an interactive sub-REPL (`navigateToCopyPoint`) allowing the user to precisely position the cursor step-by-step prior to executing a cut or copy command.

## Command Reference

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `addline` | `<text>` | Appends a new line containing `<text>` to the end of the document. |
| `insert` | `<text>` | Inserts `<text>` exactly at the current cursor position. |
| `delete` | *(prompts)* | Prompts for the number of characters to delete backwards from the cursor. |
| `deleteline`| `<lineNumber>` | Deletes the specified line and routes pointers to bridge the gap. |
| `undo` / `redo` | None | Reverts or reapplies the last inline text modification. |
| `search` | `<text>` | Searches for `<text>`. Prompts for Find All vs Find Next. |
| `replace` | `<old> <new>` | Replaces `<old>` with `<new>`. Prompts for Replace All vs Replace Next. |
| `display` | None | Prints the entire document with a physical `^` cursor indicator. |
| `copy` / `cut` | *(interactive)*| Triggers the navigator, asks for character count, and saves to clipboard. |
| `paste` | None | Inserts the string at the top of the clipboard at the cursor position. |
| `openfile` | `<filename>` | Reads a file into the editor's memory. |
| `savefile` | None | Writes the current document state to `fileSave.txt`. |
| Navigation | None | `moveup`, `movedown`, `moveleft`, `moveright`, `jumptoline <n>` |
| `exit` | None | Safely frees all allocated memory and terminates the process. |

## Memory Management
The project adheres to strict manual memory management. It ensures that memory allocated for line buffers, action payloads (undo/redo), and clipboard entries is systematically freed on command execution or program exit (`freeEditor`, `freeStack`, `freeClipBoardStack`). Edge cases, such as deleting the sole remaining line, handle buffer flushing without destroying the root head pointer.
