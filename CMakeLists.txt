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
add_subdirectory(src/adc)
add_subdirectory(src/circular_buffer)
add_subdirectory(src/thread)
add_subdirectory(src/visitor)
add_subdirectory(src/init)

# Build the main executables
add_executable(resfet src/main.cpp)

# Build the mock executables
add_executable(mock_resfet src/main.cpp)
target_compile_definitions(mock_resfet PUBLIC MOCK=1)

# Build the test executables
add_executable(adc_test test/adc/adc_test.cpp)
add_executable(udp_echo_server test/udp/udp_echo_server.cpp)

# Link the libraries
target_link_libraries(resfet networking logger time config adc circular_buffer thread visitor init)
target_link_libraries(mock_resfet networking logger time config adc circular_buffer mock_thread visitor init)
target_link_libraries(adc_test adc)
