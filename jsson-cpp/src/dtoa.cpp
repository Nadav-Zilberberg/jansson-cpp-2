#include "dtoa.hpp"
#include <sstream>
#include <iomanip>

namespace jsson {

std::string Dtoa::doubleToString(double value) noexcept {
    // Use ostringstream for precise control over formatting
    std::ostringstream oss;
    oss << std::setprecision(15) << value;
    return oss.str();
}

} // namespace jsson