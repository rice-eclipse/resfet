/**
 * @file rpi_pins.hpp
 * @author Andrew Obler (andrewobler@gmail.com)
 * @brief Pin assignments for the Raspberry Pi.
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __RPI_PINS_HPP
#define __RPI_PINS_HPP

#include <bcm2835.h>

// Definitions for 3 ADCs
#define ADC_0_CS RPI_V2_GPIO_P1_26
#define ADC_1_CS RPI_V2_GPIO_P1_24
#define ADC_2_CS RPI_V2_GPIO_P1_22
#define LC_ADC ADC_2_CS
#define PT_ADC ADC_1_CS
#define TC_ADC ADC_1_CS // ARCA: ADC_0_CS, MK11: ADC_1_CS

// Definitions for 6 Drivers
#define DRIVER1 RPI_V2_GPIO_P1_11 // ARCA: RPI_V2_GPIO_P1_11, MK11: RPI_V2_GPIO_P1_11
#define DRIVER2 RPI_V2_GPIO_P1_13 // ARCA: RPI_V2_GPIO_P1_13, MK11: RPI_V2_GPIO_P1_13
#define DRIVER3 RPI_V2_GPIO_P1_15 // ARCA: RPI_V2_GPIO_P1_15, MK11: RPI_V2_GPIO_P1_15
#define DRIVER4 RPI_V2_GPIO_P1_18 // ARCA: RPI_V2_GPIO_P1_18, MK11: RPI_V2_GPIO_P1_18
#define DRIVER5 RPI_V2_GPIO_P1_33 // ARCA: RPI_V2_GPIO_P1_33, MK11: N/A
#define DRIVER6 RPI_V2_GPIO_P1_16 // ARCA: RPI_V2_GPIO_P1_33, MK11: RPI_V2_GPIO_P1_16

// defines for Luna
#define MAIN_VALVE DRIVER1
#define PRESSURE_VALVE DRIVER2
#define IGN_START DRIVER6

// defines for Titan
#define MAIN_FEED_VALVE DRIVER1
#define OXI_VALVE DRIVER2
#define GROUND_VENT_VALVE DRIVER3
#define IGN_START DRIVER6

#endif // __RPI_PINS_HPP
