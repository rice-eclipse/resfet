/*
 * Implementations of methods in queue_visitor_imps.hpp
 */

#include "titan_visitor.hpp"
#include <climits>
#include <unistd.h>
#include "../final/main_buff_logger.hpp"

void titan_visitor::visitProc(work_queue_item& wq_item) {
    logger.info("In titan_visitor process case");
    char c = wq_item.data[0];

    logger.debug("Processing request on worker.");

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
            worker_visitor::visitProc(wq_item);
            break;
        }
    }
}

void titan_visitor::visitTimed(work_queue_item& wq_item) {
    // Get the current time
    now = get_time();

    // Get the timed item that added this:
    timed_item *ti = wq_item.extra_datap;

    ti->scheduled = now;

    if (ti->buffer != NULL) {
        worker_visitor::visitTimed(wq_item);
    } else { // Handle the cases of using ignition stuff.
        if (ti->action == ign2) { // close MAIN, open VENT, close TANK
            ti_list->disable(ign2);
            ti_list->enable(ign3, now);

            logger.info("Writing tank off, vent on, main valve on from timed item.", now);

            bcm2835_gpio_write(MAIN_VALVE, LOW);
            bcm2835_gpio_write(VENT, LOW); // VENT open is LOW, not HIGH
            bcm2835_gpio_write(TANK, HIGH); // TANK off is HIGH, not LOW

            start_time_nitr = now;
            burn_on = true;
        }
        if (ti->action == ign3) { // After ign2, close MAIN_VALVE and return to default state
            logger.info("Ending burn.", now);
            burn_on = false;

            ti_list->disable(ign3);

            logger.debug("Writing ignition off from timed item.", now);
            bcm2835_gpio_write(IGN_START, LOW);

            /*
             * TODO we don't need to close a valve since the ignition valve state
             * is the same as the default valve state. Is there a potential issue
             * with overburning?
             */
        }
    }
}