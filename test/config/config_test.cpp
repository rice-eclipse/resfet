/**
 * @file config_test.cpp
 * @author Andrew Obler (andrewobler@gmail.com)
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Basic functionality test for config.hpp.
 * @version 0.1
 * @date 2019-07-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <cstring>
#include <iostream>

#include "config/config.hpp"
#include "libtest/libtest.hpp"

ConfigMapping config;
char testBuf[256];
uint32_t testInt;

int test_bad_config(void *args) {
    assert_equals(config.readFrom("./bad_config.ini"), 1, "Read from bad_config.ini");

    assert_equals(config.getString("", "a", testBuf, 128), 1,
		    "Can't read from bad_config.ini");
    return (0);
}

int test_get_string(void *args) {
    assert_equals(config.readFrom("./config_test.ini"), 0, "Read from config_test.ini");

    assert_equals(config.getString("", "spaghetti", testBuf, 128), 0,
		    "Get spaghetti key");
    assert_string_equals(testBuf, "meatballs", MAX_CONFIG_LENGTH, "Check spaghetti value");

    assert_equals(config.getString("Section", "foo", testBuf, 128), 0,
         	    "Get key foo from Section");
    assert_equals(std::strcmp(testBuf, "bar"), 0, "Check foo value");

    assert_equals(config.getString("OtherSection", "something", testBuf, 128), 0,
		    "Get key something from OtherSection");
    assert_string_equals(testBuf, "something", MAX_CONFIG_LENGTH,
		    "Check something value");

    return (0);
}

int test_get_int(void *args) {
    assert_equals(config.getInt("Section", "x", &testInt), 0,
		    "Get key x from Section");
    assert_equals(testInt, 512, "Check x value");

	assert_equals(config.getInt("OtherSection", "pi", &testInt), 0,
					"Get key pi from OtherSection");
	assert_equals(testInt, 314159, "Check pi value");

    return (0);
}

int main() {
    testlib_init("Config Parser");

    test ("readFrom", &test_bad_config, NULL);
    test("Get String", &test_get_string, NULL);
    test("Get Int", &test_get_int, NULL);

    return (testlib_shutdown());
}
