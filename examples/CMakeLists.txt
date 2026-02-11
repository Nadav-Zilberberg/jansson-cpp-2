# CMakeLists.txt for the example executable

cmake_minimum_required(VERSION 3.14)

# Define the example executable
add_executable(example example.cpp)

# Link the example with the jsson-cpp library
target_link_libraries(example PRIVATE jsson-cpp)

# Set the output directory to match the expected structure
set_target_properties(example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)