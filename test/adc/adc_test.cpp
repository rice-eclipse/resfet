/**
 * @file adc_test.cpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Testing code for LTC1876 ADC
 * @version 0.1
 * @date 2019-02-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <cstdio>

#include "adc/adc.hpp"

int main() {
    adc_reader reader;
    uint16_t result;

    // TODO change CS pin for the Pi 4?
    reader.add_adc_info(0, RPI_GPIO_P1_24, 0);

    while (true) {
    	result = reader.read_item(0);
	printf("Received data: %d\n", result);
    }

    return (0);
}
