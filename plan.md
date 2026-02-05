# Refined Plan: Converting the C Source Base to Modern C++ (jasson-cpp)

## Overview
The goal is to migrate the entire `src/` directory (C source files) into a new modern C++ project named **jasson-cpp**.  
Each C file will be transformed into a corresponding C++ module (class or namespace) that leverages:

* Encapsulated classes with constructors/destructors and RAII  
* Smart pointers (`std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr`) instead of raw `malloc`/`free`  
* Standard Library containers (`std::vector`, `std::string`, `std::optional`, `std::variant`)  
* `enum class` for scoped enumerations  
* Exception‑based error handling where appropriate  
* C++17 features (auto, range‑based loops, `constexpr`, `std::filesystem`, `std::string_view`, move semantics, templates)  

Only the files that contribute to the core library are converted; test suites, examples, and documentation files are omitted.

---

## Phase 1 – Project Skeleton & Build Setup
1. **Create the new repository structure**  
   ```
   jasson-cpp/
   ├─ CMakeLists.txt
   ├─ include/
   │   └─ jasson/   (public headers)
   ├─ src/
   │   └─ (converted .cpp files)
   ├─ test/        (existing tests – unchanged)
   └─ examples/    (unchanged)
   ```
2. **Add a minimal `CMakeLists.txt`** that:
   * Sets C++ standard to `17`
   * Enables warnings and treats warnings as errors (`-Wall -Wextra -Werror`)
   * Finds the required packages (`Boost` optional for future extensions)
   * Adds the library target `jasson` with `PUBLIC` includes
   * Installs headers and library
3. **Configure modern compiler flags** (e.g., `-fno-omit-frame-pointer`, `-g`, `-fsanitize=address,undefined` for debugging).

*Result*: A clean, reproducible build environment that enforces modern C++ practices from the start.

---

## Phase 2 – Core Data‑Structure Refactoring
Identify the most central modules that hold state and logic. Convert them first to establish a solid foundation.

| C File (src) | Target C++ Module | Conversion Focus |
|--------------|-------------------|------------------|
| `value.c`    | `Value` class     | Replace the `jsmn`‑style union with a type‑safe `std::variant`; encapsulate reference counting with `std::shared_ptr`. |
| `hashtable.c`| `HashTable` class | Use `std::unordered_map` or a custom hash table built on `std::vector<std::unique_ptr<Node>>`; replace manual bucket management with RAII. |
| `memory.c`   | `MemoryPool`      | Wrap the allocator in a class that uses `std::unique_ptr` for internal buffers; provide `allocate<T>()` returning `std::unique_ptr<T>`. |
| `strbuffer.c`| `StringBuffer`    | Replace the raw char buffer with `std::string` or `std::vector<char>`; expose `append`, `c_str`, and `shrink_to_fit` methods. |
| `utf.c`      | `Utf8Util`        | Convert C string utilities to static methods using `std::u8string_view` and `std::string`. |
| `dtoa.c`     | `DoubleConverter` | Use `std::to_chars` (C++17) for fast, allocation‑free conversion; handle errors via exceptions. |
| `version.c`  | `Version` class   | Store version components in a `struct` with `operator==` and `std::string` representation; make it immutable. |

**General conversion steps per file:**
1. Create a header in `include/jasson/` with the same name (e.g., `value.h`).  
2. Implement the class in `src/` (e.g., `value.cpp`).  
3. Replace all raw structs with encapsulated classes; expose only necessary public methods.  
4. Substitute `malloc`/`free` calls with `std::make_unique` or the `MemoryPool` allocator.  
5. Convert global functions to **member functions** or **namespace utilities** that operate on the new types.  
6. Replace error‑code returns with `throw`‑based exceptions where the semantics permit.  
7. Add `constexpr` where possible and mark methods `noexcept` if they cannot throw.  
8. Ensure the class is **move‑constructible** and **move‑assignable** (use `std::move` and defaulted move constructors).  

*Example focus*: When converting `value.c`, replace the `jsmn` union with `std::variant<Null, bool, int, double, std::string, std::shared_ptr<Value>>` and provide a `Value::as<T>()` accessor that throws `std::bad_variant_access` on mismatch.

---

## Phase 3 – Utility Modules
These files contain self‑contained helpers that can be migrated independently.

| C File | Target C++ Module | Key Points |
|--------|-------------------|------------|
| `pack_unpack.c` | `PackUnpack` namespace | Use `std::vector<uint8_t>` and `std::string_view` for buffer handling; replace manual length tracking with `size_t`. |
| `error.c` | `JassonError` hierarchy | Define an abstract `JassonError` class derived from `std::exception`; each error code becomes a concrete subclass. |
| `load.c` / `dump.c` | `Loader` / `Dumper` classes | Encapsulate file I/O with `std::filesystem::path` and `std::ifstream/std::ofstream`; use `std::optional` for return values. |
| `utf.c` | `Utf8Util` (already listed) | Leverage `<unicode>` utilities from the standard library. |

Conversion steps follow the same pattern as Phase 2, with an added emphasis on **exception safety** (e.g., using `std::unique_ptr` to guarantee cleanup on early returns).

---

## Phase 4 – Per‑File Conversion Blueprint
Below is a concrete checklist for each source file. Follow the checklist sequentially; tick off items as you complete them.

### 1. `value.c` → `Value` class
- [ ] Create `include/jasson/value.h` declaring `class Value`.  
- [ ] Replace the `jsmn` union with `std::variant`.  
- [ ] Implement reference‑counted storage using `std::shared_ptr`.  
- [ ] Add `as<T>()` accessors that throw on type mismatch.  
- [ ] Write unit tests (unchanged) to verify behavior.

### 2. `hashtable.c` → `HashTable` class
- [ ] Define `class HashTable` with private `std::vector<std::unique_ptr<Node>> buckets;`.  
- [ ] Implement insertion, lookup, and deletion using RAII containers.  
- [ ] Replace `int` hash returns with `std::size_t` and use `std::hash` where appropriate.  
- [ ] Provide iterator support (optional) using `std::unordered_map`‑style semantics.

### 3. `memory.c` → `MemoryPool` class
- [ ] Encapsulate the pool buffer in `std::unique_ptr<char[]>`.  
- [ ] Offer `allocate<T>(size_t count)` returning `std::unique_ptr<T[]>`.  
- [ ] Add `reset()` to free the pool.  
- [ ] Ensure alignment requirements are met (use `alignas`).

### 4. `strbuffer.c` → `StringBuffer` class
- [ ] Store data in `std::string buffer_;`  
- [ ] Provide `append(std::string_view)` and `c_str() const`.  
- [ ] Use `reserve()` to avoid repeated reallocations.

### 5. `dtoa.c` → `DoubleConverter`
- [ ] Use `std::to_chars` for fast conversion.  
- [ ] Throw `std::runtime_error` on conversion failure.  

### 6. `version.c` → `Version` class
- [ ] Store major, minor, patch as `int`.  
- [ ] Implement `operator==`, `operator<`, and `to_string()`.  
- [ ] Mark methods `constexpr` where possible.

### 7. `pack_unpack.c` → `PackUnpack` namespace
- [ ] Replace raw buffers with `std::vector<uint8_t>` and `std::string_view`.  
- [ ] Use `std::copy_n` and `std::move` for efficiency.

### 8. `error.c` → `JassonError` hierarchy
- [ ] Create base class `class JassonError : public std::exception`.  
- [ ] Derive specific error types (`InvalidSyntaxError`, `OutOfMemoryError`, …).  
- [ ] Provide descriptive `what()` messages.

### 9. `load.c` / `dump.c` → `Loader` / `Dumper`
- [ ] Open files via `std::filesystem::path` and `std::ifstream/std::ofstream`.  
- [ ] Return `std::optional<std::string>` for loaded content.  
- [ ] Throw on I/O errors.

### 10. `utf.c` → `Utf8Util`
- [ ] Consolidate functions as `static std::string normalize(std::string_view)`.  
- [ ] Use `std::u8string_view` for compile‑time UTF‑8 literals.

---

## Phase 5 – Skipping Irrelevant Files
The following paths are **outside** the conversion scope:

* All files under `test/` – existing test suite will be linked against the new library unchanged.  
* Files under `examples/` – they remain as C examples for reference only.  
* Documentation files (`doc/`, `README*`, `*.md` not related to the build) – no conversion needed.  

These files will be copied verbatim into the new repository to preserve history and reference material.

---

## Phase 6 – Validation & Continuous Integration
1. **Compile the library** using the provided CMake configuration.  
2. **Run the existing test suite** (unchanged) to verify functional equivalence.  
3. **Enable static analysis** (`clang-tidy` with modern C++ checks) as part of CI.  
4. **Add a `CODECOVERAGE` target** to measure test coverage after migration.  

---

## Appendices
### A. Example Build Commands
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### B. Coding Standards Checklist
- [ ] Use `enum class` for any enumerated constants.  
- [ ] Prefer `auto` and range‑based loops.  
- [ ] Apply `const`‑correctness on all member functions that do not modify state.  
- [ ] Mark functions `noexcept` when they cannot throw.  
- [ ] Use `std::make_unique` / `std::make_shared` for all dynamic allocations.  
- [ ] Avoid raw `new`/`delete` and `malloc`/`free`.  

---

*End of Plan*  
