# Example: Comprehensive Usage of Jsson-CPP

This example demonstrates the full capabilities of the modern C++ JSON library (`jsson-cpp`). It showcases:

## Features Demonstrated

### 1. Creating JSON Values
- Direct construction of various JSON types (null, boolean, integer, real, string)
- Building JSON arrays and objects
- Using `JsonValue` constructors and `emplace_back` for dynamic addition

### 2. Parsing JSON from Files
- Creating a temporary JSON file with hardcoded content
- Parsing the file using `Parser::parse`
- Converting the parsed JSON back to a string using `dump`

### 3. Error Handling
- Demonstrating handling of various `JsonError` codes:
  - `CannotOpenFile` (non-existent file)
  - `InvalidSyntax` (malformed JSON)
- Using exceptions via `error.hpp` to catch and report errors

### 4. Pack/Unpack Functionality
- Using `PackUnpack::pack` and `PackUnpack::unpack` to serialize/deserialize values
- Demonstrating packing of integers, strings, and booleans
- Showing how to retrieve packed data with proper position tracking

### 5. Data Structures
- Using `Hashtable` for key-value storage with thread-safe operations
- Using `StringBuffer` for efficient string manipulation

### 6. Exception Usage
- All error handling is performed using `JsonError` exceptions
- Errors are caught and their messages are printed

## Building and Running the Example

The example can be built using CMake:

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./examples/example
```

The executable will run and print demonstrations of all the features listed above.

## Code Structure

- `examples/example.cpp`: Main example program
- `examples/example.md`: Documentation (this file)
- The example links against the `jsson-cpp` library which contains all the implementation files in `jsson-cpp/src/`

## Implementation Details

The example includes headers from all modules in `jsson-cpp`:
- `error.hpp` - Exception definitions and error handling
- `json_value.hpp` - Core JSON value representation
- `parser.hpp` - JSON parsing functionality
- `pack_unpack.hpp` - Binary packing/unpacking utilities
- `memory.hpp` - Smart pointer utilities
- `hashtable.hpp` - Thread-safe hash table implementation
- `strbuffer.hpp` - Efficient string buffer class

All these modules are compiled into a single library and linked with the example executable.