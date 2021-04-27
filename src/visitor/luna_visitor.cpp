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
#include <cstdio>

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
	, gitvc_times_ms(std::vector<uint32_t>())
{
	config.getBool("Luna", "use_gitvc", &use_gitvc);
	config.getInt("Luna", "time_between_gitvc_ms", &time_between_gitvc_ms);
	config.getInt("Luna", "gitvc_wait_time_ms", &gitvc_wait_time_ms);
	config.getVector("Luna", "gitvc_times_ms", &gitvc_times_ms);
	// TODO get vector of gitvc times
	logger.debug("use_gitvc: %d\n", use_gitvc);
	logger.debug("time_between_gitvc_ms: %d\n", time_between_gitvc_ms);
	logger.debug("gitvc_wait_time_ms: %d\n", gitvc_wait_time_ms);

	for (int index = 0; index < gitvc_times_ms.size(); index++)
		logger.debug("gitvc_times_ms: %d\n", gitvc_times_ms[index]);
}

void LunaVisitor::visitCommand(COMMAND c) {
    switch (c) {
        case UNSET_DRIVER1: {
	    logger.info("Writing main valve off using pin %d\n", MAIN_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            break;
        }
        case SET_DRIVER1: {
	    logger.info("Writing main valve on using pin %d\n", MAIN_VALVE);
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            break;
        }
        case UNSET_DRIVER2: {
	    logger.info("Writing pressurization valve off using pin %d\n", PRESSURE_VALVE);
            bcm2835_gpio_write(PRESSURE_VALVE, LOW);
            break;
        }
        case SET_DRIVER2: {
	    logger.info("Writing pressurization valve on using pin %d\n", PRESSURE_VALVE);
            bcm2835_gpio_write(PRESSURE_VALVE, HIGH);
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
