/**
 * @file config.hpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A simple parser for key-value config files.
 * @version 0.1
 * @date 2019-07-17
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef __CONFIG_HPP
#define __CONFIG_HPP

#include <stdint.h>

#define MAX_LINE_LENGTH 128
#define MAX_CONFIG_LENGTH 32

enum class CONFIG_TYPE {
	CSTRING,
	NUMBER
};

struct config_pair {
	char *key;
	union {
		uint16_t number;
		char cstring[MAX_CONFIG_LENGTH];
	};
	CONFIG_TYPE type;

	config_pair() = default;

	config_pair(char *key, char *value, CONFIG_TYPE type)
		: key(key)
		// , value(value)
		, type(type)
		{
			if (type == CONFIG_TYPE::NUMBER)
				number = atoi(value);
			else if (type == CONFIG_TYPE::CSTRING)
				strncpy(cstring, value, MAX_CONFIG_LENGTH);
		};
};	

CONFIG_TYPE get_type(const char *type_str);
	
uint8_t read_config_file(const char *filename, struct config_pair *array, uint8_t size);

uint8_t set_config_var(void *var, const char *name, struct config_pair *array, uint8_t size);


#endif
