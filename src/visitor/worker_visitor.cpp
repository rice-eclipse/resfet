#include <stdint.h>

#include "worker_visitor.hpp"

int engine_type;
int time_between_gitvc;
int gitvc_wait_time;
int preignite_us;
int hotflow_us;
bool ignition_on;
bool use_gitvc;
std::vector<uint8_t> gitvc_times;

void worker_visitor::visitCommand(COMMAND c) {
    logger.info("In worker_visitor process case");

    switch (c) {
        case UNSET_VALVE1: {
	    logger.info("Writing main valve off using pin %d\n", MAIN_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            break;
        }
        case SET_VALVE1: {
	    logger.info("Writing main valve on using pin %d\n", MAIN_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            break;
        }
        case UNSET_VALVE2: {
	    logger.info("Writing water valve off using pin %d\n", WATER_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            break;
        }
        case SET_VALVE2: {
	    logger.info("Writing water valve on using pin %d\n", WATER_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            break;
        }
        case UNSET_VALVE3: {
	    logger.info("Writing GITVC valve off using pin %d\n", GITVC_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            break;
        }
        case SET_VALVE3: {
	    logger.info("Writing GITVC valve on using pin %d\n", GITVC_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            break;
        }
        case START_IGNITION: {
            logger.error("Starting ignition\n");
	    // TODO more info
	    doIgn();
            break;
        }
        case STOP_IGNITION: {
            logger.error("Stopping ignition", now);
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
