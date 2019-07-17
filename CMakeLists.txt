cmake_minimum_required (VERSION 2.8.7)
add_compile_options(-std=c++11)
project(resfet)

# Include the header files
include_directories(include)

# Add the directories for libraries
add_subdirectory(src/networking)
add_subdirectory(src/logger)
add_subdirectory(src/time)
add_subdirectory(src/config)

# Build the main executable
add_executable(resfet src/main.cpp)

# Link the libraries
target_link_libraries(resfet networking logger time config)
