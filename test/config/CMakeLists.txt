# Create the config test executables
add_executable(config_test config_test.cpp)
target_link_libraries(config_test libtest config logger time)

# Add a post-build step to gather the testing executables
# and resources (.ini) in the same directory
add_custom_command(TARGET config_test POST_BUILD
		   COMMAND cp config_test.ini "${CMAKE_CURRENT_BINARY_DIR}/"
		   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
