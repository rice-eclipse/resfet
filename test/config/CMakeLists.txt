# Create the config test executables
# TODO if there are multiple testing executables, add them using add_dependenies to the target ${TEST_NAME}
set(TEST_PREFIX config)
set(TEST_NAME ${TEST_PREFIX}_test)

add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
target_link_libraries(${TEST_NAME} libtest config logger time)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
set_tests_properties(${TEST_NAME} PROPERTIES LABELS CONFIG_PARSER)

# Copy the resources (.ini) files to the output directory
# TODO if there are multiple .ini test inputs, add them here
add_custom_command(TARGET ${TEST_NAME} POST_BUILD
		   COMMAND cp config_test.ini "${CMAKE_CURRENT_BINARY_DIR}/"
		   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(${TEST_PREFIX}_coverage
                  COMMAND ctest -R ${TEST_PREFIX} || :
                  WORKING_DIRECTORY ${CMAKE_BUILD_DIR})

add_custom_command(TARGET ${TEST_PREFIX}_coverage POST_BUILD
                   COMMAND echo gcov -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TEST_NAME}.dir/ ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.cpp
                   COMMAND gcov -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TEST_NAME}.dir/" "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.cpp" | grep -A 1 ${TEST_NAME} > summary
                   COMMAND echo gcov -o ${CMAKE_BINARY_DIR}/src/${TEST_PREFIX}/CMakeFiles/${TEST_PREFIX}.dir ${CMAKE_SOURCE_DIR}/src/${TEST_PREFIX}/${TEST_PREFIX}.cpp
                   COMMAND gcov -o ${CMAKE_BINARY_DIR}/src/${TEST_PREFIX}/CMakeFiles/${TEST_PREFIX}.dir ${CMAKE_SOURCE_DIR}/src/${TEST_PREFIX}/${TEST_PREFIX}.cpp | grep -A 1 ${TEST_PREFIX} >> summary
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/coverage)
