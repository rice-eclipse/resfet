/**
 * @file adc.hpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A set of structures and classes for reading from the ADCs.
 * @version 0.1
 * @date 2019-07-16
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef __ADC_HPP
#define __ADC_HPP

#include <bcm2835.h>
#include <stdint.h>

struct adc_info {
	RPiGPIOPin cs_pin;
	uint8_t channel;

	adc_info() = default;

	adc_info(RPiGPIOPin cs_pin, uint8_t channel)
		: cs_pin(cs_pin)
		, channel(channel)
		{};

};

enum SENSOR {
		LC_MAIN = 0,
		LC1,
		LC2,
		LC3,
		PT_COMBUSTION,
		PT_INJECTOR,
		PT_FEED,
		TC1,
		TC2,
		TC3,
		NUM_SENSORS
};

class adc_reader {
	private:
		struct adc_info adc_infos[SENSOR::NUM_SENSORS];

	public:
		adc_reader();

		uint16_t read_item(uint8_t sensor_index);

		void add_adc_info(uint8_t sensor_index, RPiGPIOPin cs_pin, uint8_t channel);
};

#endif
