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
#include <string.h>

#include "libtest/libtest.hpp"

// TODO use the logger.
// specify log level in command line
// pretty printing

static test_stats global_stats = {0};
static test_stats func_stats;

static const char *test_suite_name;

int testlib_init(const char *suite_name) {
	test_suite_name = suite_name;

	printf("BEGIN SUITE %.32s\n", test_suite_name);
	printf("Log file is %.32s\n", "TODO");

	return (0);
}

int testlib_shutdown() {
	printf("END SUITE %.32s. pass %d, fail %d, total %d\n",
			test_suite_name,global_stats.num_pass,
			global_stats.num_fail, global_stats.num_total);

	return (0);
}

int test(const char *test_name, int (*test_func)(void *, TestStats), void *test_args) {
	int result;
	test_stats stats = {0};

	printf("BEGIN TEST %.32s: %.32s\n", test_suite_name, test_name);
	result = test_func(test_args, &stats);

	printf("END TEST %.32s: %.32s. pass %d, fail %d, total %d\n",
			test_suite_name, test_name,
			stats.num_pass, stats.num_fail, stats.num_total);


	global_stats.num_pass += stats.num_pass;
	global_stats.num_fail += stats.num_fail;
	global_stats.num_total += stats.num_total;

	return (result);
}

int assert_equals(int result, int expected, TestStats s, const char *assert_name) {
	if (result == expected) {
		printf("\t Pass: %.32s.\n", assert_name, result);
		s->num_pass++;
	} else {
		printf("\t Fail: %.32s. Expected %d but result was %d\n",
				assert_name, expected, result);
		s->num_fail++;
	}

	s->num_total++;

	return (result);
}

int assert_not_equals(int result, int expected, TestStats s, const char *assert_name) {
	if (result != expected) {
		printf("\t Pass: %.32s.\n", expected, result);
		s->num_pass++;
	} else {
		printf("\t Fail: %.32s. Expected %d but result was %d\n",
				assert_name, expected, result);
		s->num_fail++;
	}

	s->num_total++;

	return (result);
}

int assert_true(int result, TestStats s, const char *assert_name) {
	if (result) {
		printf("\t Pass: %.32s\n", assert_name);
		s->num_pass++;
	} else {
		printf("\t Fail: %.32s\n", assert_name);
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

int assert_string_equals(const char *result, const char *expected, int len, TestStats s, const char *assert_name) {
	int ret; 

	if ((ret = strncmp(result, expected, len)) == 0) {
		printf("\t Pass: %.32s.\n", assert_name, result);
		s->num_pass++;
	} else {
		printf("\t Fail: %.32s. Expected %s but result was %s\n",
				assert_name, expected, result);
		s->num_fail++;
	}

	s->num_total++;

	return (ret);
}

int assert_string_not_equals(const char *result, const char *expected, int len, TestStats s, const char *assert_name) {
	int ret;

	if ((ret = strncmp(result, expected, len)) != 0) {
		printf("\t Pass: %.32s.\n", expected, result);
		s->num_pass++;
	} else {
		printf("\t Fail: %.32s. Expected %d but result was %d\n",
				assert_name, expected, result);
		s->num_fail++;
	}

	s->num_total++;

	return (ret);
}
