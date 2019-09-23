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

#include "config/config.hpp"
#include "commands/rpi_pins.hpp"
#include "visitor/titan_visitor.hpp"

TitanVisitor::TitanVisitor()
    : WorkerVisitor()
{

}

TitanVisitor::TitanVisitor(ConfigMapping& config)
    : WorkerVisitor(config)
{

}

void TitanVisitor::visitCommand(COMMAND c) {
    logger.info("In titan_visitor process case");

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
        case SET_VALVE2: {
            logger.info("Turning vent on using pin %d\n", VENT_VALVE);
            bcm2835_gpio_write(VENT_VALVE, LOW);
            break;
        }
        case UNSET_VALVE2: {
            logger.info("Turning vent off using pin %d\n", VENT_VALVE);
            bcm2835_gpio_write(VENT_VALVE, HIGH);
            break;
        }
        case UNSET_VALVE3: {
	    logger.info("Writing tank valve off using pin %d\n", TANK_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            break;
        }
        case SET_VALVE3: {
	    logger.info("Writing tank valve on using pin %d\n", TANK_VALVE);
            bcm2835_gpio_write(TANK_VALVE, HIGH);
            break;
        }
        case LEAK_CHECK: {
            logger.info("Entering leak check preset\n");
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            bcm2835_gpio_write(VENT_VALVE, HIGH);
            bcm2835_gpio_write(TANK_VALVE, HIGH);
            break;
        }
        case FILL: {
            logger.info("Entering fill preset\n");
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            bcm2835_gpio_write(VENT_VALVE, HIGH);
            bcm2835_gpio_write(TANK_VALVE, LOW);
            break;
        }
        case FILL_IDLE: {
            logger.info("Entering fill idle preset\n");
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            bcm2835_gpio_write(VENT_VALVE, HIGH);
            bcm2835_gpio_write(TANK_VALVE, HIGH);
            break;
        }
        case DEF: {
            logger.info("Entering default preset\n");
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            bcm2835_gpio_write(VENT_VALVE, LOW);
            bcm2835_gpio_write(TANK_VALVE, HIGH);
            break;
        }
        default: {
            // Defer to super visitor
            WorkerVisitor::visitCommand(c);
            break;
        }
    }
}
