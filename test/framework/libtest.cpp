/**
 * @file libtest.cpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Implementation of classes and methods used in the testing library
 * @version 0.1
 * @date 2020-05-29
 * 
 * @copyright Copyright (c) 2020
 */

#include <stdio.h>
#include <assert.h>

#include "libtest.hpp"

// TODO use the logger. Make logger a library?

static test_stats global_stats;
static test_stats func_stats;

static const char *test_suite_name;

int testlib_init(const char *suite_name) {
	test_suite_name = suite_name;

	printf("BEGIN SUITE %32s\n", test_suite_name);
	printf("Log file is %32s\n", "TODO");

	return (0);
}

int testlib_shutdown() {
	return (0);
}

int test(const char *test_name, int (*test_func)(void *, TestStats), void *test_args) {
	int result;
	test_stats stats = {0};

	printf("BEGIN TEST %32s: %32s\n", test_suite_name, test_name);
	result = test_func(test_args, &stats);

	printf("END TEST %32s: %32s. pass %d, fail %d, total %d\n",\
			test_suite_name, test_name,\
			stats.num_pass, stats.num_fail, stats.num_total);
	return (result);
}

int assert_true(int result, TestStats s, const char *assert_name) {
	if (result) {
		printf("\t %32s pass\n", assert_name);
		s->num_pass++;
	} else {
		printf("\t %32s fail\n", assert_name);
		s->num_fail++;
	}

	s->num_total++;

	return (result);
}

int assert_false(int result, TestStats s, const char *assert_name) {
	return (assert_true(!result, s, assert_name));
}

int assert_null(void *result, TestStats s, const char *assert_name) {
	return (assert_true(result == NULL, s, assert_name));
}

int assert_not_null(void *result, TestStats s, const char *assert_name) {
	return (assert_true(result != NULL, s, assert_name));
}
