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
#include <mutex>
#include <stdint.h>

#include "adc/adc.hpp"

/**
 * @brief Global lock to ensure sequential ADC reads.
 */
std::mutex adc_mutex;

/**
 * @brief The pins of the respective ADCs for each sensor.
 */
RPiGPIOPin SENSOR_PINS[SENSOR::NUM_SENSORS] = {
	// LCs
	RPI_GPIO_P1_22,
	RPI_GPIO_P1_22,
	RPI_GPIO_P1_22,
	RPI_GPIO_P1_22,
	
	// PTs
	RPI_GPIO_P1_24,
	RPI_GPIO_P1_24,
	RPI_GPIO_P1_24,
	RPI_GPIO_P1_24,

	// TCs
	RPI_GPIO_P1_24,
	RPI_GPIO_P1_24,
	RPI_GPIO_P1_24
};

uint8_t SENSOR_CHANNELS[NUM_SENSORS] = {
	0, // LC1 (prev. LC_MAIN)
	1, // LC2
	2, // LC3
	3, // LC4

	3, // PT_COMBUSTION
	2, // PT_INJECTOR
	1, // PT_FEED
	0, // PT_OXI

	4, // TC1
	5, // TC2
	6 // TC3
};

uint16_t SENSOR_FREQS[NUM_SENSORS] = {
	2000,
	2000,
	2000,
	2000,
	500,
	500,
	500,
	500,
	20,
	20,
	20
};

char SENSOR_NAMES[NUM_SENSORS][20] = {
	"LC1",
	"LC2",
	"LC3",
	"LC4",
	"PT1",
	"PT2",
	"PT3",
	"PT4",
	"TC1",
	"TC2",
	"TC3"
};

adc_reader::adc_reader() {
}

uint16_t adc_reader::read_item(uint8_t sensor_index) {
	// Lock the mutex.
	adc_mutex.lock();
	
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

	// bcm2835_spi_chipSelect(info.cs_pin);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);
	bcm2835_gpio_write(info.cs_pin, LOW);

	bcm2835_spi_transfernb(write_buf, read_buf, 3);

	bcm2835_gpio_write(info.cs_pin, HIGH);

	/* Annoying formatting because the return value is split across two bytes. */
	read_buf[2] = (uint8_t)(((read_buf[2] >> 2) | ((read_buf[1] & 0x03) << 6)) & 0xFF);
	read_buf[1] = (uint8_t)((read_buf[1] >> 2) & 0xFF);

	adc_mutex.unlock();

	/* Swap endianness of last two bytes and return */
	return __bswap_16(*(uint16_t *)(read_buf + 1));
}

uint16_t adc_reader::count_up() {
	static uint16_t num = 0;
	return num++;
}

void adc_reader::add_adc_info(uint8_t sensor_index, RPiGPIOPin cs_pin, uint8_t channel) {
	if (sensor_index < 0 || sensor_index > SENSOR::NUM_SENSORS)
		return;

	adc_infos[sensor_index] = adc_info(cs_pin, channel);
}
