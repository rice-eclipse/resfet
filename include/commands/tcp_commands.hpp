/**
 * @file tcp_commands.hpp
 * @author Andrew Obler (andrewobler@gmail.com)
 * @brief Controller commands sent as bytes over TCP.
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __TCP_COMMANDS_HPP
#define __TCP_COMMANDS_HPP

#include <stdint.h>

enum COMMAND: uint8_t {
    UNSET_VALVE1 = 1,
    SET_VALVE1,
    UNSET_VALVE2,
    SET_VALVE2,
    UNSET_VALVE3,
    SET_VALVE3,
    START_IGNITION,
    STOP_IGNITION,
    SET_WATER,
    UNSET_WATER,
    SET_GITVC,
    UNSET_GITVC,
    LEAK_CHECK,
    FILL,
    FILL_IDLE,
    DEF,
    NUM_COMMANDS
};

char command_names[][20] {
    "UNSET_VALVE1",
    "SET_VALVE",
    "UNSET_VALVE",
    "SET_VALVE",
    "UNSET_VALVE",
    "SET_VALVE",
    "UNSET_IGNITION",
    "SET_IGNITION",
    "SET_WATER",
    "UNSET_WATER",
    "SET_GITVC",
    "UNSET_GITVC",
    "LEAK_CHECK",
    "FILL",
    "FILL_IDLE",
    "DEF"
};

#endif
