/**
 * @file example.cpp
 * @brief Demonstrates usage of the jsson-cpp library.
 *
 * This example shows:
 * - Various ways to create JSON values
 * - Conversion from a JSON string to a JSON object and back
 * - Error handling through exceptions
 * - Pack/unpack functionality for binary serialization
 *
 * Build the example with the provided CMakeLists.txt in the examples directory.
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstddef> // for std::byte

// jsson headers
#include <jsson/json_value.hpp>
#include <jsson/pack_unpack.hpp>
#include <jsson/error.hpp>

using namespace jsson;

/**
 * @brief Helper function to print a JSON value.
 *
 * @param val The JSON value to print.
 */
void printJson(const JsonValue& val) {
    std::cout << "JSON value (type: " << val.type() << "): " << val.toString() << std::endl;
}

/**
 * @brief Demonstrates creating JSON values in various ways.
 */
void demonstrateCreation() {
    std::cout << "=== Demonstrating JSON value creation ===" << std::endl;

    // Null value
    JsonValue nullVal;
    printJson(nullVal);

    // Boolean
    JsonValue boolVal(true);
    printJson(boolVal);

    // Number (int)
    JsonValue intVal(42);
    printJson(intVal);

    // Number (double)
    JsonValue doubleVal(3.14);
    printJson(doubleVal);

    // String (copy)
    JsonValue strVal("hello");
    printJson(strVal);

    // String (move)
    JsonValue strValMove(std::string("world"));
    printJson(strValMove);

    // Object
    JsonObject obj;
    obj["name"] = "Alice";
    obj["age"] = 30;
    JsonValue objVal(obj);
    printJson(objVal);

    // Array
    JsonArray arr;
    arr.push_back(JsonValue(1));
    arr.push_back(JsonValue(2));
    JsonValue arrVal(arr);
    printJson(arrVal);
}

/**
 * @brief Demonstrates converting a JSON string to a JSON object and back.
 *
 * Note: This example assumes that the JSON string is parsed into a JsonValue.
 * The parsing functionality is provided by the JsonValue constructor that
 * accepts a string containing valid JSON.
 */
void demonstrateStringConversion() {
    std::cout << "\n=== Demonstrating string conversion ===" << std::endl;

    // Hard-coded JSON string
    const std::string hardcodedJson = "{\"title\":\"Example\",\"count\":123,\"active\":true}";
    
    // Parse the JSON string into a JsonValue
    JsonValue parsedJson(hardcodedJson);
    std::cout << "Parsed JSON: " << parsedJson.toString() << std::endl;

    // Convert back to a string representation
    std::string reconstructed = parsedJson.toString();
    std::cout << "Reconstructed JSON: " << reconstructed << std::endl;
}

/**
 * @brief Demonstrates error handling with exceptions.
 *
 * This function intentionally triggers a few error scenarios and catches
 * the resulting exceptions.
 */
void demonstrateErrors() {
    std::cout << "\n=== Demonstrating error handling ===" << std::endl;

    // Create an object
    JsonObject obj;
    obj["key"] = JsonValue(123);

    // Create a JsonValue from the object
    JsonValue val(obj);

    // Try to access as a number (should throw)
    try {
        val.asNumber();
    } catch (const std::exception& e) {
        std::cout << "Caught exception (asNumber): " << e.what() << std::endl;
    }

    // Try to access as a boolean (should throw)
    try {
        val.asBoolean();
    } catch (const std::exception& e) {
        std::cout << "Caught exception (asBoolean): " << e.what() << std::endl;
    }

    // Try to access a non-existing key in an object that is not an object
    JsonValue nonObjectVal(42); // number
    try {
        // This will throw because the JsonValue is not an object
        nonObjectVal.asObject();
    } catch (const std::exception& e) {
        std::cout << "Caught exception (asObject on non-object): " << e.what() << std::endl;
    }
}

/**
 * @brief Demonstrates pack/unpack functionality.
 *
 * This function packs a string value into a binary buffer and then unpacks
 * it back to a string, showing that the round-trip works correctly.
 */
void demonstratePackUnpack() {
    std::cout << "\n=== Demonstrating pack/unpack ===" << std::endl;

    // Pack a string
    size_t pos = 0;
    auto buffer = PackUnpack::pack(BinaryType::String, std::string("packed_string"));
    std::cout << "Packed buffer size: " << buffer.size() << " bytes" << std::endl;

    // Unpack the string back
    std::string unpacked = PackUnpack::unpack<std::string>(buffer, pos);
    std::cout << "Unpacked string: " << unpacked << std::endl;

    // Try to pack an unsupported type (should throw)
    try {
        PackUnpack::pack(BinaryType::Integer, std::string("not_an_integer"));
    } catch (const std::exception& e) {
        std::cout << "Caught exception (unsupported pack): " << e.what() << std::endl;
    }
}

int main() {
    try {
        demonstrateCreation();
        demonstrateStringConversion();
        demonstrateErrors();
        demonstratePackUnpack();
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\n=== Example finished successfully ===" << std::endl;
    return EXIT_SUCCESS;
}