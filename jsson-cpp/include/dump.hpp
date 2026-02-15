#ifndef DUMP_HPP
#define DUMP_HPP

#include <memory>
#include <variant>
#include <ostream>
#include "json_value.hpp"

namespace jsson {

/**
 * @brief Class for dumping JSON values to output streams.
 *
 * This class encapsulates the dumping functionality using modern C++
 * idioms: RAII, std::visit, range-based for loops, and const-correctness.
 */
class JsonDumper {
public:
    /**
     * @brief Dump a JSON value to an output stream.
     *
     * @param value The JSON value to dump.
     * @param out   The output stream to write to.
     */
    void dump(const std::shared_ptr<JsonValue>& value, std::ostream& out) const;

private:
    /**
     * @brief Helper visitor struct for std::visit.
     */
    struct DumpVisitor;
    /**
     * @brief Recursively dump a JSON value using the visitor.
     */
    void dumpValue(const std::shared_ptr<JsonValue>& value, std::ostream& out) const;
};

} // namespace dump

/**
 * @brief Serialize a JSON value to an output stream.
 *
 * This operator provides a human-readable representation of a JSON value.
 *
 * @param out The output stream.
 * @param value The JSON value to serialize.
 * @return The output stream reference.
 */
inline std::ostream& operator<<(std::ostream& out, const std::shared_ptr<JsonValue>& value) {
    JsonDumper dumper;
    dumper.dump(value, out);
    return out;
}

/**
 * @brief Serialize a JSON value to an output stream.
 *
 * This operator provides a human-readable representation of a JSON value.
 *
 * @param out The output stream.
 * @param value The JSON value to serialize.
 * @return The output stream reference.
 */
inline std::ostream& operator<<(std::ostream& out, const jsson::JsonValue& value) {
    JsonDumper dumper;
    dumper.dump(std::make_shared<const jsson::JsonValue>(value), out);
    return out;
}

/**
 * @brief Serialize a JSON object to an output stream.
 *
 * This operator provides a human-readable representation of a JSON object.
 *
 * @param out The output stream.
 * @param obj The JSON object to serialize.
 * @return The output stream reference.
 */
inline std::ostreaminline std::ostream& operator<<(std::ostream& out, const jsson::JsonObjectconst JsonObject& obj obj) { operator<<(std::ostreaminline std::ostream& operator<<(std::ostream& out, const jsson::JsonObjectconst JsonObject& obj obj) { out, const jsson::JsonObjectinline std::ostream& operator<<(std::ostream& out, const jsson::JsonObjectconst JsonObject& obj obj) { obj) {
    JsonDumper dumper;
    dumper.dump(std::make_shared<const JsonObject>(obj), out);
    return out;
}

/**
 * @brief Serialize a JSON array to an output stream.
 *
 * This operator provides a human-readable representation of a JSON array.
 *
 * @param out The output stream.
 * @param arr The JSON array to serialize.
 * @return The output stream reference.
 */
inline std::ostream& operator<<(std::ostream& out, const jsson::JsonArrayconst JsonArray& arr arr) {
    JsonDumper dumper;
    dumper.dump(std::make_shared<const JsonArray>(arr), out);
    return out;
}

#endif // DUMP_HPP