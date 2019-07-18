/**
 * @file circular_buffer.hpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Circular buffer for storing data and formatting it
 * 	  to be sent.
 * @version 0.1
 * @date 2019-07-18
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef __CIRCULAR_BUFFER_HPP
#define __CIRCULAR_BUFFER_HPP

#include <stdint.h>
#include "adc/adc.hpp"

/**
 * @brief The header of a packet that describes the type of
 * 	  sensor and number of following data_items.
 */
struct data_header {
	SENSOR sensor;
	uint16_t length;
}

/**
 * @brief A combination of an ADC reading and a timestamp of
 * 	  when the reading was taken.
 */
struct data_item {
	uint16_t reading;
	timestamp_t timestamp;
};

/**
 * @brief Data rates for each of the sensors. This shouldn't
 * 	  need to change between tests.
 */
uint8_t SENSOR_FREQS[NUM_SENSORS] = {
	2000,
	2000,
	2000,
	2000,
	500,
	500,
	500,
	20,
	20,
	20
};

class circular_buffer {
	private:
		/* @brief The starting time reference for data timestamps */
		timestamp_t start_time_us;

		/* @brief The maximum number of data_items this buffer can contain */
		uint16_t size;

		/* @brief The array of data_items this circular buffer stores */
		struct data_item *data;

		/* @brief A pointer to the head of this circular buffer */
		struct data_item *head;

		/* @brief A pointer to the tail of this circular buffer */
		struct data_item *tail;

	public:
		/* @brief The sensor type this circular buffer stores data for */
		SENSOR sensor;

		/**
		 * @brief The constructor for a circular buffer (one for each sensor).
		 *
		 * @param start_time_us The starting time offset in microseconds.
		 * @param sensor The sensor associated with this circular buffer.
		 * @param size The number of data_items this circular buffer should be able to hold.
		 */
		circular_buffer (timestamp_t start_time_us, SENSOR sensor, uint16_t size);

		/**
		 * @brief Copies a header and new data into the provided buffer.
		 *
		 * @param buf A pointer to an array that will be populated with
		 * 	      a new header and data.
		 *
		 * @return The number of bytes written to the buffer.
		 */
		uint8_t get_data_to_send(uint8_t  **buf);

		/**
		 * @brief Adds a new data_item to the internal buffer.
		 */
		void add_data_item();

};

#endif
