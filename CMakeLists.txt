cmake_minimum_required(VERSION 3.14)
project(jasson-cpp LANGUAGES C CXX)

# Set C++ standard to 17 and enforce it
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings and treat them as errors
add_compile_options(-Wall -Wextra -Werror)
# Additional debugging flags
add_compile_options(-fno-omit-frame-pointer -g)
# Enable sanitizers (they are no-ops on platforms that don't support them)
add_compile_options(-fsanitize=address,undefined)

# Find Boost (optional for future extensions)
find_package(Boost OPTIONAL)

# Add the library target
add_library(jasson
    src/jansson.cpp
    src/hashtable.cpp
    src/strbuffer.cpp
    src/lookup3.cpp
    src/utf.cpp
    src/memory.cpp
    src/version.cpp
    src/dtoa.cpp
    src/dump.cpp
    src/load.cpp
    src/pack_unpack.cpp
    src/value.cpp
)

# Public include directories
target_include_directories(jasson PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link against Boost if found (future-proofing)
target_link_libraries(jasson PUBLIC Boost::boost)

# Installation rules
install(TARGETS jasson
        EXPORT jassonTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(EXPORT jassonTargets
        FILE jassonTargets.cmake
        NAMESPACE jasson::
        DESTINATION lib/cmake/jasson)

install(DIRECTORY include/jasson/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp"
        FILES_MATCHING PATTERN "*.h"
)

