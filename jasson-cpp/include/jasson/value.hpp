#ifndef JASSON_VALUE_HPP
#define JASSON_VALUE_HPP

#include <variant>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstddef>

namespace jasson {

enum class ValueType : int {
    Null,
    Boolean,
    Integer,
    Double,
    String,
    Object,
    Array
};

class Value;

/**
 * Forward declaration of Value for shared_ptr usage.
 */
using ValuePtr = std::shared_ptr<Value>;

class Value {
public:
    // Constructors
    Value() : type_(ValueType::Null) {}
    Value(bool value) : type_(ValueType::Boolean), boolean_(value) {}
    Value(int value) : type_(ValueType::Integer), integer_(value) {}
    Value(double value) : type_(ValueType::Double), double_(value) {}
    Value(const char* value) : type_(ValueType::String), string_(value) {}
    Value(const std::string& value) : type_(ValueType::String), string_(value) {}
    Value(const char* value, size_t length) : type_(ValueType::String) {
        string_.append(value, length);
    }
    Value(std::vector<ValuePtr> array) : type_(ValueType::Array), array_(std::move(array)) {}
    Value(std::vector<ValuePtr> object) : type_(ValueType::Object), object_(std::move(object)) {}

    // Destructor
    ~Value() = default;

    // Copy and move semantics
    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;
    Value(Value&&) = default;
    Value& operator=(Value&&) = default;

    // Type accessor
    ValueType type() const noexcept { return type_; }

    // Value accessors that may throw std::bad_variant_access
    bool as_bool() const {
        if (type_ != ValueType::Boolean) throw std::bad_variant_access();
        return boolean_;
    }

    int as_integer() const {
        if (type_ != ValueType::Integer) throw std::bad_variant_access();
        return integer_;
    }

    double as_double() const {
        if (type_ != ValueType::Double) throw std::bad_variant_access();
        return double_;
    }

    const std::string& as_string() const {
        if (type_ != ValueType::String) throw std::bad_variant_access();
        return string_;
    }

    const std::vector<ValuePtr>& as_array() const {
        if (type_ != ValueType::Array) throw std::bad_variant_access();
        return array_;
    }

    const std::vector<ValuePtr>& as_object() const {
        if (type_ != ValueType::Object) throw std::bad_variant_access();
        return object_;
    }

    // Null value helper
    static ValuePtr NullValue() {
        static ValuePtr null_value = std::make_shared<Value>();
        null_value->type_ = ValueType::Null;
        return null_value;
    }

private:
    ValueType type_;
    bool boolean_;
    int integer_;
    double double_;
    std::string string_;
    std::vector<ValuePtr> array_;
    std::vector<ValuePtr> object_;
};

} // namespace jasson

#endif // JASSON_VALUE_HPP
