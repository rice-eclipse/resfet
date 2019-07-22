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

#include <stdint.h>
#include <string.h>

#include "circular_buffer/circular_buffer.hpp"
#include "time/time.hpp"
#include "adc/adc.hpp"

circular_buffer::circular_buffer(SENSOR sensor, uint16_t size)
	: sensor(sensor)
	{
		data = new data_item[size];
		end = data + size;
		head = data;
		tail = data;
	};

uint8_t circular_buffer::get_data(uint8_t **bufptr, uint16_t size) {
	uint8_t *buf = *bufptr;
	struct data_header *header = (struct data_header *)buf;
	struct data_item *data_start = (struct data_item *)(buf + sizeof(struct data_header));
	uint16_t bytes_to_copy;

	/* Check if the buffer is empty */
	if (head == tail)
		return 0;

	/* Write the header */
	header->sensor = sensor;
	header->length = get_data_length(); 

	/* Copy in as much data as we can without overflowing the buffer */
	bytes_to_copy = header->length < size ? header->length : size;
	memcpy(data_start, tail, bytes_to_copy);

	tail += bytes_to_copy / sizeof(struct data_item);
}

void circular_buffer::add_data_item(uint16_t reading, timestamp_t timestamp) {
	struct data_item *next = head + 1;

	/* Check for wrap around */
	if (next >= end) 
		next = data;

	/* Check if the buffer is full */
	/* TODO log this? */
	if (next == tail) 
		return;

	/* Write the new reading into the buffer */
	head->reading = reading;
	head->timestamp = timestamp;

	head = next;
}

uint16_t circular_buffer::get_data_length() {
	/* TODO maybe just have a variable we keep track of for number of unwritten items */
	if (head > tail)
		return (uint16_t)(head - tail) * sizeof(struct data_item);
	else
		return (uint16_t)((end - tail) + (head - data)) * sizeof(struct data_item);
}
