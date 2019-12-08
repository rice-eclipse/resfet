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

#include <atomic>
#include <thread>
#include <vector>

#include "adc/adc.hpp"
#include "circular_buffer/circular_buffer.hpp"
#include "logger/logger.hpp"
#include "networking/Udp.hpp"

// Defined in main.cpp
extern std::atomic<bool> pressureShutoff;

class PeriodicThread {
	private:
		/**
		 * @brief The actual thread for this instance. Detached from the main
		 * 		  process.
		 */
		std::thread core_thread;

		// Parameters for the thread function:

		/**
		 * @brief The ADC interface associated with this thread. 
		 */
		adc_reader reader;

		/**
		 * @brief List of loggers to which data from corresponding buffers in
		 * 		  PeriodicThread::buffers will be logged as it is read.
		 */
		std::vector<Logger>* loggers;

		/**
		 * @brief List of buffers managed by this thread. Data will be pulled
		 * 		  from each one in sequence if it is available, and logged to
		 * 		  the corresponding logger in PeriodicThread::loggers.
		 */
		std::vector<circular_buffer>* buffers;

		/**
		 * @brief The time between readings of data from the ADC, in
		 * 		  nanoseconds. 
		 */
		uint64_t sleep_time_ns;

		/**
		 * @brief The total number of sensors connected to the controller.
		 */
		uint8_t num_sensors;
                
                /**
                 * @brief Upper limit for pressure values (calibrated, not raw), above which a
                 *        safety shutoff will occur.
                 */
                double pressureMax;
                
                /**
                 * @brief Lower limit for pressure values (calibrated, not raw), above which a
                 *        safety shutoff will occur.
                 */
                double pressureMin;
                
                /**
                 * @brief Slope for the pressure calibration function, used to test for a
                 *        safety shutoff.
                 */
                double pressureSlope;
                
                /**
                 * @brief Y-intercept for the pressure calibration function, used to test
                 *        for a safety shutoff.
                 */
                double pressureYint;

		/**
		 * @brief The UDP output socket through which data will be sent as it
		 *        is logged.
		 */
		Udp::OutSocket* sock;

	public:
		/**
		 * @brief The constructor for a Periodic Thread. The thread uses an
		 * 		  adc_reader() and logger() to periodically read from an ADC
		 * 	      and save the raw data into a file. Data writes are buffered
		 * 	  	  using a circular buffer.
		 * 	
		 * @param frequency_hz the frequency to read, in Hertz
		 * @param sensors the list of sensors to sample
		 * @param num_sensors the number of sensors to be read
		 */
		PeriodicThread(uint16_t frequency_hz,
                               SENSOR *sensors,
                               uint8_t num_sensors,
                               double pressureMax,
                               double pressureMin,
                               double pressureSlope,
                               double pressureYint,
                               Udp::OutSocket *sock);

		/**
		 * @brief Destroy this thread. Frees memory associated with
		 * 		  PeriodicThread::buffers and PeriodicThread::loggers.
		 */
		~PeriodicThread();

		/**
		 * @brief Start this thread collecting and sending data autonomously.
		 */
		void start();
};

#endif
