/**
 * @file thread.cpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Thread that reads and logs sensor data at some frequency.
 * @version 0.1
 * @date 2019-07-22
 * 
 * @copyright Copyright (c) 2019
 */

#include <pthread.h>
#include <stdint.h>
#include <vector>

#include "adc/adc.hpp"
#include "thread/thread.hpp"
#include "logger/logger.hpp"
#include "time/time.hpp"

#define BUFF_SIZE	4096

PeriodicThread::PeriodicThread(uint16_t frequency_hz, SENSOR *sensors, uint8_t num_sensors)
	{
		int index;

		param.reader = adc_reader();

		/* TODO assume we don't sleep for more than 1s */
		param.sleep_time_ns = (1.0 / (double)frequency_hz) * 1000000000;

		param.buffers = new std::vector<circular_buffer>;
		param.loggers = new std::vector<Logger>;

		for (index = 0; index < num_sensors; index++) {
			param.buffers->push_back(circular_buffer(sensors[index], BUFF_SIZE));
			param.loggers->push_back(Logger(SENSOR_NAMES[sensors[index]],
					SENSOR_NAMES[sensors[index]], LogLevel::SILENT));
		}

		param.num_sensors = num_sensors;
	}

void *threadFunc(void *param) {
	struct thread_param *t_param = (struct thread_param *)param;
	struct timespec rem, spec;
	std::vector<circular_buffer>::iterator it;
	std::vector<Logger>::iterator it_log;
	uint16_t reading;
	timestamp_t timestamp, old_timestamp = 0;
	BUFF_STATUS status;
	uint8_t *b = new uint8_t[BUFF_SIZE];
	
	while(1) {
		spec.tv_sec = 0;
		spec.tv_nsec = t_param->sleep_time_ns;
		// TODO check this timing is accurate
		while (nanosleep(&spec, &rem) == -1) {
			printf("New wait time: %lu\n", rem.tv_nsec);
			spec.tv_nsec = rem.tv_nsec;
		}

		it_log = t_param->loggers->begin();
		for (it = t_param->buffers->begin(); it != t_param->buffers->end(); ++it) {
			// TODO replace count_up with actual sampling
			reading = t_param->reader.count_up();
			timestamp = get_elapsed_time_us();
			// printf("reading: %d timestamp: %lu\n", reading, timestamp);
			status = it->push_data_item(reading, timestamp);
			if (status == BUFF_STATUS::FULL) {
				it->get_data(&b, BUFF_SIZE);
				it_log->data(b);
			}
			it_log++;
			printf("reading: %d Timestamp delta: %lu\n", reading, timestamp - old_timestamp);
			old_timestamp = timestamp;
		}
	}
}

void PeriodicThread::start() {
	pthread_create(&thread, NULL, threadFunc, (void *)&param);
}
