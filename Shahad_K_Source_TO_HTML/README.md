# s2html

A lightweight C-based source code to HTML converter that performs syntax highlighting for C source files.

`s2html` parses C source code, identifies language constructs such as comments, strings, keywords, header files, numeric constants, and character literals, then generates an HTML file with appropriate styling for improved readability.

---

## Features

* Converts C source files into HTML
* Syntax highlighting for:

  * Preprocessor directives (`#include`, `#define`, etc.)
  * Reserved keywords
  * Numeric constants
  * String literals
  * Character literals
  * Header files
  * Single-line comments
  * Multi-line comments
* Generates valid HTML5 output
* Uses an event-driven parser architecture
* Simple Makefile-based build system

---

## Project Structure

```
s2html/
│
├── s2html_main.c      # Program entry point
├── s2html_event.c     # Lexical parser and event generator
├── s2html_event.h     # Event definitions and parser interface
├── s2html_conv.c      # HTML conversion functions
├── s2html_conv.h      # HTML conversion interface
├── Makefile           # Build configuration
└── styles.css         # CSS styling for syntax highlighting
```

---

## Build Requirements

* GCC compiler
* GNU Make

### Ubuntu/Debian

```bash
sudo apt install build-essential
```

### Fedora

```bash
sudo dnf install gcc make
```

---

## Compilation

Build the project using:

```bash
make
```

This generates:

```bash
s2html
```

---

## Usage

### Basic Usage

```bash
./s2html source_file.c
```

Example:

```bash
./s2html hello.c
```

Output:

```bash
hello.c.html
```

---

### Custom Output Name

```bash
./s2html source_file.c output_name
```

Example:

```bash
./s2html hello.c highlighted
```

Output:

```bash
highlighted.html
```

---

## Generated Output

The generated HTML contains:

```html
<!DOCTYPE html>
<html>
<head>
    ...
</head>
<body>
    <pre>
        highlighted source code
    </pre>
</body>
</html>
```

Syntax highlighting is controlled through CSS classes such as:

```css
.preprocess_dir
.comment
.string
.header_file
.numeric_constant
.reserved_key1
.reserved_key2
.ascii_char
```

---

## Build Targets

### Build

```bash
make
```

Builds the main executable.

### Run Tests

```bash
make test
```

Builds and runs the test suite.

### Clean

```bash
make clean
```

Removes:

```bash
*.o
s2html
test_runner
```

---

## Parser Architecture

The parser operates as a finite-state machine (FSM).

Main parser states include:

* Idle
* Preprocessor Directive
* Header File
* Reserved Keyword
* Numeric Constant
* String Literal
* Character Literal
* Single-Line Comment
* Multi-Line Comment

Each recognized token generates a parser event (`pevent_t`) which is then converted into corresponding HTML output.

---

## Example

Input:

```c
#include <stdio.h>

int main()
{
    printf("Hello World\n");
    return 0;
}
```

Output:

```html
<span class="preprocess_dir">#include</span>
<span class="header_file">&lt;stdio.h&gt;</span>

<span class="reserved_key1">int</span> main()
{
    printf(<span class="string">"Hello World\n"</span>);
    <span class="reserved_key2">return</span>
    <span class="numeric_constant">0</span>;
}
```

---

## Author

Developed as a C programming project demonstrating:

* File handling
* Lexical analysis
* Finite-state machines
* Syntax highlighting
* HTML generation

---

## License

This project is provided for educational and learning purposes.
