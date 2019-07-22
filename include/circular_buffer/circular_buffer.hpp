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
#include "time/time.hpp"
#include "adc/adc.hpp"

/**
 * @brief The header of a packet that describes the type of
 * 	  sensor and number of following data_items.
 */
struct data_header {
	SENSOR sensor;
	uint16_t length;
};

/**
 * @brief A combination of an ADC reading and a timestamp of
 * 	  when the reading was taken.
 */
struct data_item {
	uint16_t reading;
	timestamp_t timestamp;
};

class circular_buffer {
	private:
		/* @brief A pointer to the end of this circular buffer */
		struct data_item *end;

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
		circular_buffer(SENSOR sensor, uint16_t size);

		/**
		 * @brief Copies a header and new data into the provided buffer.
		 *
		 * @param buf A pointer to an array that will be populated with
		 * 	      a new header and data.
		 *
		 * @return The number of bytes written to the buffer.
		 */
		uint8_t get_data(uint8_t **bufptr, uint16_t size);

		/**
		 * @brief Adds a new data_item to the internal buffer.
		 *
		 * @param reading The reading of the new data_item.
		 * @param timestamp The timestamp of the new data_item.
		 */
		void add_data_item(uint16_t reading, timestamp_t timestamp);

		/**
		 * @brief Calculates the number of bytes of unsent data.
		 *
		 * @return The number of bytes of new data.
		 */
		uint16_t get_data_length();

};

#endif
