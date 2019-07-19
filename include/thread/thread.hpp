/**
 * @file thread.hpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Thread that reads and logs sensor data at some frequency.
 * @version 0.1
 * @date 2019-07-08
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef __THREAD_HPP
#define __THREAD_HPP

#include "adc/adc.hpp"

class PeriodicThread {
	private:
		double sleep_time;

		SENSOR sensor;

		char *log_file;

	public:
		PeriodicThread(uint16_t frequency, SENSOR sensor);


};

#endif
