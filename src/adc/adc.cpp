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

	// TCs
	RPI_GPIO_P1_24,
	RPI_GPIO_P1_24,
	RPI_GPIO_P1_24
};

uint8_t SENSOR_CHANNELS[NUM_SENSORS] = {
	0, // LC_MAIN
	1, // LC1
	3, // LC2
	4, // LC3

	2, // PT_COMBUSTION
	1, // PT_INJECTOR
	0, // PT_FEED

	4, // TC1
	5, // TC2
	6  // TC3
};

uint16_t SENSOR_FREQS[NUM_SENSORS] = {
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

char SENSOR_NAMES[NUM_SENSORS][20] = {
	"LC_MAIN",
	"LC1",
	"LC2",
	"LC3",
	"PT_COMBUSTION",
	"PT_INJECTOR",
	"PT_FEED",
	"TC1",
	"TC2",
	"TC3"
};

adc_reader::adc_reader() {
}

// TODO add logic for going into nap mode for slower sensors (wait 60ms)
// TODO initiate dummy conversion in init after ADC power on.

uint16_t adc_reader::read_item(uint8_t sensor_index) {
	adc_info info;
	char channel;

	if (sensor_index < 0 || sensor_index > SENSOR::NUM_SENSORS)
		return -1;
	info = adc_infos[sensor_index];

	/*
	 * See datasheet for LTC1876 ADC for the SPI interface. Page 7, Table 1
	 */

	// Set the 3-bit channel field
	if (info.channel % 2 == 0) 
		channel = info.channel / 2;
	else
		channel = 1 << 2 | ((info.channel - 1) / 2);

	char input = 1 << 6 | 		// Single mode (not differential)
		     channel << 5 | 	// Channel select
		     0 << 2 | 		// Keep CH7 as CH7, not COMMON
		     1 << 1 | 		// Unipolar (straight binary output)
		     0; 		// No sleep mode

	char write_buf[2] = {input, 0};
	char read_buf[2] = {0, 0};

	// Short active high pulse to start a conversion (return low within 100 ns)
	// See Figure 7
	bcm2835_gpio_write(info.cs_pin, HIGH);
	bcm2835_gpio_write(info.cs_pin, LOW);

	bcm2835_spi_transfernb(write_buf, read_buf, 3);

	// Ignore the first input because it used a different config input.
	// TODO is this right?
	
	// Start new conversion with our current config input.
	bcm2835_gpio_write(info.cs_pin, HIGH);
	bcm2835_gpio_write(info.cs_pin, LOW);

	bcm2835_spi_transfernb(write_buf, read_buf, 2);

	// Output is MSB first
	return (read_buf[0] << 8) | read_buf[1];
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
