#include <iostream>
#include <memory>
#include <vector>
#include <optional>
#include <string>
#include <filesystem>
#include <cstddef>

#include "jsson-cpp/include/error.hpp"
#include "jsson-cpp/include/json_value.hpp"
#include "jsson-cpp/include/parser.hpp"
#include "jsson-cpp/include/pack_unpack.hpp"
#include "jsson-cpp/include/memory.hpp"
#include "jsson-cpp/include/hashtable.hpp"
#include "jsson-cpp/include/strbuffer.hpp"

using namespace jsson;

// Helper function to dump a JsonValue to a string
std::string dump_to_string(const JsonValue& value) {
    // Use StringBuffer to build the string representation
    StringBuffer sb(64);
    // We can use JsonDumper to dump the value into the buffer
    // For simplicity, we'll use the toString method if available
    // If not, we can manually dump using a simple approach
    // Here we assume JsonValue has a dump method that writes to an output stream
    // We'll use a simple approach: convert to string using dump
    std::ostringstream oss;
    JsonDumper dumper(oss);
    dumper.dump(value);
    return oss.str();
}

// Helper function to create a JsonValue from various types
JsonValue create_json_from_various_types() {
    // Null
    JsonValue null_val(JsonValue::Type::Null);
    
    // Boolean
    JsonValue bool_val(JsonValue::Type::Boolean, true);
    
    // Integer
    JsonValue int_val(JsonValue::Type::Integer, 42);
    
    // Real (double)
    JsonValue double_val(JsonValue::Type::Real, 3.14159);
    
    // String
    JsonValue string_val(JsonValue::Type::String, std::string("Hello, World!"));
    
    // Array
    JsonValue array_val(JsonValue::Type::Array);
    array_val.emplace_back(JsonValue::Type::Integer, 1);
    array_val.emplace_back(JsonValue::Type::String, "two");
    array_val.emplace_back(JsonValue::Type::Boolean, false);
    
    // Object
    JsonValue object_val(JsonValue::Type::Object);
    object_val.emplace_back("key1", JsonValue::Type::Integer, 10);
    object_val.emplace_back("key2", JsonValue::Type::String, "value2");
    
    return object_val; // Return the object for demonstration
}

// Helper function to demonstrate pack/unpack
void demonstrate_pack_unpack() {
    std::cout << "=== Pack/Unpack Demo ===" << std::endl;
    
    // Pack an integer
    Buffer packed_int = PackUnpack::pack(BinaryType::Integer, static_cast<int64_t>(123));
    size_t pos = 0;
    int64_t unpacked_int = PackUnpack::unpack<int64_t>(packed_int, pos);
    std::cout << "Packed and unpacked integer: " << unpacked_int << std::endl;
    
    // Pack a string
    Buffer packed_str = PackUnpack::pack(BinaryType::String, std::string("example"));
    size_t pos2 = 0;
    std::string unpacked_str = PackUnpack::unpack<std::string>(packed_str, pos2);
    std::cout << "Packed and unpacked string: " << unpacked_str << std::endl;
    
    // Pack a boolean
    Buffer packed_bool = PackUnpack::pack(BinaryType::Boolean, true);
    size_t pos3 = 0;
    bool unpacked_bool = PackUnpack::unpack<bool>(packed_bool, pos3);
    std::cout << "Packed and unpacked boolean: " << std::boolalpha << unpacked_bool << std::endl;
}

int main() {
    std::cout << "=== Jsson-CPP Example ===" << std::endl;
    
    // 1. Different ways to create a json
    std::cout << "\n--- Creating JsonValues ---" << std::endl;
    
    // Direct construction
    JsonValue null_val(JsonValue::Type::Null);
    JsonValue bool_val(JsonValue::Type::Boolean, false);
    JsonValue int_val(JsonValue::Type::Integer, 123);
    JsonValue double_val(JsonValue::Type::Real, 2.718);
    JsonValue string_val(JsonValue::Type::String, std::string("test"));
    
    // Array construction
    JsonValue array_val(JsonValue::Type::Array);
    array_val.emplace_back(JsonValue::Type::Integer, 1);
    array_val.emplace_back(JsonValue::Type::String, "two");
    array_val.emplace_back(JsonValue::Type::Boolean, true);
    
    // Object construction
    JsonValue object_val(JsonValue::Type::Object);
    object_val.emplace_back("name", JsonValue::Type::String, "Alice");
    object_val.emplace_back("age", JsonValue::Type::Integer, 30);
    object_val.emplace_back("is_student", JsonValue::Type::Boolean, false);
    
    std::cout << "Null value: " << dump_to_string(null_val) << std::endl;
    std::cout << "Boolean value: " << dump_to_string(bool_val) << std::endl;
    std::cout << "Integer value: " << dump_to_string(int_val) << std::endl;
    std::cout << "Double value: " << dump_to_string(double_val) << std::endl;
    std::cout << "String value: " << dump_to_string(string_val) << std::endl;
    std::cout << "Array value: " << dump_to_string(array_val) << std::endl;
    std::cout << "Object value: " << dump_to_string(object_val) << std::endl;
    
    // 2. Create a json from a hard coded string and convert it back into a string
    std::cout << "\n--- Parse Hardcoded JSON String ---" << std::endl;
    
    // Create a temporary file with JSON content
    std::filesystem::path temp_file = std::filesystem::temp_directory_path() / "example.json";
    {
        std::ofstream ofs(temp_file);
        ofs << R"({"name":"John","age":30,"city":"New York"})";
    }
    
    // Parse the file using Parser
    Parser parser;
    try {
        std::unique_ptr<JsonValue> parsed_json = parser.parse(temp_file.string());
        // Convert back to string
        std::string json_string = parsed_json->dump();
        std::cout << "Parsed JSON string: " << json_string << std::endl;
    } catch (const JsonError& e) {
        std::cout << "Error parsing JSON: " << e.what() << std::endl;
    }
    
    // Clean up temp file
    std::filesystem::remove(temp_file);
    
    // 3. Test all different kinds of errors
    std::cout << "\n--- Error Handling Demo ---" << std::endl;
    
    // Test error codes by attempting operations that may throw
    try {
        // Try to parse a non-existent file
        Parser parser_invalid;
        std::unique_ptr<JsonValue> invalid_json = parser_invalid.parse("nonexistent.json");
    } catch (const JsonError& e) {
        std::cout << "Caught JsonError: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
    
    // Try to parse invalid syntax (we can create a malformed JSON file)
    std::filesystem::path malformed_file = std::filesystem::temp_directory_path() / "malformed.json";
    {
        std::ofstream ofs(malformed_file);
        ofs << R"({"invalid": json})"; // Missing closing brace
    }
    
    try {
        Parser parser_syntax;
        std::unique_ptr<JsonValue> syntax_json = parser_syntax.parse(malformed_file.string());
    } catch (const JsonError& e) {
        std::cout << "Caught JsonError (syntax): " << e.what() << std::endl;
    }
    
    std::filesystem::remove(malformed_file);
    
    // 4. Demonstrate pack and unpack functionality
    demonstrate_pack_unpack();
    
    // 5. Use exceptions via error.hpp
    // Already demonstrated above with catch blocks
    
    // 6. Demonstrate hashtable usage
    std::cout << "\n--- Hashtable Demo ---" << std::endl;
    Hashtable<std::string, std::string> table;
    table.insert("key1", "value1");
    table.insert("key2", "value2");
    auto found = table.find("key1");
    if (found) {
        std::cout << "Found value for key1: " << found->second << std::endl;
    }
    
    // 7. Demonstrate strbuffer usage
    std::cout << "\n--- StringBuffer Demo ---" << std::endl;
    StringBuffer sb(32);
    sb.append("Hello");
    sb.append(" ");
    sb.append("World");
    std::cout << "StringBuffer content: " << sb.str() << std::endl;
    
    std::cout << "\n=== Example Completed Successfully ===" << std::endl;
    return 0;
}