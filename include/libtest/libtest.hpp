/**
 * @file libtest.hpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Definition of classes and methods used in the testing library
 * @version 0.1
 * @date 2020-05-29
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef __TESTLIB_H
#define __TESTLIB_H

typedef struct {
	int num_pass;
	int num_fail;
	int num_total;
} test_stats, *TestStats;

/**
 * @brief Initializes the testing library for a particular test suite/file.
 *
 * @test_suite_name The name of the test suite used in printouts.
 */
int testlib_init(const char *suite_name);

/**
 * @brief Cleans up and exits.
 */
int testlib_shutdown();

/**
 * @brief Runs a particular test.
 *
 * @test_func The function containing the tests. 
 */
int test(const char *test_name, int (*test_func)(void *), void *test_args);

/**
 * @brief Asserts the result matches the expected result.
 *
 * @result The result to be checked.
 * @expected The expected result.
 * @TestStats The statistics  for this test suite/file.
 * @assert_name The name of this assertion.
 */
int assert_equals(int result, int expected, const char *assert_name);

/**
 * @brief Asserts the result is true.
 *
 * @result The result to be checked.
 * @TestStats The statistics  for this test suite/file.
 * @assert_name The name of this assertion.
 */
int assert_true(int result, const char *assert_name);

/**
 * @brief Asserts the result is false.
 *
 * @result The result to be checked.
 * @TestStats The statistics  for this test suite/file.
 * @assert_name The name of this assertion.
 */
int assert_false(int result, const char *assert_name);

/**
 * @brief Asserts the result is null.
 *
 * @result The result to be checked.
 * @TestStats The statistics  for this test suite/file.
 * @assert_name The name of this assertion.
 */
int assert_null(void *result, const char *assert_name);

/**
 * @brief Asserts the result is not null.
 *
 * @result The result to be checked.
 * @TestStats The statistics  for this test suite/file.
 * @assert_name The name of this assertion.
 */
int assert_not_null(void *result, const char *assert_name);

/**
 * @brief Asserts the result string matches the expected string.
 *
 * @result The result string to be checked.
 * @expected The expected string.
 * @len The maximum length of the comparison.
 * @TestStats The statistics  for this test suite/file.
 * @assert_name The name of this assertion.
 */
int assert_string_equals(const char *result, const char *expected, int len, const char *assert_name); 
/**
 * @brief Asserts the result string matches the expected string.
 *
 * @result The result string to be checked.
 * @expected The expected string.
 * @len The maximum length of the comparison.
 * @TestStats The statistics  for this test suite/file.
 * @assert_name The name of this assertion.
 */
int assert_string_not_equals(const char *result, const char *expected, int len, const char *assert_name); 

#endif
