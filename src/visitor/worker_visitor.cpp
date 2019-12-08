/**
 * @file WorkerVisitor.cpp
 * @author Andrew Obler (obj2@rice.edu)
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A visitor superclass for handling commands.
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 */

#include <atomic>
#include <chrono>
#include <cstdio>
#include <stdint.h>
#include <time.h>
#include <thread>

#include "config/config.hpp"
#include "commands/rpi_pins.hpp"
#include "logger/logger.hpp"
#include "time/time.hpp"
#include "visitor/worker_visitor.hpp"

const char *command_names[NUM_COMMANDS] = {
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
    "TAPE_ON",
    "TAPE_OFF",
    "DEF"
};

WorkerVisitor::WorkerVisitor()
    : config(ConfigMapping())
    , logger("Visitor", "Visitor_Logger", LogLevel::DEBUG)
{

}

WorkerVisitor::WorkerVisitor(ConfigMapping& config)
    : config(config)
    , logger("Visitor Logger", "Visitor_Logger", LogLevel::DEBUG)
{
	config.getInt("Worker", "preignite_ms", &preignite_ms);
	config.getInt("Worker", "hotflow_ms", &hotflow_ms);
	logger.debug("preignite_ms: %d\n", preignite_ms);
	logger.debug("hotflow_ms: %d\n", hotflow_ms);
}

Logger WorkerVisitor::ignThreadLogger = Logger("Ign Thread", "IgnThreadLog", LogLevel::DEBUG);

void WorkerVisitor::ignThreadFunc(timestamp_t time, timestamp_t preigniteTime) {
    ignThreadLogger.info("Ignition monitor thread started\n");

    // Whether the main valve is open; should open after preigniteTime elapses
    bool mainOpen = false;

    // Keep track of ignition time
    set_start_time();
    timestamp_t initTime = get_elapsed_time_ms();
    timestamp_t timeElapsed = 0;

    // Loop while there is time left for ignition
    while (timeElapsed < time) {
        // Check if the main valve should be opened
        if (!mainOpen && timeElapsed > preigniteTime) {
            bcm2835_gpio_write(VALVE1, HIGH);
            mainOpen = true;
            ignThreadLogger.info("Preignite time elapsed, opening main valve\n");
        }


        // Check ignitionOn to see if we should stop the burn
        if (!ignitionOn.load()) {
            // Main thread has indicated an ignition stop, so close everything
            bcm2835_gpio_write(VALVE1, LOW);
            bcm2835_gpio_write(IGN_START, LOW);
            ignThreadLogger.info("Emergency stop indicated, closing valve ending burn\n");
            return;
        }

        // Sleep for a bit so we're not checking every cycle
        std::this_thread::sleep_for(std::chrono::milliseconds(IGN_CHECK_MS));
        timeElapsed = get_elapsed_time_ms() - initTime;
    }

    // Burn time has elapsed, shut it off and indicate
    bcm2835_gpio_write(VALVE1, LOW);
    bcm2835_gpio_write(IGN_START, LOW);
    ignitionOn.store(false);
    ignThreadLogger.info("Burn time elapsed, burn has ended\n");
}

void WorkerVisitor::visitCommand(COMMAND c) {
    switch (c) {
        case UNSET_VALVE1: {
	    logger.info("Writing valve 1 off using pin %d\n", VALVE1);
            bcm2835_gpio_write(VALVE1, LOW);
            break;
        }
        case SET_VALVE1: {
	    logger.info("Writing valve 1 on using pin %d\n", VALVE1);
            bcm2835_gpio_write(VALVE1, HIGH);
            break;
        }
        case UNSET_VALVE2: {
	    logger.info("Writing valve 2 off using pin %d\n", VALVE2);
            bcm2835_gpio_write(VALVE2, LOW);
            break;
        }
        case SET_VALVE2: {
	    logger.info("Writing valve 2 on using pin %d\n", VALVE2);
            bcm2835_gpio_write(VALVE2, HIGH);
            break;
        }
        case UNSET_VALVE3: {
	    logger.info("Writing valve 3 off using pin %d\n", VALVE3);
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            break;
        }
        case SET_VALVE3: {
	    logger.info("Writing valve 3 on using pin %d\n", VALVE3);
            bcm2835_gpio_write(VALVE3, HIGH);
            break;
        }
        case START_IGNITION: {
            logger.info("Starting ignition\n");
            doIgn();
            break;
        }
        case STOP_IGNITION: {
            logger.info("Stopping ignition\n");
            ignitionOn.store(false);

            // NOTE: in theory, we don't need to do this, because it happens in the thread,
            // but better safe than sorry
            bcm2835_gpio_write(VALVE1, LOW);
            bcm2835_gpio_write(IGN_START, LOW);

            break;
        }
        default: {
	        logger.error("Command not handled: %d\n", c);
            break;
        }
    }
}

void WorkerVisitor::doIgn() {
    ignitionOn.store(true);
    bcm2835_gpio_write(IGN_START, HIGH);

    // Create a monitoring thread to cut off ignition after time has elapsed
    std::thread t(ignThreadFunc, hotflow_ms, preignite_ms);
    t.detach();
}
