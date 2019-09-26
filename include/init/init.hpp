/**
 * @file init.hpp
 * @author RJ Cunningham (rjc6@rice.edu)
 * @brief Initialization functions for Raspbery Pi GPIOs
 * 	  to be sent.
 * @version 0.1
 * @date 2017-11-29
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef SOFTWARE_INITIALIZATION_HPP
#define SOFTWARE_INITIALIZATION_HPP

/**
 * @brief Sets all pins to the default state.
 */
void initialize_pins();

/**
 * @brief Sets all pins to the default state for Titan.
 */
void titan_initialize_pins();

/**
 * @brief Initializes the SPI and ADC modules.
 */
int initialize_spi();

#endif //SOFTWARE_INITIALIZATION_HPP
