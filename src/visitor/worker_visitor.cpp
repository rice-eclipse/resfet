/**
 * @file WorkerVisitor.cpp
 * @author Andrew Obler (obj2@rice.edu)
 * @author Tommy Yuan (ty19@rice.edu) * @brief A visitor superclass for handling commands.
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 */

#include <mutex>
#include <stdint.h>
#include <time.h>
#include <thread>

#include "config/config.hpp"
#include "commands/rpi_pins.hpp"
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
    "DEF"
};

WorkerVisitor::WorkerVisitor()
    : config(ConfigMapping())
    , burn_on(false)
    , logger("Visitor", "Visitor_Logger", LogLevel::DEBUG)
{

}

WorkerVisitor::WorkerVisitor(ConfigMapping& config)
    : config(config)
    , burn_on(false)
    , logger("Visitor Logger", "Visitor_Logger", LogLevel::DEBUG)
{
	config.getInt("Worker", "preignite_ms", &preignite_ms);
	config.getInt("Worker", "hotflow_ms", &hotflow_ms);
	logger.debug("preignite_ms: %d\n", preignite_ms);
	logger.debug("hotflow_ms: %d\n", hotflow_ms);
}

void WorkerVisitor::ignThreadFunc(timestamp_t time, bool* pBurnOn, std::mutex* pMtx) {
    // Keep track of ignition time
    set_start_time();
    timestamp_t initTime = get_elapsed_time_ms();
    timestamp_t timeElapsed = 0;

    // Calculate time between checks of pBurnOn
    timespec tsCheck = { IGN_CHECK_MS / 1000, (IGN_CHECK_MS % 1000) * 1000000 };

    // Loop while there is time left for ignition
    while (timeElapsed < time) {
        // Check pBurnOn
        pMtx->lock();
        if (!*pBurnOn) {
            // Main thread has indicated an ignition stop
            bcm2835_gpio_write(IGN_START, HIGH);
            pMtx->unlock();
            break;
        }
        pMtx->unlock();
        // Sleep so we're not checking every cycle
        nanosleep(&tsCheck, NULL);
        timeElapsed = get_elapsed_time_ms() - initTime;
    }

    // Burn time has elapsed, shut it off and indicate
    bcm2835_gpio_write(IGN_START, HIGH);
    pMtx->lock();
    *pBurnOn = false;
    pMtx->unlock();
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
            // TODO more info
            doIgn();
            break;
        }
        case STOP_IGNITION: {
            logger.info("Stopping ignition\n");
            burnMtx.lock();
            burn_on = false;
            burnMtx.unlock();

            // TODO: shouldn't need to do this, the thread should detect it
            // bcm2835_gpio_write(IGN_START, HIGH);

            // TODO close valve
            break;
        }
        default: {
	        logger.error("Command not handled: %d\n", c);
            break;
        }
    }
}

void WorkerVisitor::doIgn() {
    burnMtx.lock();
    burn_on = true;
    burnMtx.unlock();

    // Create a monitoring thread to cut off ignition after time has elapsed
    std::thread t(ignThreadFunc, hotflow_ms, &burn_on, &burnMtx);
    t.detach();
}
