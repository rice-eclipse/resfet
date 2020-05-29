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
int testInt;

static void printKeyFailure(char* key) {
    std::cerr << "TEST FAILURE: unable to read key `" << key << "`"
              << std::endl;
}

static void printKeySuccess(char* key) {
    std::cerr << "TEST FAILURE: key `" << key << "` is present but should not be"
              << std::endl;
}

static void printStrCompFailure(char* key, char* exp, char* act) {
    std::cerr << "TEST FAILURE: expected value `" << exp << "` for key `"
              << key << "`, but found `" << act << "`" << std::endl;
}

static void printIntCompFailure(char* key, int exp, int act) {
    std::cerr << "TEST FAILURE: expected value `" << exp << "` for key `"
              << key << "`, but found `" << act << "`" << std::endl;
}

int test_key(void *args, TestStats s) {
    assert_true(config.getString("", "spaghetti", testBuf, 128) == 0,
		    s, "Get key");

    return (0);
}

int main() {
    config.readFrom("./test.ini");

    testlib_init("Config Parser");

    test("Key test", &test_key, NULL);

    testlib_shutdown();

//     if (std::strcmp(testBuf, "meatballs") != 0) {
//         printStrCompFailure("spaghetti", "meatballs", testBuf);
//         return -1;
//     }
//     if (config.getInt("Section", "x", &testInt) != 0) {
//         printKeyFailure("x");
//         return -1;
//     }
//     if (testInt != 512) {
//         printIntCompFailure("x", 512, testInt);
//         return -1;
//     }
//     if (config.getString("Section", "foo", testBuf, 128) != 0) {
//         printKeyFailure("foo");
//         return -1;
//     }
//     if (std::strcmp(testBuf, "") != 0) {
//         printStrCompFailure("foo", "<empty string>", testBuf);
//         return -1;
//     }
//     if (config.getString("OtherSection", "something", testBuf, 128) != 0) {
//         printKeyFailure("something");
//         return -1;
//     }
//     if (std::strcmp(testBuf, "something") != 0) {
//         printStrCompFailure("something", "something", testBuf);
//         return -1;
//     }
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
