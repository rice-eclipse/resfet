# Create the thread library
add_library(thread STATIC thread.cpp)

add_library(mock_thread STATIC thread.cpp)
target_compile_definitions(mock_thread PUBLIC MOCK=1)

target_link_libraries(thread circular_buffer pthread)
target_link_libraries(mock_thread circular_buffer pthread)
