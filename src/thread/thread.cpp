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
#include <pthread.h>
#include <stdint.h>
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
							  Udp::OutSocket *sock)
{
	this->reader = adc_reader();

	// TODO this is hard coded for the load cells
	// 0 1 3 4 for load cells
	this->reader.add_adc_info(0, (bcm2835SPIChipSelect) 25, 0);
	this->reader.add_adc_info(1, (bcm2835SPIChipSelect) 25, 1);
	this->reader.add_adc_info(2, (bcm2835SPIChipSelect) 25, 3);
	this->reader.add_adc_info(3, (bcm2835SPIChipSelect) 25, 4);

	// TODO assume we don't sleep for more than 1s
	// TODO sleep time seems to be twice as long as it should be
	this->sleep_time_ns = (1.0 / (double)frequency_hz) * 1000000000;

	this->buffers = new std::vector<circular_buffer>;
	this->loggers = new std::vector<Logger>;

	for (int index = 0; index < num_sensors; index++) {
		this->buffers->push_back(circular_buffer(sensors[index], 16));
		this->loggers->push_back(Logger(SENSOR_NAMES[sensors[index]],
						  		 SENSOR_NAMES[sensors[index]], LogLevel::DEBUG));
	}

	this->num_sensors = num_sensors;
	this->sock = sock;
}

PeriodicThread::~PeriodicThread() {
	delete this->buffers;
	delete this->loggers;
}

// The function that is run by each thread
static void *threadFunc(adc_reader reader,
                        std::vector<Logger>* loggers,
                        std::vector<circular_buffer>* buffers,
                        uint64_t sleep_time_ns,
                        uint8_t num_sensors,
                        Udp::OutSocket* sock)
{
	struct timespec rem, spec;
	std::vector<circular_buffer>::iterator it;
	std::vector<Logger>::iterator it_log;
	timestamp_t timestamp, old_timestamp = 0;
	uint16_t reading;
	uint8_t *b = new uint8_t[BUFF_SIZE];
	BUFF_STATUS status;
	
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
			timestamp = get_elapsed_time_us();
			// printf("reading: %d timestamp: %lu\n", reading, timestamp);
			// printf("reading: %d Timestamp delta: %lu\n", reading, timestamp - old_timestamp);
			status = it->push_data_item(reading, timestamp);

			/* If the circular buffer is full, send the available data */
			if (status == BUFF_STATUS::FULL) {
				it->get_data(&b, BUFF_SIZE);
				it_log->data(b, BUFF_SIZE);
				if (sock != NULL && sock->getFd() != -1)
					sock->sendBuf(b, BUFF_SIZE);
				else {
					printf("Problem with socket\n");
					return NULL;
				}
				// printf("Sent over udp\n");
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
							  this->sock);
	core_thread.detach();
}
