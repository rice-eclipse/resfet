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

/**
 * @brief Information necessary for communicating with
 * 	  an ADC through SPI.
 */
struct adc_info {
	RPiGPIOPin cs_pin;
	uint8_t channel;

	adc_info() = default;

	adc_info(RPiGPIOPin cs_pin, uint8_t channel)
		: cs_pin(cs_pin)
		, channel(channel)
		{};

};

/**
 * @brief A list of sensors we support.
 *
 * Warning before changing: This enum is used for
 * array indexes.
 */
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
		/** 
		 * @brief An array of adc_infos that the reader uses to
		 *        read from the appropriate ADC.
		 */
		struct adc_info adc_infos[SENSOR::NUM_SENSORS];

	public:
		/**
		 * @brief The constructor for an adc_reader.
		 *
		 * TODO provide configs in constructor?
		 */
		adc_reader();

		/**
		 * @brief Reads the specified sensor.
		 *
		 * @param sensor_index The sensor to read,
		 * 	  based on the SENSOR enum.
		 *
		 * @return The reading from the sensor.
		 */
		uint16_t read_item(uint8_t sensor_index);

		/**
		 * @brief Registers an adc_info in the internal array.
		 * 	  Required for initialization before read_item().
		 *
		 * @param sensor_index The index of the new adc_info,
		 * 	  based on the SENSOR enum.
		 * @param cs_pin The chip select pin used for this sensor.
		 * @param channel The adc channel used for this sensor.
		 */
		void add_adc_info(uint8_t sensor_index, RPiGPIOPin cs_pin, uint8_t channel);
};

#endif
