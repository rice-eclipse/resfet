/**
 * @file luna_visitor.cpp
 * @author Andrew Obler (obj2@rice.edu)
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A visitor for handling commands for Luna.
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 */

#include "visitor/luna_visitor.hpp"

luna_visitor::luna_visitor()
	: gitvc_on(0)
	, gitvc_count(0)
	{
	};

void luna_visitor::visitCommand(COMMAND c) {
    logger.info("In luna_visitor process case");

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
        default: {
	    logger.error("Command not handled: %d\n", c);
            break;
        }
    }
}

void luna_visitor::doIgm() {

}
