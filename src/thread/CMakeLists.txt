# Create the thread library
add_library(thread STATIC thread.cpp)

target_link_libraries(thread circular_buffer pthread)
