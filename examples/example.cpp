#!/usr/bin/env cpp
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
#include "jsson-cpp/json_value.hpp"
#include "jsson-cpp/pack_unpack.hpp"
#include "jsson-cpp/error.hpp"
#include "jsson.hpp"
#include "jsson-cpp/memory.hpp"
#include "jsson-cpp/strbuffer.hpp"
#include "jsson-cpp/dump.hpp"
#include "jsson-cpp/parser.hpp"
#include "jsson-cpp/dtoa.hpp"
#include "jsson-cpp/utf8.hpp"

using jsson::json_value::JsonValue;
using jsson::json_value::JsonObject;
using jsson::json_value::JsonArray;
using jsson::json_value::JsonNumber;
using jsson::json_value::JsonString;
using jsson::json_value::JsonBoolean;
using jsson::json_value::JsonNull;
using jsson::memory::Allocator;
using jsson::pack_unpack::PackUnpack;
using jsson::error::JsonError;
using jsson::hashtable::JsonStringHashTable;
using jsson::dump::JsonDumper;
using jsson::parser::Parser;
using jsson::dtoa::Dtoa;

/**
 * Helper to create a JSON object with some key-value pairs.
 */
JsonObject create_test_object() {
    JsonObject obj;
    obj.emplace("name", JsonString("Alice"));
    obj.emplace("age", JsonNumber(30));
    obj.emplace("is_member", JsonBoolean(true));
    obj.emplace("scores", JsonArray{JsonNumber(85), JsonNumber(92), JsonNumber(78)});
    return obj;
}

/**
 * Helper to create a JSON array.
 */
JsonArray create_test_array() {
    return JsonArray{JsonNumber(1), JsonString("two"), JsonBoolean(false), JsonNull()};
}

/**
 * Demonstrate different ways to create JSON values.
 */
void demonstrate_creation() {
    std::cout << "=== Demonstrating JSON value creation ===\n";

    // Null
    JsonValue null_val(JsonNull{});
    std::cout << "Null: " << null_val << "\n";

    // Boolean
    JsonValue bool_val(JsonBoolean{true});
    std::cout << "Boolean true: " << bool_val << "\n";

    // Number
    JsonValue num_val(JsonNumber{3.14159});
    std::cout << "Number: " << num_val << "\n";

    // String
    JsonValue str_val(JsonString{"Hello, world!"});
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
    JsonValue pi_json_str(JsonString{pi_str});
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
    if (!parsed) {
        throw JsonError("Failed to parse JSON", parser.get_error_code());
    }

    // Convert parsed JSON back to a string using dump.cpp
    std::string dumped;
    JsonDumper dumper;
    dumper.dump(*parsed, std::back_inserter(dumped));
    std::cout << "Dumped JSON: " << dumped << "\n";

    // Also demonstrate using the dump helper function directly
    std::ostringstream oss;
    JsonDumper::dump(*parsed, std::ostringstream_iterator(oss));
    std::cout << "Dumped via helper: " << oss.str() << "\n";
}

/**
 * Demonstrate pack/unpack functionality.
 */
void demonstrate_pack_unpack() {
    std::cout << "\n=== Pack/unpack demonstration ===\n";

    // Create a JSON number
    JsonValue val(JsonNumber{123.45});
    std::cout << "Original value: " << val << "\n";

    // Pack it into a double
    double packed = PackUnpack::pack<double>(val);
    std::cout << "Packed as double: " << packed << "\n";

    // Unpack it back to a JSON value
    JsonValue unpacked = PackUnpack::unpack(packed);
    std::cout << "Unpacked value: " << unpacked << "\n";
}

/**
 * Demonstrate hashtable usage.
 */
void demonstrate_hashtable() {
    std::cout << "\n=== Hashtable demonstration ===\n";

    JsonStringHashTable table;
    auto obj = create_test_object();

    // Insert key-value pairs into the hashtable via JsonValue's object storage
    for (const auto& [key, value] : obj) {
        table.insert(key, value.get()); // Simplified usage for demo
    }

    std::cout << "Hashtable size: " << table.size() << "\n";
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
        if (!parsed) {
            throw JsonError("Parsing failed", parser.get_error_code());
        }
    } catch (const JsonError& e) {
        std::cout << "Caught JsonError: " << e.what() << " (code: " << static_cast<int>(e.get_code()) << ")\n";
    }

    // Try to access a non‑existent key
    try {
        JsonValue obj(JsonObject{});
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
    if (!arr) {
        throw JsonError("Memory allocation failed", jsson::error::JsonErrorCode::OutOfMemory);
    }
    for (size_t i = 0; i < 5; ++i) {
        arr[i] = static_cast<int>(i * i);
    }
    std::cout << "Allocated array of 5 ints, first element: " << arr[0] << "\n";

    // Use StrBuffer to build a message
    jsson::strbuffer::StringBuffer sb;
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
    std::cout << "UTF‑8 string: " << utf8_str << "\n";

    // Validate UTF‑8 using Utf8::isValid from utf.cpp
    if (Utf8::isValid(utf8_str)) {
        std::cout << "UTF‑8 string is valid\n";
    } else {
        std::cout << "UTF‑8 string is invalid\n";
    }

    // Convert to JSON string and back
    JsonString js(utf8_str);
    std::cout << "JSON string representation: " << js << "\n";
}

/**
 * Main entry point.
 */
int main() {
    try {
        demonstrate_creation();
        demonstrate_parse_and_dump();
        demonstrate_pack_unpack();
        demonstrate_hashtable();
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