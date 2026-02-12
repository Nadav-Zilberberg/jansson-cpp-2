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

#endif // DUMP_HPP