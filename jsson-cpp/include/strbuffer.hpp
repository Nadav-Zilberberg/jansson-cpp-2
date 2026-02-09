#ifndef JSSON_STRBUFFER_HPP
#define JSSON_STRBUFFER_HPP

#include <string>
#include <cstddef>
#include <algorithm>

class StringBuffer {
public:
    explicit StringBuffer(size_t initial_size = 16);
    ~StringBuffer() = default;

    // Disable copy
    StringBuffer(const StringBuffer&) = delete;
    StringBuffer& operator=(const StringBuffer&) = delete;

    // Allow move
    StringBuffer(StringBuffer&&) noexcept = default;
    StringBuffer& operator=(StringBuffer&&) noexcept = default;

    // Append methods
    StringBuffer& append(char c);
    StringBuffer& append(const char* data, size_t size);

    // Prepend methods
    StringBuffer& prepend(char c);
    StringBuffer& prepend(const char* data, size_t size);

    // Remove characters
    StringBuffer& remove(size_t index, size_t count);

    // Accessors
    const char* value() const;
    std::string str() const;
    size_t size() const;
    void clear();

    // Move out the internal buffer
    std::string steal();

private:
    std::string buffer_;

    void ensure_capacity(size_t min_capacity);
};

#endif // JSSON_STRBUFFER_HPP