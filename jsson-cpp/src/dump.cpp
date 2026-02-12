/*
 * Copyright (c) 2009-2016 Petri Lehtinen <petri@digip.org>
 *
 * Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include "json_value.hpp"
#include <memory>
#include <variant>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace dump {

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
    void dump(const std::shared_ptr<jsson::JsonValue>& value, std::ostream& out) const {
        dumpValue(value, out);
    }

private:
    /**
     * @brief Helper visitor struct for std::visit.
     */
    struct DumpVisitor {
        std::ostream& out;
        DumpVisitor(std::ostream& o) : out(o) {}

        void operator()(const std::monostate&) const {
            out << "null";
        }

        void operator()(bool b) const {
            out << (b ? "true" : "false");
        }

        void operator()(double d) const {
            out << d;
        }

        void operator()(int64_t i) const {
            out << i;
        }

        void operator()(const std::string& s) const {
            out << '"' << escape(s) << '"';
        }

        void operator()(const char* s) const {
            out << '"' << escape(s) << '"';
        }

        void operator()(const std::unique_ptr<jsson::JsonObject>& obj) const {
            dumpObject(obj->keys(), out);
        }

        void operator()(const std::unique_ptr<jsson::JsonArray>& arr) const {
            dumpArray(arr->data(), out);
        }

    private:
        static std::string escape(const std::string& s) {
            std::string result;
            for (char c : s) {
                if (c == '\\' || c == '"') result += '\\';
                result += c;
            }
            return result;
        }

        /**
         * @brief Dump the contents of a JSON object.
         */
        void dumpObject(const jsson::JsonObject::Map& map, std::ostream& out) const {
            out << '{';
            bool first = true;
            for (const auto& kv : map) {
                if (!first) out << ", ";
                first = false;
                out << '"' << escape(kv.first) << "\": ";
                dumpValue(kv.second, out);
            }
            out << '}';
        }

        /**
         * @brief Dump the contents of a JSON array.
         */
        void dumpArray(const jsson::JsonArray::Vec& vec, std::ostream& out) const {
            out << '[';
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i) out << ", ";
                dumpValue(vec[i], out);
            }
            out << ']';
        }

    };

    /**
     * @brief Recursively dump a JSON value using the visitor.
     */
    static void dumpValue(const std::shared_ptr<jsson::JsonValue>& value, std::ostream& out) {
        DumpVisitor visitor(out);
        std::visit(visitor, value->raw_variant());
    }
};

} // namespace dump