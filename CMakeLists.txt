cmake_minimum_required (VERSION 3.12)
add_compile_options(-std=c++11 -fprofile-arcs -ftest-coverage)
# add_compile_options(-std=c++11 -Werror -Wall -Wextra)
set(CMAKE_EXE_LINKER_FLAGS "-lgcov -fprofile-arcs -ftest-coverage")
project(resfet)

# Include the header files
include_directories(include)

# Add the directories for source code
add_subdirectory(src/networking)
add_subdirectory(src/logger)
add_subdirectory(src/time)
add_subdirectory(src/config)
add_subdirectory(src/adc)
add_subdirectory(src/circular_buffer)
add_subdirectory(src/thread)
add_subdirectory(src/visitor)
add_subdirectory(src/init)
add_subdirectory(src/libtest)

# Enable CTest testing
include(CTest)

# Add the directories for testing code
add_subdirectory(test/config)

# Create custom target for coverage testing
file(MAKE_DIRECTORY coverage)
add_custom_target(coverage)
add_dependencies(coverage config_coverage) # TODO add other testing targets here

# Build the main executables
add_executable(resfet src/main.cpp)

add_executable(mock_resfet src/main.cpp)
target_compile_definitions(mock_resfet PUBLIC MOCK=1)

# Link the libraries
target_link_libraries(resfet networking logger time config adc circular_buffer thread visitor init gcov)
target_link_libraries(mock_resfet networking logger time config adc circular_buffer mock_thread visitor init gcov)