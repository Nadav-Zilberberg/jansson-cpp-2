#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>
#include <sstream>
#include <stdexcept>
#include <optional>

/* Forward declarations */
class JsonObject;
class JsonArray;

/**
 * @brief Class representing any JSON value (object, array, number, string, etc.).
 *
 * This class encapsulates a JSON value using a `std::variant` that can hold
 * concrete types: object, array, number, string, boolean, or null.
 * Composite values (objects and arrays) are stored via `std::unique_ptr`.
 */
class JsonValue {
public:
    /** Enumerate possible JSON value types. */
    enum class Type {
        Null,
        Boolean,
        Number,
        String,
        Object,
        Array
    };

    /*=====================================================================
     *  Constructors / Assignment
     *====================================================================*/

    // Default ctor creates a null value
    JsonValue() : type_(Type::Null), data_(std::monostate{}) {}

    // Boolean
    explicit JsonValue(bool value) : type_(Type::Boolean), data_(value) {}

    // Number (double)
    explicit JsonValue(double value) : type_(Type::Number), data_(value) {}

    // Integer (int64_t)
    explicit JsonValue(int64_t value) : type_(Type::Number), data_(value) {}

    // String (copy)
    explicit JsonValue(const std::string& value) : type_(Type::String), data_(value) {}

    // String (move)
    explicit JsonValue(std::string&& value) : type_(Type::String), data_(std::move(value)) {}

    // Object (copy)
    explicit JsonValue(const JsonObject& object) : type_(Type::Object), data_(std::make_unique<JsonObject>(object)) {}

    // Object (move)
    explicit JsonValue(JsonObject&& object) : type_(Type::Object), data_(std::make_unique<JsonObject>(std::move(const_cast<JsonObject&>(object)))) {}

    // Array (copy)
    explicit JsonValue(const JsonArray& array) : type_(Type::Array), data_(std::make_unique<JsonArray>(array)) {}

    // Array (move)
    explicit JsonValue(JsonArray&& array) : type_(Type::Array), data_(std::make_unique<JsonArray>(std::move(const_cast<JsonArray&>(array)))) {}

    // Copy / Move (defaulted)
    JsonValue(const JsonValue&) = default;
    JsonValue(JsonValue&&) = default;
    JsonValue& operator=(const JsonValue&) = default;
    JsonValue& operator=(JsonValue&&) = default;

    /*=====================================================================
     *  Type Inquiry
     *====================================================================*/

    /** @return The JSON value type. */
    Type type() const noexcept { return type_; }

    /** @return true if the value is null. */
    bool isNull() const noexcept { return type_ == Type::Null; }

    /** @return true if the value is a boolean. */
    bool isBoolean() const noexcept { return type_ == Type::Boolean; }

    /** @return true if the value is a number. */
    bool isNumber() const noexcept { return type_ == Type::Number; }

    /** @return true if the value is a string. */
    bool isString() const noexcept { return type_ == Type::String; }

    /** @return true if the value is an object. */
    bool isObject() const noexcept { return type_ == Type::Object; }

    /** @return true if the value is an array. */
    bool isArray() const noexcept { return type_ == Type::Array; }

    /*=====================================================================
     *  Accessors
     *====================================================================*/

    /** @return Reference to the contained object. Throws if not an object. */
    std::reference_wrapper<JsonObject>& asObject();

    /** @return Const reference to the contained object. Throws if not an object. */
    std::reference_wrapper<const JsonObject>& asObject() const;

    /** @return Reference to the contained array. Throws if not an array. */
    std::reference_wrapper<JsonArray>& asArray();

    /** @return Const reference to the contained array. Throws if not an array. */
    std::reference_wrapper<const JsonArray>& asArray() const;

    /** @return The boolean value. Throws if not a boolean. */
    bool asBoolean() const;

    /** @return The double value. Throws if not a number. */
    double asNumber() const;

    /** @return Reference to the underlying string. Throws if not a string. */
    const std::string& asString() const;

    /** @return Reference to the underlying string (non‑const). Throws if not a string. */
    std::string& asString();

    /**
     * @brief Convert the JSON value to its string representation.
     * @return A string containing the JSON representation.
     */
    std::string toString() const;

    /*=====================================================================
     *  Assignment Operators for Each Type
     *====================================================================*/

    JsonValue& operator=(bool value);
    JsonValue& operator=(double value);
    JsonValue& operator=(int64_t value);
    JsonValue& operator=(const std::string& value);
    JsonValue& operator=(std::string&& value);
    JsonValue& operator=(const JsonObject& object);
    JsonValue& operator=(JsonObject&& object);
    JsonValue& operator=(const JsonArray& array);
    JsonValue& operator=(JsonArray&& array);

private:
    Type type_;
    std::variant<std::monostate, bool, double, int64_t, std::string,
                 std::unique_ptr<JsonObject>, std::unique_ptr<JsonArray>> data_;
};

/*=====================================================================
 *  JsonObject Declaration
 *====================================================================*/

class JsonObject {
public:
    using Map = std::unordered_map<std::string, JsonValue>;

    JsonObject() = default;
    explicit JsonObject(const Map& map) : data_(map) {}
    explicit JsonObject(Map&& map) : data_(std::move(map)) {}

    /** @return Reference to the value associated with @p key. */
    JsonValue& operator[](const std::string& key);

    /** @return Const reference to the value associated with @p key. */
    const JsonValue& operator[](const std::string& key) const;

    /** @return The underlying map (for iteration). */
    Map& keys();

    /** @return The underlying map (const). */
    const Map& keys() const;

private:
    Map data_;
};

/*=====================================================================
 *  JsonArray Declaration
 *====================================================================*/

class JsonArray {
public:
    using Vec = std::vector<JsonValue>;

    JsonArray() = default;
    explicit JsonArray(Vec&& vec) : data_(std::move(vec)) {}

    /** @return Reference to the element at @p index. */
    JsonValue& operator[](size_t index);

    /** @return Const reference to the element at @p index. */
    const JsonValue& operator[](size_t index) const;

    /** @return The underlying vector (for iteration). */
    Vec& data();

    /** @return The underlying vector (const). */
    const Vec& data() const;

private:
    Vec data_;
};

#endif // JSON_VALUE_HPP