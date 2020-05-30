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

#include "logger/logger.hpp"
#include "libtest/libtest.hpp"

#define SUITE_LEN 	32
#define TEST_LEN 	32
#define ASSERT_LEN 	32
#define DASH_LEN 	128

static test_stats global_stats = {0};
static test_stats stats;

static const char *test_suite_name;
static char dashes[DASH_LEN];

static Logger *logger;

int create_dashes(int len) {
	int i;

	if (len > DASH_LEN) {
		return (1);
	}

	// Create a buffer of dashes
	for (i = 0; i < len; i++) {
		dashes[i] = '-';
	}
	dashes[i] = '\0';

	return (0);
}

int testlib_init(const char *suite_name) {
	test_suite_name = suite_name;

	logger = new Logger("Test Logger", test_suite_name, LogLevel::DEBUG);	

	// Length of dashes will be the max of the two lengths
	create_dashes(strlen("Test Suite: ") + strlen(test_suite_name));
	// create_dashes(strlen("Log file: ") + strlen(logger->getFilename()));

	logger->verbatim("%s\n", dashes);
	logger->verbatim("Test Suite: %.*s\n", SUITE_LEN, test_suite_name);
	// logger->verbatim("Log file: %s\n", logger->getFilename());
	logger->verbatim("%s\n", dashes);

	return (0);
}

int testlib_shutdown() {
	logger->verbatim("Global Summary: pass %d, fail %d, total %d\n",
			global_stats.num_pass, global_stats.num_fail,
			global_stats.num_total);

	return (0);
}

int test(const char *test_name, int (*test_func)(void *), void *test_args) {
	int result;
	stats = {0};

	logger->verbatim("- Test: %.*s \n", TEST_LEN, test_name);
	result = test_func(test_args);

	logger->verbatim("Summary: pass %d, fail %d, total %d\n\n",
			stats.num_pass, stats.num_fail, stats.num_total);


	global_stats.num_pass += stats.num_pass;
	global_stats.num_fail += stats.num_fail;
	global_stats.num_total += stats.num_total;

	return (result);
}

int assert_equals(int result, int expected, const char *assert_name) {
	if (result == expected) {
		logger->verbatim("\t Pass: %.*s.\n", ASSERT_LEN, assert_name, result);
		stats.num_pass++;
	} else {
		logger->verbatim("\t Fail: %.*s. Expected %d but result was %d\n",
				ASSERT_LEN, assert_name, expected, result);
		stats.num_fail++;
	}

	stats.num_total++;

	return (result);
}

int assert_not_equals(int result, int expected, const char *assert_name) {
	if (result != expected) {
		logger->verbatim("\t Pass: %.*s.\n", ASSERT_LEN, expected, result);
		stats.num_pass++;
	} else {
		logger->verbatim("\t Fail: %.*s. Expected %d but result was %d\n",
				ASSERT_LEN, assert_name, expected, result);
		stats.num_fail++;
	}

	stats.num_total++;

	return (result);
}

int assert_true(int result, const char *assert_name) {
	if (result) {
		logger->verbatim("\t Pass: %.*s\n", ASSERT_LEN, assert_name);
		stats.num_pass++;
	} else {
		logger->verbatim("\t Fail: %.*s\n", ASSERT_LEN, assert_name);
		stats.num_fail++;
	}

	stats.num_total++;

	return (result);
}

int assert_false(int result, const char *assert_name) {
	return (assert_true(!result, assert_name));
}

int assert_null(void *result, const char *assert_name) {
	return (assert_true(result == NULL, assert_name));
}

int assert_not_null(void *result, const char *assert_name) {
	return (assert_true(result != NULL, assert_name));
}

int assert_string_equals(const char *result, const char *expected, int len, const char *assert_name) {
	int ret; 

	if ((ret = strncmp(result, expected, len)) == 0) {
		logger->verbatim("\t Pass: %.*s.\n", ASSERT_LEN, assert_name, result);
		stats.num_pass++;
	} else {
		logger->verbatim("\t Fail: %.*s. Expected %s but result was %s\n",
				ASSERT_LEN, assert_name, expected, result);
		stats.num_fail++;
	}

	stats.num_total++;

	return (ret);
}

int assert_string_not_equals(const char *result, const char *expected, int len, const char *assert_name) {
	int ret;

	if ((ret = strncmp(result, expected, len)) != 0) {
		logger->verbatim("\t Pass: %.*s.\n", ASSERT_LEN, expected, result);
		stats.num_pass++;
	} else {
		logger->verbatim("\t Fail: %.*s. Expected %d but result was %d\n",
				ASSERT_LEN, assert_name, expected, result);
		stats.num_fail++;
	}

	stats.num_total++;

	return (ret);
}
