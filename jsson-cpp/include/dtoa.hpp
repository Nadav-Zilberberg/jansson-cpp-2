#ifndef DTOA_HPP
#define DTOA_HPP

#include <string>
#include <sstream>
#include <iomanip>

namespace jsson {

/**
 * @brief Utility class for converting double values to string representation.
 *
 * This class encapsulates the functionality previously implemented in
 * dtoa.c, providing a modern C++ interface with RAII and proper error handling.
 */
class Dtoa {
public:
    /**
     * @brief Convert a double to its string representation.
     *
     * @param value The double value to convert.
     * @return std::string The string representation of the double.
     *
     * The conversion uses std::ostringstream with a precision of 15 decimal places,
     * which matches the typical precision needed for double values.
     */
    static std::string doubleToString(double value) noexcept;
};

} // namespace jsson

#endif // DTOA_HPP