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

#include "visitor/titan_visitor.hpp"

void titan_visitor::visitProc(COMMAND c) {
    logger.info("In titan_visitor process case");

    switch (c) {
        case set_water: {
            logger.info("Turning vent on on pin " + std::to_string(WATER_VALVE), now);
            bcm2835_gpio_write(WATER_VALVE, LOW);
            break;
        }
        case unset_water: {
            logger.info("Turning vent off on pin " + std::to_string(WATER_VALVE), now);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            break;
        }
        case leak_check: {
            logger.info("Entering Titan Leak Check Preset");
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            bcm2835_gpio_write(GITVC_VALVE, HIGH);
            break;
        }
        case fill: {
            logger.info("Entering Titan Fill Preset");
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            bcm2835_gpio_write(GITVC_VALVE, LOW);
            break;
        }
        case fill_idle: {
            logger.info("Entering Titan Fill Idle Preset");
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            bcm2835_gpio_write(GITVC_VALVE, HIGH);
            break;
        }
        case def: {
            logger.info("Entering Titan Default Preset");
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
