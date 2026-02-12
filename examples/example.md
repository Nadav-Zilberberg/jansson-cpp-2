# Example Usage of jsson-cpp

This document describes the example program located at `examples/example.cpp` and how to build and run it.

## Overview

The example demonstrates the capabilities of the modern C++ `jsson-cpp` library, including:

- Creating JSON values of different types (null, boolean, number, string, array, object).
- Parsing a hard‑coded JSON string and converting it back to a string representation.
- Error handling using exceptions defined in `error.hpp`.
- Pack/unpack functionality for converting between JSON values and native types.
- Usage of all library components:
  - `strbuffer.hpp` – building strings with `StringBuffer`.
  - `dump.hpp` – dumping JSON values to strings.
  - `parser.hpp` – parsing JSON strings into `JsonValue` objects.
  - `utf8.hpp` – validating UTF‑8 strings.
  - `pack_unpack.hpp` – packing and unpacking values.
  - `memory.hpp` – custom memory allocation with `Allocator`.
  - `hashtable.hpp` – hash table utilities.
  - `dtoa.hpp` – double‑to‑string conversion.
  - `error.hpp` – exception types and error codes.

## Building the Example

The project uses CMake for building. From the repository root:

```bash
mkdir -p build && cd build
cmake ..          # Configures the project
cmake --build .   # Compiles the library and the example
```

The resulting executable will be placed in the `examples/` directory inside the build folder:

```bash
./examples/example
```

## Running the Example

The program exercises all major features of the library and prints diagnostic output to the console. It will:

1. Create JSON values of various types and display them.
2. Parse a hard‑coded JSON string and dump it back to a string.
3. Demonstrate error handling by attempting to parse an invalid JSON string.
4. Use pack/unpack to convert between JSON values and native C++ types.
5. Allocate memory using the custom `Allocator`.
6. Validate UTF‑8 strings.
7. Use hash table utilities and double‑to‑string conversion.

Any errors are caught as `jsson::error::JsonError` exceptions and reported with their error code.

## Code Structure

- `examples/example.cpp` – Main program that exercises the library.
- `CMakeLists.txt` – Root CMake configuration that builds the library and the example.
- `jsson-cpp/` – Contains the library source (`src/`) and header (`include/`) files.

## License

The example code is provided under the same license as the `jsson-cpp` library.