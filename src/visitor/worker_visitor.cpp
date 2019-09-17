/**
 * @file worker_visitor.cpp
 * @author Andrew Obler (obj2@rice.edu)
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A visitor superclass for handling commands.
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 */

#include <stdint.h>

#include "commands/rpi_pins.hpp"
#include "visitor/worker_visitor.hpp"

int engine_type;
int time_between_gitvc;
int gitvc_wait_time;
int preignite_us;
int hotflow_us;
bool ignition_on;
bool use_gitvc;
std::vector<uint8_t> gitvc_times;

worker_visitor::worker_visitor()
	: burn_on(0)
	, logger("Visitor Logger", "Visitor_Logger", LogLevel::DEBUG)
{
	// logger = Logger("Visitor Logger", "Visitor_Logger", LogLevel::DEBUG);
};

void worker_visitor::visitCommand(COMMAND c) {
    logger.info("In worker_visitor process case");

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
            logger.error("Starting ignition\n");
	    // TODO more info
	    doIgn();
            break;
        }
        case STOP_IGNITION: {
            logger.error("Stopping ignition\n");
	    // TODO more info
	    // TODO stop ignition thread
            bcm2835_gpio_write(IGN_START, HIGH);
            break;
        }
        default: {
	    logger.error("Command not handled: %d\n", c);
            break;
        }
    }
}

void worker_visitor::doIgn() {
}
