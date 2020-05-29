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

// TODO define config max line

ConfigMapping config;
char testBuf[256];
uint32_t testInt;

int test_read_from() {
	// TODO test malformed config files
	return (0);
}

int test_get_string(void *args, TestStats s) {
    assert_equals(config.readFrom("./config_test.ini"), 1, s, "Read from config_test.ini");

    assert_equals(config.getString("", "spaghetti", testBuf, 128), 0,
		    s, "Get spaghetti key");
    assert_equals(std::strcmp(testBuf, "meatballs"), 0, s, "Check spaghetti value");

    assert_equals(config.getString("OtherSection", "foo", testBuf, 128), 0,
         	    s, "Get key foo from Section");
    // assert_equals(std::strcmp(testBuf, ""), 0, s, "Check foo value");

    assert_equals(config.getString("OtherSection", "something", testBuf, 128), 0,
		    s, "Get key something from OtherSection");
    assert_equals(std::strcmp(testBuf, "something"), 0,
		    s, "Check something value");

    return (0);
}

int test_get_int(void *args, TestStats s) {
    assert_equals(config.getInt("Section", "x", &testInt), 0,
		    s, "Get key x from Section");
    assert_equals(testInt, 512, s, "Check x value");

    return (0);
}

int main() {
    testlib_init("Config Parser");

    test("Get String", &test_get_string, NULL);
    test("Get Int", &test_get_int, NULL);

    testlib_shutdown();

//     if (config.getString("OtherSection", "this_config_key_name_is_far_longer_than_sixty_four_characters_and_thus_should_be_rejected_by_the_parser_lets_see_if_thats_what_actually_happens", testBuf, 256) == 0) {
//         // Should reject too-long keys (sscanf fails)
//         std::cerr << "Accepted a key that is ridiculously long" << std::endl;
//         return -1;
//     }
//     // Above line has a fatal error, rest of file should be absent from map
//     if (config.getInt("OtherSection", "pi", &testInt) == 0) {
//         printKeySuccess("pi");
//         return -1;
//     }
//     std::cout << "All tests passed!" << std::endl;

    return (0);
}
