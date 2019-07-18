/**
 * @file adc.cpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A set of structures and classes for reading from the ADCs.
 * @version 0.1
 * @date 2019-07-16
 * 
 * @copyright Copyright (c) 2019
 */

#include <bcm2835.h>
#include <byteswap.h>
#include <stdint.h>

#include "adc/adc.hpp"

adc_reader::adc_reader() {
}

uint16_t adc_reader::read_item(uint8_t sensor_index) {
	adc_info info;

	if (sensor_index < 0 || sensor_index > SENSOR::NUM_SENSORS)
		return -1;
	info = adc_infos[sensor_index];

	/*
	 * See datasheet for MCP3204 ADC for the SPI interface.
	 *
	 * Set on, single mode, channel.
	 */
	char channel = 0x01 << 4 | 0x01 << 3 | (char) info.channel;
	char write_buf[3] = {channel, 0, 0};
	char read_buf[3] = {0, 0, 0};

	bcm2835_spi_chipSelect(info.cs_pin);

	bcm2835_spi_transfernb(write_buf, read_buf, 3);

	/* Annoying formatting because the return value is split across two bytes. */
	read_buf[2] = (uint8_t)(((read_buf[2] >> 2) | ((read_buf[1] & 0x03) << 6)) & 0xFF);
	read_buf[1] = (uint8_t)((read_buf[1] >> 2) & 0xFF);

	/* Swap endianness of last two bytes and return */
	return __bswap_16(*(uint16_t *)(read_buf + 1));
}

void adc_reader::add_adc_info(uint8_t sensor_index, RPiGPIOPin cs_pin, uint8_t channel) {
	if (sensor_index < 0 || sensor_index > SENSOR::NUM_SENSORS)
		return;

	adc_infos[sensor_index] = adc_info(cs_pin, channel);
}
