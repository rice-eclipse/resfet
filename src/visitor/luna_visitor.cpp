/*
 * Implementation of Luna visitor.
 */

#include "luna_visitor.hpp"
#include <climits>
#include <unistd.h>
#include "../final/main_buff_logger.hpp"

void luna_visitor::visitProc(work_queue_item& wq_item) {
    logger.info("In luna_visitor process case");
    char c = wq_item.data[0];

    logger.debug("Processing request on worker.");

    switch (c) {
        case set_water: {
            logger.info("Turning water on on pin " + std::to_string(WATER_VALVE), now);
            bcm2835_gpio_write(WATER_VALVE, HIGH);
            break;
        }
        case unset_water: {
            logger.info("Turning water off on pin " + std::to_string(WATER_VALVE), now);
            bcm2835_gpio_write(WATER_VALVE, LOW);
            break;
        }
        case set_gitvc: {
            logger.info("Turning gitvc on on pin " + std::to_string(GITVC_VALVE), now);
            bcm2835_gpio_write(GITVC_VALVE, LOW);
            break;
        }
        case unset_gitvc: {
            logger.info("turning gitvc off on pin " + std::to_string(GITVC_VALVE), now);
            bcm2835_gpio_write(GITVC_VALVE, HIGH);
            break;
        }
        default: {
            worker_visitor::visitProc(wq_item);
            break;
        }
    }
}

void luna_visitor::visitTimed(work_queue_item& wq_item) {
    // Get the current time
    now = get_time();

    // Get the timed item that added this:
    timed_item *ti = wq_item.extra_datap;

    ti->scheduled = now;

    if (ti->buffer != NULL) {
        worker_visitor::visitTimed(wq_item);
    } else { // Handle the cases of using ignition stuff.
        if (ti->action == ign2) { // Open the main valve and initiative GITVC
            ti_list->disable(ign2);
            ti_list->enable(ign3, now);

            logger.info("Writing main valve on from timed item.", now);

            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            start_time_nitr = now;
            burn_on = true;

            if (use_gitvc && gitvc_times.size() > gitvc_count) {
                ti_list->enable(gitvc, now); // GITVC delay is initially set to gitvc_wait_time in timed_item_list.cpp
                logger.info("Setting GITVC to start after " + std::to_string(gitvc_wait_time) + " microseconds", now);
                logger.info("Total " + std::to_string(gitvc_times.size()) + " gitvc opens", now);
                gitvc_on = false;
            }
        }
        if (ti->action == ign3) { // End the burn and gitvc
            logger.info("Ending burn.", now);
            burn_on = false;
            logger.debug("Writing main valve off from timed item.", now);
            bcm2835_gpio_write(MAIN_VALVE, LOW);

            ti_list->disable(ign3);

            logger.debug("Writing ignition off from timed item.", now);
            bcm2835_gpio_write(IGN_START, LOW);

            ti_list->disable(gitvc);
            gitvc_count = INT_MAX; // Added security for shutting off GITVC
            logger.debug("Ending GITVC from timed item.", now);
            bcm2835_gpio_write(GITVC_VALVE, HIGH);

            bcm2835_gpio_write(WATER_VALVE, LOW);
        }
        if (ti->action == gitvc) { // Should only reach here once GITVC is set initially
            if (gitvc_on) { // Currently on, so turn it off

                // Disable current GITVC
                bcm2835_gpio_write(GITVC_VALVE, HIGH);
                gitvc_on = false;
                logger.debug("Writing GITVC off from timed item for " + std::to_string(time_between_gitvc) + " microseconds", now);

                // Use ti to turn on new GITVC in after time_between_gitvc time passes
                ti_list->set_delay(gitvc, time_between_gitvc);
                ti_list->enable(gitvc, now);
            } else if (gitvc_times.size() > gitvc_count && burn_on){ // Currently off, so turn it on if we're still igniting

                // Re-enable GITVC
                bcm2835_gpio_write(GITVC_VALVE, LOW);
                gitvc_on = true;
                logger.debug("Writing GITVC on from timed item for " + std::to_string(gitvc_times.at(gitvc_count)) + " microseconds", now);

                // Use ti to turn off current GITVC after gitvc_times.at(gitvc_count) time passes
                ti_list->set_delay(gitvc, gitvc_times.at(gitvc_count));
                ti_list->enable(gitvc, now);

                gitvc_count++;
            }
        }
    }
}
