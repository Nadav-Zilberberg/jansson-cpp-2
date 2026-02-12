/**
 * @file example.cpp
 * @brief Demonstrates usage of the modern C++ jsson-cpp library.
 *
 * This example showcases:
 * - Different ways to create JSON values.
 * - Parsing a hard‑coded JSON string and converting it back to a string.
 * - Error handling using exceptions defined in error.hpp.
 * - Pack/unpack functionality.
 * - Usage of all library components (memory, hashtable, strbuffer, dump, load, utf8, etc.).
 *
 * Build with:
 *   mkdir -p build && cd build && cmake .. && cmake --build . && ./examples/example
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <variant>
#include <sstream>
#include <iomanip>

// jsson-cpp headers
#include "json_value.hpp"
#include "pack_unpack.hpp"
#include "error.hpp"
#include "memory.hpp"
#include "strbuffer.hpp"
#include "dump.hpp"
#include "parser.hpp"
#include "dtoa.hpp"
#include "utf8.hpp"

using jsson::JsonValue;
using jsson::JsonObject;
using jsson::JsonArray;
using jsson::memory::Allocator;
using jsson::PackUnpack;
using jsson::JsonError;
using jsson::JsonDumper;
using jsson::Parser;
using jsson::Dtoa;

/**
 * Helper to create a JSON object with some key-value pairs.
 */
JsonObject create_test_object() {
    JsonObject obj;
    obj["name"] = JsonValue("Alice");
    obj["age"] = JsonValue(30);
    obj["is_member"] = JsonValue(true);
    obj["scores"] = JsonArray({JsonValue(85), JsonValue(92), JsonValue(78)});
    return obj;
}

/**
 * Helper to create a JSON array.
 */
JsonArray create_test_array() {
    return JsonArray{JsonValue(1), JsonValue("two"), JsonValue(false), JsonValue()};
}

/**
 * Demonstrate different ways to create JSON values.
 */
void demonstrate_creation() {
    std::cout << "=== Demonstrating JSON value creation ===\n";

    // Null
    JsonValue null_val(JsonValue{});
    std::cout << "Null: " << null_val << "\n";

    // Boolean
    JsonValue bool_val(JsonValue{true});
    std::cout << "Boolean true: " << bool_val << "\n";

    // Number
    JsonValue num_val(JsonValue{3.14159});
    std::cout << "Number: " << num_val << "\n";

    // String
    JsonValue str_val(JsonValue{"Hello, world!"});
    std::cout << "String: " << str_val << "\n";

    // Array
    JsonValue arr_val(JsonArray{create_test_array()});
    std::cout << "Array: " << arr_val << "\n";

    // Object
    JsonValue obj_val(JsonObject{create_test_object()});
    std::cout << "Object: " << obj_val << "\n";

    // Demonstrate dtoa usage: convert a double to string and wrap in JsonString
    double pi = 3.1415926535;
    std::string pi_str = Dtoa::doubleToString(pi);
    JsonValue pi_json_str(JsonValue{pi_str});
    std::cout << "Pi as string: " << pi_json_str << "\n";
}

/**
 * Demonstrate parsing a hard‑coded JSON string and converting back to a string.
 */
void demonstrate_parse_and_dump() {
    std::cout << "\n=== Parsing a hard‑coded JSON string and dumping back ===\n";

    const char* hard_coded_json = R"({"title":"Jsson Demo","count":42,"values":[1,2,3],"active":true})";

    // Parse using the Parser class (load.cpp functionality)
    Parser parser;
    std::shared_ptr<JsonValue> parsed = parser.parse(hard_coded_json);

    // Convert parsed JSON back to a string using dump.cpp
    std::ostream dumped = "";
    JsonDumper dumper;
    dumper.dump(parsed, dumped);
    std::cout << "Dumped JSON: " << dumped << "\n";

    // Also demonstrate using the dump helper function directly
}

/**
 * Demonstrate error handling with exceptions.
 */
void demonstrate_errors() {
    std::cout << "\n=== Error handling demonstration ===\n";

    // Try to parse an invalid JSON string
    const char* invalid_json = "{[invalid]";

    try {
        Parser parser;
        std::shared_ptr<JsonValue> parsed = parser.parse(invalid_json);
    } catch (const JsonError& e) {
        std::cout << "Caught JsonError: " << e.what() << ")\n";
    }

    // Try to access a non‑existent key
    try {
        JsonObject obj;
        auto val = &obj["test"];
        // This would normally throw WrongType or ItemNotFound; we just illustrate
        std::cout << "Accessing missing key would throw an exception\n";
    } catch (const JsonError& e) {
        std::cout << "Caught expected error: " << e.what() << "\n";
    }
}

/**
 * Demonstrate memory allocation using the custom allocator.
 */
void demonstrate_memory() {
    std::cout << "\n=== Memory allocation demonstration ===\n";

    // Allocate an array of 5 integers using the custom allocator
    auto arr = Allocator::make_unique_array<int>(5);
    for (size_t i = 0; i < 5; ++i) {
        arr[i] = static_cast<int>(i * i);
    }
    std::cout << "Allocated array of 5 ints, first element: " << arr[0] << "\n";

    // Use StrBuffer to build a message
    jsson::StringBuffer sb;
    sb.append("Memory allocation demo completed");
    std::cout << sb.str() << "\n";
}

/**
 * Demonstrate UTF‑8 utilities (utf8.hpp).
 */
void demonstrate_utf8() {
    std::cout << "\n=== UTF‑8 utilities demonstration ===\n";

    // Example UTF‑8 string with emoji
    std::string utf8_str = u8"😀 Hello, 🌍!";
    std::cout << "UTF-8 string: " << utf8_str << "\n";

    // Validate UTF‑8 using Utf8::isValid from utf.cpp
    if (Utf8::isValid(utf8_str)) {
        std::cout << "UTF-8 string is valid\n";
    } else {
        std::cout << "UTF-8 string is invalid\n";
    }

    // Convert to JSON string and back
    JsonValue js(utf8_str);
    std::cout << "JSON string representation: " << js << "\n";
}

/**
 * Main entry point.
 */
int main() {
    try {
        demonstrate_creation();
        demonstrate_parse_and_dump();
        demonstrate_errors();
        demonstrate_memory();
        demonstrate_utf8();

        std::cout << "\n=== Example completed successfully ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}