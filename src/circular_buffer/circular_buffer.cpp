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
#include <stdio.h>

#include "circular_buffer/circular_buffer.hpp"
#include "time/time.hpp"
#include "adc/adc.hpp"

circular_buffer::circular_buffer(SENSOR sensor, uint16_t num_items)
	: sensor(sensor)
	{
		data = new data_item[num_items];
		end = data + num_items + 1;
		head = data;
		tail = data;
	};

uint16_t circular_buffer::get_data(uint8_t **bufptr, uint16_t size) {
	uint8_t *buf = *bufptr;
	struct data_header *header = (struct data_header *)buf;
	uint16_t bytes_to_copy, bytes_written = sizeof(struct data_header);

	/* TODO account for struct padding */
	/* Write the data as long as there is data to add */
	while (bytes_written + sizeof(struct data_item) <= size && 
	       pop_data_item(&buf[bytes_written]) != BUFF_STATUS::EMPTY) {
		bytes_written += sizeof(struct data_item);
	}

	/* Write the header */
	header->sensor = sensor;
	header->length = bytes_written;

	/* TODO check validity? */
	header->valid = 0;

	// printf("Bytes written: %lu\n", bytes_written);

	return bytes_written;
}

BUFF_STATUS circular_buffer::push_data_item(uint16_t reading, timestamp_t timestamp) {
	struct data_item *next = head + 1;

	/* Check for wrap around */
	if (next >= end) 
		next = data;

	/* Check if the buffer is full */
	/* TODO log this? */
	if (next == tail) {
		// printf("Buffer is full\n");
		return BUFF_STATUS::FULL;
	}

	/* Write the new reading into the buffer */
	head->reading = reading;
	head->timestamp = timestamp;

	head = next;

	return BUFF_STATUS::JUSTRIGHT;
}

BUFF_STATUS circular_buffer::pop_data_item(uint8_t *item) {
	struct data_item *d_item = (struct data_item*)item;

	/* Check if the buffer is empty */
	if (head == tail) {
		printf("Buffer is empty\n");
		return BUFF_STATUS::EMPTY;
	}

	/* Return the information */
	d_item->reading = tail->reading;
	d_item->timestamp = tail->timestamp;

	/* Check for wrap around when incrementing */
	tail++;
	if (tail >= end)
		tail = data;
}
