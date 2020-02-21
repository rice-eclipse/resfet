/**
 * @file thread.cpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Thread that reads and logs sensor data at some frequency.
 * @version 0.1
 * @date 2019-07-22
 * 
 * @copyright Copyright (c) 2019
 */

#include <bcm2835.h>
#include <mutex>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

#include "adc/adc.hpp"
#include "commands/rpi_pins.hpp"
#include "logger/logger.hpp"
#include "thread/thread.hpp"
#include "time/time.hpp"
#include "networking/Udp.hpp"

/* TODO this has to be 16n + 4 */
#define BUFF_SIZE	260

PeriodicThread::PeriodicThread(uint16_t frequency_hz,
                               SENSOR *sensors,
                               uint8_t num_sensors,
                               double pressureMax,
                               double pressureMin,
                               double pressureSlope,
                               double pressureYint,
                               Udp::OutSocket *sock)
{
        // Set pressure cutoff info
        this->pressureMax = pressureMax;
        this->pressureMin = pressureMin;
        this->pressureSlope = pressureSlope;
        this->pressureYint = pressureYint;
        
        // Set up ADC block
	this->reader = adc_reader();

	// Register each sensor with the ADC reader
	for (int i = 0; i < num_sensors; i++) {
            this->reader.add_adc_info(sensors[i], SENSOR_PINS[sensors[i]], SENSOR_CHANNELS[sensors[i]]);
	}

	// TODO assume we don't sleep for more than 1s
	// TODO sleep time seems to be twice as long as it should be
	this->sleep_time_ns = (1.0 / (double)frequency_hz) * 1000000000;

	this->buffers = new std::vector<circular_buffer>;
	this->loggers = new std::vector<Logger>;

	for (int index = 0; index < num_sensors; index++) {
		this->buffers->push_back(circular_buffer(sensors[index], 16));
		this->loggers->push_back(
                    Logger(SENSOR_NAMES[sensors[index]],
                           SENSOR_NAMES[sensors[index]],
                           LogLevel::DEBUG)
                );
	}

	this->num_sensors = num_sensors;
	this->sock = sock;
}

PeriodicThread::~PeriodicThread() {
	delete this->buffers;
	delete this->loggers;
}

// Perform a conversion from a raw ADC reading value to a calibrated value
//static double convertReading(uint16_t reading, double slope, double yint) {
//        return slope * reading + yint;
//}

// The function that is run by each thread
static void *threadFunc(adc_reader reader,
                        std::vector<Logger>* loggers,
                        std::vector<circular_buffer>* buffers,
                        uint64_t sleep_time_ns,
                        uint8_t num_sensors,
                        double pressureMax,
                        double pressureMin,
                        double pressureSlope,
                        double pressureYint,
                        Udp::OutSocket* sock)
{
	struct timespec rem, spec;
	std::vector<circular_buffer>::iterator it;
	std::vector<Logger>::iterator it_log;
	timestamp_t timestamp, old_timestamp = 0;
	uint16_t reading;
	uint8_t *b = new uint8_t[BUFF_SIZE];
	BUFF_STATUS status;
        
        // Running average for PT_COMB
        double combAvg = 700; // initialize to a reasonable value
	
	// TODO: ever break out of this loop?
	while(1) {
		spec.tv_sec = 0;
		spec.tv_nsec = sleep_time_ns;

		// Sleep multiple times if we get interrupted while sleeping
		while (nanosleep(&spec, &rem) == -1) {
			printf("New wait time: %lu\n", rem.tv_nsec);
			spec.tv_nsec = rem.tv_nsec;
		}

		it_log = loggers->begin();
		for (it = buffers->begin(); it != buffers->end(); ++it) {
#ifdef MOCK
			reading = reader.count_up();
#else
			reading = reader.read_item(it->sensor);
#endif
            // Include the reading in the running average
			if (it->sensor == PT_COMBUSTION) {
				double converted = pressureSlope * reading + pressureYint;
				combAvg = combAvg * 0.95 + converted * 0.05;
                        
				// If the average is beyond the cutoff threshold, signal the cutoff
				//printf("Running avg: %f\n", (float) combAvg);
				if (combAvg > pressureMax || combAvg < pressureMin) {
				    pressureShutoff.store(true);
					printf("Stored pressure shutoff: %d\n", (int) pressureShutoff.load());
				} else {
					if (pressureShutoff.load()) {
						printf("Pressure returned to nominal\n");
					}
					pressureShutoff.store(false);
				}
			}
                        
			timestamp = get_elapsed_time_us();
			status = it->push_data_item(reading, timestamp);

			/* If the circular buffer is full, send the available data */
			if (status == BUFF_STATUS::FULL) {
				it->get_data(&b, BUFF_SIZE);
				it_log->data(b, BUFF_SIZE);
				
				// Send the data over UDP
				if (sock != NULL && sock->getFd() != -1)
					try {
						sock->sendBuf(b, BUFF_SIZE);
					} catch (Udp::OpFailureException&) {
						printf("Op failure!\n");
					} catch (Udp::BadOutSocketException&) {
						printf("Bad socket!\n");
					} catch (...) {
						printf("Unknown error!\n");
					}
				else {
					printf("Problem with socket\n");
					return NULL;
				}
			}
                        
			old_timestamp = timestamp;
			it_log++;
		}
	}

	// Clean up
	delete b;
}

void PeriodicThread::start() {
	core_thread = std::thread(threadFunc,
                                  this->reader,
                                  this->loggers,
                                  this->buffers,
                                  this->sleep_time_ns,
                                  this->num_sensors,
                                  this->pressureMax,
                                  this->pressureMin,
                                  this->pressureSlope,
                                  this->pressureYint,
                                  this->sock);
	core_thread.detach();
}
