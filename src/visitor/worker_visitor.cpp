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

// Forward declaration for use in constructor
static void ignThreadFunc(timestamp_t, timestamp_t, timestamp_t, bool, int32_t);

const char *command_names[NUM_COMMANDS] = {
    "UNSET_DRIVER1",
    "SET_DRIVER1",
    "UNSET_DRIVER2",
    "SET_DRIVER2",
    "UNSET_DRIVER3",
    "SET_DRIVER3",
    "UNSET_DRIVER4",
    "SET_DRIVER4",
    "UNSET_DRIVER5",
    "SET_DRIVER5",
	"UNSET_DRIVER6",
    "SET_DRIVER6",
	"STOP_IGNITION", // Analogous to UNSET_DRIVER6
    "START_IGNITION", // Analogous to SET_DRIVER6
	"TITAN_LEAK_CHECK",
    "TITAN_FILL",
    "TITAN_FILL_IDLE",
    "TITAN_TAPE_ON",
    "TITAN_TAPE_OFF",
    "TITAN_DEF",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED"   
};

WorkerVisitor::WorkerVisitor()
    : config(ConfigMapping())
    , logger("Visitor Logger", "Visitor_Logger", LogLevel::DEBUG)
{
    // TODO have this at all?
}

WorkerVisitor::WorkerVisitor(ConfigMapping& config)
    : config(config)
    , logger("Visitor Logger", "Visitor_Logger", LogLevel::DEBUG)
{
	config.getInt("Worker", "preignite_ms", &preignite_ms);
	config.getInt("Worker", "hotflow_ms", &hotflow_ms);
	config.getInt("Pressure", "pressureshutoff_ms", &pressureshutoff_ms);
	config.getBool("Pressure", "shutoff_enabled", &enableShutoff);

	logger.debug("preignite_ms: %d\n", preignite_ms);
	logger.debug("hotflow_ms: %d\n", hotflow_ms);
    logger.debug("pressureshutoff_ms: %d\n", pressureshutoff_ms);
    logger.debug("shutoff_enabled: %d\n", enableShutoff);

    ignitionOn.store(false);
    
    bool engine_type;
    config.getBool("Main", "engine_type", &engine_type);

    // Create a persistent ignition monitor thread
    if (engine_type) {
        // Titan Engine
        logger.info("Creating ignition monitor thread for TITAN engine.\n");
        std::thread t(ignThreadFunc, hotflow_ms, preignite_ms, pressureshutoff_ms, enableShutoff, -1);
        t.detach();
    } else {
        // Luna Engine
        logger.info("Creating ignition monitor thread for LUNA engine.\n");
        std::thread t(ignThreadFunc, hotflow_ms, preignite_ms, pressureshutoff_ms, enableShutoff, MAIN_VALVE);
        t.detach();
    }
}

static Logger ignThreadLogger = Logger("Ign Thread", "IgnThreadLog", LogLevel::DEBUG);

static void ignThreadFunc(timestamp_t time, timestamp_t preigniteTime, timestamp_t pressureShutoffDelay, bool enableShutoff, int32_t valve) {
    ignThreadLogger.info("Ignition monitor thread started\n");
    set_start_time();

    bool mainOpen; // will flip true once preigniteTime elapses
    timestamp_t initTime, timeElapsed;

    // Main thread loop, runs forever
    while (true) {
        ignThreadLogger.info("Monitor thread waiting for burn...\n");

        // Wait until the main worker thread indicates the start of a burn
        while (!ignitionOn.load()) {
            // Sleep for a bit so we're not checking every cycle
            std::this_thread::sleep_for(std::chrono::milliseconds(IGN_CHECK_MS));
        }

        ignThreadLogger.info("Received ignition signal.\n");

        // Keep track of ignition time
        initTime = get_elapsed_time_ms();
        timeElapsed = 0;

        // Write HIGH to the ignition pin
        bcm2835_gpio_write(IGN_START, HIGH);
        ignThreadLogger.info("Igniting the ignitors. Writing high on %d\n", IGN_START);

        // Loop while there is time left for ignition
        while (timeElapsed < time) {
            // Check if the main valve should be opened
            if (!mainOpen && timeElapsed > preigniteTime) {

                if (valve >= 0) {
                    bcm2835_gpio_write(valve, HIGH);
                    mainOpen = true;
                    ignThreadLogger.info("Preignite time elapsed. Opening valve %d\n", valve);
                }
            }
            
            // Check if pressure shutoff has been indicated from the sensor thread
            if (timeElapsed > pressureShutoffDelay && enableShutoff && pressureShutoff.load()) {
                ignThreadLogger.info("Pressure shutoff indicated.\n");
                break;
            }

            // Check ignitionOn to see if we should stop the burn
            if (!ignitionOn.load()) {
                // Main thread has indicated an ignition stop, so close everything
                ignThreadLogger.info("Emergency stop indicated.\n");
                break;
            }

            // Sleep for a bit so we're not checking every cycle
            std::this_thread::sleep_for(std::chrono::milliseconds(IGN_CHECK_MS));
            timeElapsed = get_elapsed_time_ms() - initTime;
        }

        // Burn time has elapsed, shut it off and indicate
        if (valve >= 0) {
            bcm2835_gpio_write(valve, LOW);
            ignThreadLogger.info("Closing valve %d\n", valve);
        }

        bcm2835_gpio_write(IGN_START, LOW);
        ignThreadLogger.info("Finished igniting the ignitors. Writing low on %d\n", IGN_START);

        ignitionOn.store(false);
        mainOpen = false;

        ignThreadLogger.info("Burn has ended.\n");
    }

    // Should never happen
    ignThreadLogger.error("Ignition thread broke out of main loop!\n");
}

void WorkerVisitor::visitCommand(COMMAND c) {
    switch (c) {
        case UNSET_DRIVER1: {
	    logger.info("Writing driver 1 off using pin %d\n", DRIVER1);
            bcm2835_gpio_write(DRIVER1, LOW);
            break;
        }
        case SET_DRIVER1: {
	    logger.info("Writing driver 1 on using pin %d\n", DRIVER1);
            bcm2835_gpio_write(DRIVER1, HIGH);
            break;
        }
        case UNSET_DRIVER2: {
	    logger.info("Writing driver 2 off using pin %d\n", DRIVER2);
            bcm2835_gpio_write(DRIVER2, LOW);
            break;
        }
        case SET_DRIVER2: {
	    logger.info("Writing driver 2 on using pin %d\n", DRIVER2);
            bcm2835_gpio_write(DRIVER2, HIGH);
            break;
        }
        case UNSET_DRIVER3: {
	    logger.info("Writing driver 3 off using pin %d\n", DRIVER3);
            bcm2835_gpio_write(DRIVER3, LOW);
            break;
        }
        case SET_DRIVER3: {
	    logger.info("Writing driver 3 on using pin %d\n", DRIVER3);
            bcm2835_gpio_write(DRIVER3, HIGH);
            break;
        }
        case UNSET_DRIVER4: {
	    logger.info("Writing driver 4 off using pin %d\n", DRIVER4);
            bcm2835_gpio_write(DRIVER4, LOW);
            break;
        }
        case SET_DRIVER4: {
	    logger.info("Writing driver 4 on using pin %d\n", DRIVER4);
            bcm2835_gpio_write(DRIVER4, HIGH);
            break;
        }
        case UNSET_DRIVER5: {
	    logger.info("Writing driver 5 off using pin %d\n", DRIVER5);
            bcm2835_gpio_write(DRIVER5, LOW);
            break;
        }
        case SET_DRIVER5: {
	    logger.info("Writing driver 5 on using pin %d\n", DRIVER5);
            bcm2835_gpio_write(DRIVER5, HIGH);
            break;
        }
        case UNSET_DRIVER6: {
	    logger.info("Writing driver 6 off using pin %d\n", DRIVER6);
            bcm2835_gpio_write(DRIVER6, LOW);
            break;
        }
        case SET_DRIVER6: {
	    logger.info("Writing driver 6 on using pin %d\n", DRIVER6);
            bcm2835_gpio_write(DRIVER6, HIGH);
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
            bcm2835_gpio_write(MAIN_VALVE, LOW);
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
    pressureShutoff.store(false);
}
