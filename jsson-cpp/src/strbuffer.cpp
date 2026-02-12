#include "strbuffer.hpp"
#include <algorithm>
#include <cstring>

using namespace jsson;

StringBuffer::StringBuffer(size_t initial_size)
    : buffer_(), size_(0) {
    if (initial_size > 0) {
        buffer_.resize(initial_size, '\0');
        buffer_[0] = '\0';
    }
}

StringBuffer& StringBuffer::append(char c) {
    return append(&c, 1);
}

StringBuffer& StringBuffer::append(const char* data, size_t size) {
    if (size > 0) {
        ensure_capacity(size_ + size + 1);
        std::memcpy(&buffer_[size_], data, size);
        size_ += size;
        buffer_[size_] = '\0';
    }
    return *this;
}

StringBuffer& StringBuffer::prepend(char c) {
    return prepend(&c, 1);
}

StringBuffer& StringBuffer::prepend(const char* data, size_t size) {
    if (size > 0) {
        ensure_capacity(size_ + size + 1);
        // Shift existing characters to the right
        std::memmove(&buffer_[size_ + size], &buffer_[size_], 1); // move null terminator
        std::memcpy(&buffer_[0], data, size);
        size_ += size;
        buffer_[size_] = '\0';
    }
    return *this;
}

StringBuffer& StringBuffer::remove(size_t index, size_t count) {
    if (index >= size_) return *this;
    if (count >= size_ - index) {
        count = size_ - index;
    }
    // Shift left
    std::memmove(&buffer_[index], &buffer_[index + count], size_ - index - count + 1);
    size_ -= count;
    buffer_[size_] = '\0';
    return *this;
}

const char* StringBuffer::value() const {
    return buffer_.c_str();
}

std::string StringBuffer::str() const {
    return buffer_;
}

size_t StringBuffer::size() const {
    return size_;
}

void StringBuffer::clear() {
    size_ = 0;
    buffer_[0] = '\0';
}

std::string StringBuffer::steal() {
    buffer_[size_] = '\0';
    return std::move(buffer_);
}

void StringBuffer::ensure_capacity(size_t min_capacity) {
    if (buffer_.capacity() >= min_capacity) return;
    // Reserve new capacity, preserving existing content and null terminator
    std::string new_buffer;
    new_buffer.reserve(min_capacity);
    new_buffer.append(buffer_, size_); // copy existing characters
    new_buffer.push_back('\0');        // add null terminator
    buffer_.swap(new_buffer);
}