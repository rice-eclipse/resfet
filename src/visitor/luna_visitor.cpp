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

#include "config/config.hpp"
#include "commands/rpi_pins.hpp"
#include "visitor/luna_visitor.hpp"

LunaVisitor::LunaVisitor()
    : gitvc_on(false)
    , gitvc_count(0)
    , WorkerVisitor()
{
    
}

LunaVisitor::LunaVisitor(ConfigMapping& config)
	: gitvc_on(false)
	, gitvc_count(0)
	, WorkerVisitor(config)
{
	config.getBool("Luna", "use_gitvc", &use_gitvc);
	config.getInt("Luna", "time_between_gitvc_ms", &time_between_gitvc_ms);
	config.getInt("Luna", "gitvc_wait_time_ms", &gitvc_wait_time_ms);
	// TODO get vector of gitvc times
	logger.debug("use_gitvc: %d\n", use_gitvc);
	logger.debug("time_between_gitvc_ms: %d\n", time_between_gitvc_ms);
	logger.debug("gitvc_wait_time_ms: %d\n", gitvc_wait_time_ms);
}

void LunaVisitor::visitCommand(COMMAND c) {
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
            bcm2835_gpio_write(WATER_VALVE, LOW);
            break;
        }
        case SET_VALVE2: {
	    logger.info("Writing water valve on using pin %d\n", WATER_VALVE);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            break;
        }
        case UNSET_VALVE3: {
	    logger.info("Writing GITVC valve off using pin %d\n", GITVC_VALVE);
            bcm2835_gpio_write(GITVC_VALVE, LOW);
            break;
        }
        case SET_VALVE3: {
	    logger.info("Writing GITVC valve on using pin %d\n", GITVC_VALVE);
            bcm2835_gpio_write(GITVC_VALVE, HIGH);
            break;
        }
        default: {
            // Defer to super visitor
            WorkerVisitor::visitCommand(c);
            break;
        }
    }
}

void LunaVisitor::doGITVC() {
    // TODO
}
