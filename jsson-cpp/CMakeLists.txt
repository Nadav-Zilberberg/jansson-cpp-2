# CMakeLists.txt for the jsson-cpp library
cmake_minimum_required(VERSION 3.10)
project(jsson_cpp LANGUAGES CXX)

# Create a static library by default, can be changed to shared if needed
add_library(jsson_cpp STATIC)

# Include directories
target_include_directories(jsson_cpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set C++ standard
target_compile_features(jsson_cpp PUBLIC cxx_std_17)

# Compile options
target_compile_options(jsson_cpp PUBLIC -Wall -Wextra -pedantic)

# Add source files
file(GLOB JANSSON_CPP_SRC src/*.cpp)
target_sources(jsson_cpp PRIVATE ${JANSSON_CPP_SRC})

# Install target
install(TARGETS jsson_cpp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(FILES
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp>
    DESTINATION include
)