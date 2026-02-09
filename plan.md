# Conversion Plan

## Overview
This plan outlines the migration of the C source code located in `/src` of the current repository to a modern C++ codebase in a new directory `jsson-cpp`. The goal is to rewrite the code using contemporary C++ idioms: classes with encapsulated state, smart pointers for memory management, standard library containers, enum classes, exceptions, and C++17 features. The conversion will proceed file‑by‑file, ensuring that each module is refactored with clear responsibilities and maintainable design.

## Directory Structure
- `jsson-cpp/` – New root for the C++ implementation.
  - `include/` – Header files (`.hpp`).
  - `src/` – Implementation files (`.cpp`).
  - `tests/` – Test harness (unchanged).
  - `CMakeLists.txt` – Build configuration (to be added later).

## Phase 1 – Project Setup
### Step 1: Create the new directory layout
- Create `jsson-cpp/include` and `jsson-cpp/src`.
- Copy `README.rst` and documentation files as needed.
- Initialize a new CMake project that mirrors the original build settings but targets C++17.

### Step 2: Add build configuration
- Add a top‑level `CMakeLists.txt` that adds subdirectory `jsson-cpp`.
- Configure compile options: `-Wall -Wextra -pedantic -std=c++17`.
- Set up installation rules for headers and libraries.

## Phase 2 – Core Data Model Conversion
### Step 2.1: Convert `value.c` → `jsson-cpp/src/value.cpp`
- **Purpose**: `json_value` represents any JSON value (object, array, number, string, etc.).
- **Implementation Details**:
  - Create class `JsonValue` with a `std::variant` holding concrete types (object, array, number, string, boolean, null).
  - Encapsulate `std::unique_ptr<JsonObject>` and `std::unique_ptr<JsonArray>` for composite values.
  - Provide constructors, destructor (automatic via RAII), copy/move semantics.
  - Implement methods `asObject()`, `asArray()`, `asString()`, etc., returning references (`JsonObject&`, `JsonArray&`) and const overloads.
  - Use `std::optional` for optional fields.
  - Add `toString()` returning `std::string` using `std::ostringstream`.
- **Dependencies**: None within the project; this file is foundational.

### Step 2.2: Convert `jansson.h` and related structs → `jsson-cpp/include/jsson.h`
- **Purpose**: Define modern C++ equivalents of the C structs (`json_t`, `json_object`, `json_array`).
- **Implementation Details**:
  - Replace `struct json_object` with class `JsonObject` inheriting from `std::enable_shared_from_this` to allow shared ownership.
  - Replace `struct json_array` with class `JsonArray`.
  - Use `std::unordered_map<std::string, std::shared_ptr<JsonValue>>` for object members.
  - Provide methods `add(const std::string&, std::shared_ptr<JsonValue>)`, `remove(const std::string&)`, `at(const std::string&)`.
  - Use `enum class JsonType` for type enumeration.
  - Ensure copy/move constructors are defaulted; destructor is defaulted.

### Step 2.3: Convert `load.c` → `jsson-cpp/src/load.cpp`
- **Purpose**: File I/O and parsing into `JsonValue`.
- **Implementation Details**:
  - Create a `Parser` class with a static method `parse(const std::string&)` returning `std::shared_ptr<JsonValue>`.
  - Use `std::ifstream` and `std::getline` to read file content.
  - Implement recursive descent parsing using `std::string_view` for efficient substring handling.
  - Throw `std::runtime_error` on syntax errors.
  - Utilize `std::unique_ptr` for temporary buffers.

## Phase 3 – Utility Modules
### Step 3.1: Convert `strbuffer.c` → `jsson-cpp/src/strbuffer.cpp`
- **Purpose**: String buffer utilities.
- **Implementation Details**:
  - Replace C‑style buffer with class `StringBuffer` using `std::string` and `std::vector<char>` as needed.
  - Provide `append`, `prepend`, `remove` methods with reference returns.
  - Use move semantics for efficient concatenation.

### Step 3.2: Convert `utf.c` → `jsson-cpp/src/utf.cpp`
- **Purpose**: UTF‑8 handling and validation.
- **Implementation Details**:
  - Wrap functions into a namespace `Utf8`.
  - Use `std::string` and `std::u32string` where appropriate.
  - Provide `isValid(const std::string&)` returning `bool`.
  - Use `std::optional<char32_t>` for error positions.

### Step 3.3: Convert `dtoa.c` → `jsson-cpp/src/dtoa.cpp`
- **Purpose**: Double‑to‑string conversion.
- **Implementation Details**:
  - Replace custom conversion with `std::to_string` or `std::ostringstream`.
  - Provide `doubleToString(double)` returning `std::string`.
  - Ensure precision handling via `std::setprecision`.

## Phase 4 – Container and Hash Utilities
### Step 4.1: Convert `hashtable.c` → `jsson-cpp/src/hashtable.cpp`
- **Purpose**: Hash table implementation used internally.
- **Implementation Details**:
  - Implement a template class `HashTable<K, V>` using `std::unordered_map`.
  - Use `std::unique_ptr` for collision‑resolution structures if needed.
  - Provide `insert`, `erase`, `find` methods returning `std::optional<V>&`.
  - Ensure thread‑safety with `std::shared_mutex` for read/write locks.

### Step 4.2: Convert `memory.c` → `jsson-cpp/src/memory.cpp`
- **Purpose**: Memory allocation utilities.
- **Implementation Details**:
  - Replace `malloc`/`free` with `std::unique_ptr<void, decltype(&free)>` wrappers.
  - Provide `make_unique_array<T>(size_t)` returning `std::unique_ptr<T[]>`.
  - Use `std::aligned_alloc` for alignment requirements.

## Phase 5 – Remaining Source Files
### Step 5.1: Convert `dump.c` → `jsson-cpp/src/dump.cpp`
- **Purpose**: Serialization of JSON values to output streams.
- **Implementation Details**:
  - Create `dump::JsonDumper` with method `dump(const std::shared_ptr<JsonValue>&, std::ostream&)`.
  - Use range‑based for loops and `std::visit` over `std::variant`.
  - Ensure const‑correctness; methods are `const`.

### Step 5.2: Convert `pack_unpack.c` → `jsson-cpp/src/pack_unpack.cpp`
- **Purpose**: Binary packing/unpacking.
- **Implementation Details**:
  - Replace C struct packing with `std::byte` manipulation and `std::bit_cast`.
  - Use `std::vector<std::byte>` for buffers.
  - Provide `pack` and `unpack` templates returning `std::vector<std::byte>`.

### Step 5.3: Convert `error.c` → `jsson-cpp/src/error.cpp`
- **Purpose**: Error handling and messaging.
- **Implementation Details**:
  - Replace error code enums with `enum class ErrorCode`.
  - Throw `std::system_error` with `ErrorCode` as the error condition.
  - Provide `error_message(ErrorCode)` returning `std::string`.

## Phase 6 – Integration and Testing
### Step 6.1: Update Header Includes
- Replace `#include "json_*.h"` with `#include <jsson/jsson.h>` in all converted files.
- Ensure include guards are replaced with `#pragma once` or modern header guards.

### Step 6.2: Verify ABI Compatibility
- Run static analysis (e.g., `clang-tidy`) to detect any remaining C‑style patterns.
- Ensure all public APIs are exported with appropriate `JSSON_API` macro.

### Step 6.3: Run Unit Tests
- Execute existing test suite against the new implementation.
- Confirm that all previously passing tests still pass; fix any regressions.

## Phase 7 – Documentation and Release
### Step 7.1: Update Documentation
- Document each public class and function in `include/jsson.hpp`.
- Provide usage examples in `README.rst`.

### Step 7.2: Tag Release
- Create a new git tag for the C++ version.
- Publish the package to the desired package manager (e.g., vcpkg, Conan).