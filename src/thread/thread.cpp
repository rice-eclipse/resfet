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

PeriodicThread::PeriodicThread(uint16_t frequency_hz, SENSOR *sensors, uint8_t num_sensors)
	{
		int index;

		/* TODO assume we don't sleep for more than 1s */
		param.sleep_time_ns = (1.0 / (double)frequency_hz) * 1000000000;

		param.buffers = new std::vector<circular_buffer>;
		param.loggers = new std::vector<Logger>;

		for (index = 0; index < num_sensors; index++) {
			printf("index: %d\n", index);

			param.buffers->push_back(circular_buffer(sensors[index], (uint16_t)256));
			param.loggers->push_back(Logger(SENSOR_NAMES[sensors[index]],
					SENSOR_NAMES[sensors[index]], LogLevel::INFO));
		}

		param.num_sensors = num_sensors;
	}

void *threadFunc(void *param) {
	struct thread_param *t_param = (struct thread_param *)param;
	struct timespec rem, spec;
	uint8_t index;
	std::vector<circular_buffer>::iterator it;
	std::vector<Logger>::iterator it_log;
	
	spec.tv_sec = 0;
	spec.tv_nsec = t_param->sleep_time_ns;

	while(1) {
		spec.tv_nsec = t_param->sleep_time_ns;
		while (nanosleep(&rem, &spec) == -1) {
			spec.tv_nsec = rem.tv_nsec;
		}

		it_log = t_param->loggers->begin();
		for (it = t_param->buffers->begin(); it != t_param->buffers->end(); ++it) {
			it_log->info("%s\n", "abcd");
			it_log++;
		}
	}
}

void PeriodicThread::start() {
	pthread_create(&thread, NULL, threadFunc, (void *)&param);
}
