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
    switch (c) {
        case UNSET_DRIVER1: {
	    logger.info("Writing main feed line fill valve off using pin %d\n", MAIN_FEED_VALVE);
            bcm2835_gpio_write(MAIN_FEED_VALVE, LOW);
            break;
        }
        case SET_DRIVER1: {
	    logger.info("Writing main feed line fill valve on using pin %d\n", MAIN_FEED_VALVE);
            bcm2835_gpio_write(MAIN_FEED_VALVE, HIGH);
            break;
        }
        case UNSET_DRIVER2: {
            logger.info("Turning oxidizer tank valve off using pin %d\n", OXI_VALVE);
            bcm2835_gpio_write(OXI_VALVE, LOW);
            break;
        }
        case SET_DRIVER2: {
            logger.info("Turning oxidizer tank valve on using pin %d\n", OXI_VALVE);
            bcm2835_gpio_write(OXI_VALVE, HIGH);
            break;
        }
        case UNSET_DRIVER3: {
	    logger.info("Writing ground vent valve off using pin %d\n", GROUND_VENT_VALVE);
            bcm2835_gpio_write(GROUND_VENT_VALVE, LOW);
            break;
        }
        case SET_DRIVER3: {
	    logger.info("Writing ground vent valve on using pin %d\n", GROUND_VENT_VALVE);
            bcm2835_gpio_write(GROUND_VENT_VALVE, HIGH);
            break;
        }
        case TITAN_LEAK_CHECK: {
            logger.info("Entering leak check preset\n");
            // bcm2835_gpio_write(MAIN_VALVE, HIGH);
            // bcm2835_gpio_write(VENT_VALVE, HIGH);
            // bcm2835_gpio_write(TANK_VALVE, HIGH);
            break;
        }
        case TITAN_FILL: {
            logger.info("Entering fill preset\n");
            // bcm2835_gpio_write(MAIN_VALVE, HIGH);
            // bcm2835_gpio_write(VENT_VALVE, HIGH);
            // bcm2835_gpio_write(TANK_VALVE, LOW);
            break;
        }
        case TITAN_FILL_IDLE: {
            logger.info("Entering fill idle preset\n");
            // bcm2835_gpio_write(MAIN_VALVE, LOW);
            // bcm2835_gpio_write(VENT_VALVE, HIGH);
            // bcm2835_gpio_write(TANK_VALVE, HIGH);
            break;
        }
        case TITAN_DEF: {
            logger.info("Entering default preset\n");
            // bcm2835_gpio_write(MAIN_VALVE, LOW);
            // bcm2835_gpio_write(VENT_VALVE, LOW);
            // bcm2835_gpio_write(TANK_VALVE, HIGH);
            break;
        }
        default: {
            // Defer to super visitor
            WorkerVisitor::visitCommand(c);
            break;
        }
    }
}
