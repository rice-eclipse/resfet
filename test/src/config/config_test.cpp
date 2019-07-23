/**
 * @file config_test.cpp
 * @author Andrew Obler (andrewobler@gmail.com)
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

static void printKeyFailure(char* key) {
    std::cerr << "TEST FAILURE: unable to read key `" << key << "`"
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

int main() {
    ConfigMapping config;
    char testBuf[128];
    int testInt;
    if (config.readFrom("./test.ini") != 0) {
        std::cerr << "TEST FAILURE: unable to read test.ini" << std::endl;
    }
    if (config.getString("", "spaghetti", testBuf, 128) != 0) {
        printKeyFailure("spaghetti");
        return -1;
    }
    if (std::strcmp(testBuf, "meatballs") != 0) {
        printStrCompFailure("spaghetti", "meatballs", testBuf);
        return -1;
    }
    if (config.getInt("Section", "x", &testInt) != 0) {
        printKeyFailure("x");
        return -1;
    }
    if (testInt != 512) {
        printIntCompFailure("x", 512, testInt);
        return -1;
    }
    if (config.getString("Section", "foo", testBuf, 128) != 0) {
        printKeyFailure("foo");
        return -1;
    }
    if (std::strcmp(testBuf, "") != 0) {
        printStrCompFailure("foo", "<empty string>", testBuf);
        return -1;
    }
    if (config.getString("OtherSection", "something", testBuf, 128) != 0) {
        printKeyFailure("something");
        return -1;
    }
    if (std::strcmp(testBuf, "something") != 0) {
        printStrCompFailure("something", "something", testBuf);
        return -1;
    }
    if (config.getInt("OtherSection", "pi", &testInt) != 0) {
        printKeyFailure("pi");
        return -1;
    }
    if (testInt != 314159) {
        printIntCompFailure("pi", 314159, testInt);
        return -1;
    }
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
