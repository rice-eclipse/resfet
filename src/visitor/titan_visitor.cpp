/**
 * @file titan_visitor.cpp
 * @author Andrew Obler (obj2@rice.edu)
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A visitor for handling commands for Titan.
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 */

#include "commands/rpi_pins.hpp"
#include "visitor/titan_visitor.hpp"

void titan_visitor::visitCommand(COMMAND c) {
    logger.info("In titan_visitor process case");

    switch (c) {
	// TODO use the other names
        case SET_WATER: {
            logger.info("Turning vent on using pin %d\n", WATER_VALVE);
            bcm2835_gpio_write(WATER_VALVE, LOW);
            break;
        }
        case UNSET_WATER: {
            logger.info("Turning vent off using pin %d\n", WATER_VALVE);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            break;
        }
        case LEAK_CHECK: {
            logger.info("Entering leak check preset\n");
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            bcm2835_gpio_write(GITVC_VALVE, HIGH);
            break;
        }
        case FILL: {
            logger.info("Entering fill preset\n");
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            bcm2835_gpio_write(GITVC_VALVE, LOW);
            break;
        }
        case FILL_IDLE: {
            logger.info("Entering fill idle preset\n");
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            bcm2835_gpio_write(GITVC_VALVE, HIGH);
            break;
        }
        case DEF: {
            logger.info("Entering default preset\n");
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            bcm2835_gpio_write(WATER_VALVE, LOW);
            bcm2835_gpio_write(GITVC_VALVE, HIGH);
            break;
        }
        default: {
	    logger.error("Command not handled: %d\n", c);
            break;
        }
    }
}
