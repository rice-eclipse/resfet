/**
 * @file thread.hpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Thread that reads and logs sensor data at some frequency.
 * @version 0.1
 * @date 2019-07-19
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef __THREAD_HPP
#define __THREAD_HPP

#include <thread>
#include <vector>

#include "adc/adc.hpp"
#include "circular_buffer/circular_buffer.hpp"
#include "logger/logger.hpp"

struct thread_param {
	uint64_t sleep_time_ns;
	uint8_t num_sensors;
	std::vector<Logger> *loggers;
	std::vector<circular_buffer> *buffers;
	adc_reader reader;
};

void *threadFunc(void *param);

class PeriodicThread {
	private:
		/* @brief The sleep time in nanoseconds */
		uint32_t sleep_time_ns;

		/* @brief The list of sensors to read from */
		// SENSOR *sensors;

		/* @brief The list of loggers */
		std::vector<Logger> loggers;

		/* @brief The list of circular buffers to store data in */
		std::vector<circular_buffer> buffers;

		/* @brief The thread that is used to do work */
		pthread_t thread;

		/* @brief The parameters to pass to the thread */
		struct thread_param param;

	public:
		/**
		 * @brief The constructor for a Periodic Thread. The thread uses an
		 * 	  adc_reader() and logger() to periodically read from an ADC
		 * 	  and save the raw data into a file. Data is stored in a
		 * 	  circular buffer before it is written.
		 * 	  TODO send over UDP.
		 * 	
		 * @param frequency_hz The frequency to read, in Hertz
		 * @param sensors The list of sensors to sample
		 */
		PeriodicThread(uint16_t frequency_hz, SENSOR *sensors, uint8_t num_sensors);

		void start();


};

#endif
