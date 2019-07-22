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

#include "adc/adc.hpp"
#include "thread/thread.hpp"
#include "logger/logger.hpp"

PeriodicThread::PeriodicThread(uint16_t frequency_hz, SENSOR *sensors, uint8_t num_sensors)
	// : sensors(sensors)
	{
		int index;

		param.sleep_time_ns = (1.0 / (double)frequency_hz) * 1000000000;

		for (index = 0; index < num_sensors; index++) {
			buffers[index] = circular_buffer(sensors[index], 256);
			loggers[index] = Logger(SENSOR_NAMES[sensors[index]],
					SENSOR_NAMES[sensors[index]], LogLevel::SILENT);
		}

		param.num_sensors = num_sensors;
		param.buffers = &buffers;
		param.loggers = &loggers;
	}

void *threadFunc(void *param) {
	struct thread_param t_param = *(struct thread_param *)param;
	struct timespec rem, spec;
	uint8_t index;
	
	spec.tv_sec = 0;
	spec.tv_nsec = t_param.sleep_time_ns;

	while(1) {
		while (nanosleep(&rem, &spec) == -1) {
			spec.tv_nsec = rem.tv_nsec;
		}

		for (index = 0; index < t_param.num_sensors; index++) {
			t_param.buffers[index]->add_data_item();
		}
	}
}

void PeriodicThread::start() {
	pthread_create(&thread, NULL, threadFunc, (void *)&param);
}
