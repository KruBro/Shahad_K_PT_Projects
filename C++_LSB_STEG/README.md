# LSB Steganography (C++23 Port)

A command-line tool for hiding a secret text file inside a BMP image (and
extracting it back out) using least-significant-bit (LSB) steganography.
This is a modern C++23 port of an original procedural C implementation.

## Building

```bash
make
```

Produces a `steg` binary. Requires a C++23-capable compiler (developed
against GCC 16). Clean with `make clean`.

## Usage

**Encode** a secret file into an image:

```bash
./steg -e <source_image.bmp> <secret_file.txt> [output_image.bmp]
```

- `source_image.bmp` — the cover image (`.bmp` required)
- `secret_file.txt` — the file to hide (`.txt` required)
- `output_image.bmp` — optional; defaults to `stego.bmp`

**Decode** a secret file out of a previously encoded image:

```bash
./steg -d <stego_image.bmp> [output_file.txt]
```

- `stego_image.bmp` — an image previously produced by `-e`
- `output_file.txt` — optional; defaults to `output.txt`

Decoding an image that was never encoded by this tool will fail a magic
string check (`#*`) rather than produce garbage output.

## How it works

Each bit of secret data is spread across the least-significant bit of 8
consecutive image bytes. The encoded stream, in order, is:

1. A fixed magic string (`#*`), identifying the file as produced by this tool
2. The secret file's extension length, then the extension itself
3. The secret file's size, then the secret data itself

The 54-byte BMP header is copied verbatim and never touched, so the image
remains a valid, viewable BMP after encoding.

## Design notes

This port intentionally moves away from the original C idioms:

- **RAII over manual cleanup** — `std::ifstream`/`std::ofstream` members
  replace raw `FILE*`; files close automatically on every exit path,
  including early failures, eliminating a real handle-leak bug present in
  the original.
- **Validated construction** — `EncodeInfo`/`DecodeInfo` have private
  constructors reachable only through a `static create(argv)` factory
  returning `std::expected<T, Status>`. An object of either type cannot
  exist in a partially-validated state.
- **`std::span`/`std::array` over raw pointers** — buffer bounds are part
  of the type wherever the size is known, rather than tracked separately
  by convention.
- **`enum class`** for `Status`/`OperationType` — prevents silently
  comparing unrelated status codes, a real bug class in the original enums.
- **Stateless algorithm/utility layers** — the bit-level LSB logic
  (`namespace lsb`) and BMP file-format helpers (`namespace bmp`) are pure
  functions with no dependency on `EncodeInfo`/`DecodeInfo`, shared by both
  classes via `lsb.h`/`bmp.h`.

## Project layout

| File | Contents |
|---|---|
| `types.h` | `Status`, `OperationType` |
| `common.h` | Shared `magic_string` constant |
| `lsb.h` | Bit-level encode/decode primitives (`namespace lsb`) |
| `bmp.h` | BMP file-format utilities (`namespace bmp`) |
| `encode.h` / `encode.cpp` | `EncodeInfo` class |
| `decode.h` / `decode.cpp` | `DecodeInfo` class |
| `main.cpp` | CLI entry point |