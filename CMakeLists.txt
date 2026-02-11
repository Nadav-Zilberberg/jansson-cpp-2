cmake_minimum_required(VERSION 3.10)
project(jsson_cpp LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Global compile options
add_compile_options(-Wall -Wextra -pedantic)

# Add subdirectory containing the C++ implementation
add_subdirectory(jsson-cpp)

# Add examples directory
add_subdirectory(examples)

# Installation rules for the library and headers
install(TARGETS jsson-cpp
        EXPORT jsson-cpp-targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jsson-cpp/include/*.hpp
        DESTINATION include)

# Export target for finding the package
include(CMakePackageConfigHelpers)
install(EXPORT jsson-cpp-targets
        FILE jsson-cpp-targets.cmake
        NAMESPACE jsson::
        DESTINATION lib/cmake/jsson-cpp)
