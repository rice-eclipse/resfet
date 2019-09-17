#include "worker_visitor.hpp"
#include "../final/timed_item.hpp"
#include "../final/main_buff_logger.hpp"

int engine_type;
int time_between_gitvc;
int gitvc_wait_time;
float pressure_slope;
float pressure_yint;
int pressure_min;
int pressure_max;
int preignite_us;
int hotflow_us;
bool ignition_on;
bool pressure_shutoff;
bool use_gitvc;
std::vector<int> gitvc_times;

void worker_visitor::check_ti_list(timestamp_t t, safe_queue<work_queue_item> &qw) {
    int i, ti_seen = 0;
    work_queue_item wqi = {};
    for (i = 0; i < MAX_TIMED_LIST_LEN && ti_seen < TI_COUNT; i++) {
        if (ti_list->tis[i].action != wq_none) {
            ti_seen++;
            if (ti_list->tis[i].enabled && t > ti_list->tis[i].scheduled && t - ti_list->tis[i].scheduled > ti_list->tis[i].time_delay) {
                // Add this to the list of items to process:
                wqi.action = wq_timed;
                wqi.extra_datap = &ti_list->tis[i];
                qw.enqueue(wqi);
            }
        }
    }
}

void worker_visitor::visit(work_queue_item& item) {
    switch (item.action) {
        case wq_process: {
            visitProc(item);
            break;
        }
        case wq_timed: {
            visitTimed(item);
            break;
        }
        case ign1: {
            visitIgn(item);
            break;
        }
        case wq_none: {
            visitNone(item);
            break;
        }
        default: {
            visitDefault(item);
            break;
        }
    }
}

void worker_visitor::visitProc(work_queue_item& wq_item) {
    logger.info("In worker_visitor process case");
    char c = wq_item.data[0];

    logger.debug("Processing request on worker.");

    switch (c) {
        case unset_valve: {
            logger.info("Writing main valve off on pin " + std::to_string(MAIN_VALVE), now);
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            break;
        }
        case set_valve: {
            logger.info("Writing main valve on on pin " + std::to_string(MAIN_VALVE), now);
            bcm2835_gpio_write(MAIN_VALVE, HIGH);
            break;
        }
        case unset_ignition: {
            // TODO does this change for titan?
            logger.info("Writing ignition off.", now);
            bcm2835_gpio_write(IGN_START, LOW);
            logger.info("Writing main valve off.", now);
            bcm2835_gpio_write(MAIN_VALVE, LOW);
            break;
        }
        case set_ignition: {
            logger.info("Writing ignition on.", now);
            bcm2835_gpio_write(IGN_START, HIGH);
            break;
        }
        case ign_normal: {
            logger.info("Beginning ignition process.", now);
            wq_item.action = ign1;
            qw.enqueue(wq_item);
            break;
        }
        default: {
            wq_item.action = wq_none;
            qw.enqueue(wq_item);
            break;
        }
    }
}

void worker_visitor::visitTimed(work_queue_item& wq_item) {
    // Get the current time
    now = get_time();

    // Get the timed item that added this:
    timed_item *ti = wq_item.extra_datap;

    ti->scheduled = now;

    if (ti->buffer != NULL) {
        //TODO make a Logger call to debugv
        /*
        std::cout << "Reading adc please work" << ti->adc_info.pin << " "
                    << ti->adc_info.single_channel << " " << (int) ti->adc_info.channel << std::endl;

        std::cout << "Testing ADC read from ADC 2: " << \
            adcs.read_item(ti->adc_info.pin, true, ti->adc_info.channel) << std::endl;
        */
        adc_data.dat = adcs.read_item(ti->adc_info);
        //adc_data.dat = adcs.read_item(2, ti->adc_info.single_channel, ti->adc_info.channel);
        //std::cout << "Read value from ADC: " << adc_data.dat << std::endl;
        //adc_data.dat = count++;
        //usleep(100);
        //FIXME switch this.

        if (ti->action == pt_comb && pressure_shutoff) {
            double pt_cal = pressure_slope * adc_data.dat + pressure_yint;
            pressure_avg = pressure_avg * 0.95 + pt_cal * 0.05; // Running average

            if ((pressure_avg > pressure_max || pressure_avg < pressure_min) && burn_on) {
                // Start after 1000ms = 1s.
                if (now - start_time_nitr > 1000000) {
                    // GITVC is active low
                    logger.error("Pressure shutoff: " + std::to_string(pressure_avg) + " . Closing main valve and unsetting ignition.", now);
                    logger.error("Max/Min set to " + std::to_string(pressure_max) + "/" + std::to_string(pressure_min), now);
                    logger.error("Slope/y-int set to " + std::to_string(pressure_slope) + "/" + std::to_string(pressure_yint), now);

                    bcm2835_gpio_write(MAIN_VALVE, LOW);
                    bcm2835_gpio_write(IGN_START, LOW);
                    bcm2835_gpio_write(GITVC_VALVE, HIGH);
                    burn_on = false;
                }
            }
        }

        adc_data.t = now;
        ti->buffer->add_data(&adc_data, sizeof(adc_data));
        //TODO add some debugv info with information on this logger.(@patrickhan)
        // Now see if it has been long enough that we should send data:
        if (now - ti->last_send > SEND_TIME) {
            size_t bw = ti->buffer->bytes_written.load();
            logger.debugv("Will do sending", now);
            // TODO improve this logger (@patrickhan)

            //Send some data:
            nq_item.action = nq_send;
            nq_item.nbytes = bw - ti->nbytes_last_send;
            nq_item.total_bytes = ti->nbytes_last_send;
            nq_item.data[0] = ti->action; // Include info on which buffer this is from.
            nq_item.buff = ti->buffer;
            ti->nbytes_last_send = bw;
            ti->last_send = now;

            // Write the object if we have something to send and we are connected.
            if (nw_ref->connected && nq_item.nbytes > 0) {
                qn.enqueue(nq_item);
            }
            // Next we also write the data to a (binary) log file by just directly dumping it from buff.
            write_from_nqi(nq_item);
        }
    }
}

void worker_visitor::visitIgn(work_queue_item& wq_item) {
    now = get_time();
    logger.info("Beginning ignition process", now);
    if (ignition_on) {
        bcm2835_gpio_write(IGN_START, HIGH);
        logger.info("Hotflow on", now);
    } else {
        logger.info("Hotflow not on due to configuration", now);
    }

    // Enable main valve on after the preignite period
    std::cout << "main_visitor::visitIgn setting preignite_us and hotflow_us: " << preignite_us << "    " << hotflow_us << '\n';
    ti_list->set_delay(ign2, preignite_us);
    ti_list->enable(ign2, now);

    ti_list->set_delay(ign3, hotflow_us);
}

void worker_visitor::visitNone(work_queue_item& wq_item) {
    now = get_time();
    check_ti_list(now, qw);
}

void worker_visitor::visitDefault(work_queue_item& wq_item) {
    logger.error("Work queue item not handled:" + std::to_string(wq_item.action), now);
}
