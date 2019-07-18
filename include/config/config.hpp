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

/*
 * Format of Config Files
 * 	Headers are enclosed in brackets and are used to separate
 * 	blocks of different types of configs. See CONFIG_TYPE and
 *	get_type.
 * 	Example: [NUMBER]
 *
 * 	Key-value pairs are separated by an equals sign with no
 * 	spaces.
 * 	Example: port=1234
 */

/*
 * TODO we have to check types twice, both when constructing
 * config_pairs and when retrieving the values. Maybe a
 * varargs implementation would be better.
 */

/**
 * @brief Defines the types of configs that we read. Necessary
 * 	  for converting the raw strings into the right values.
 *
 * TODO expand to more complicated types?
 */
enum class CONFIG_TYPE {
	CSTRING,
	NUMBER
};

/**
 * @brief The generic key-value pair format that configuration
 * 	  information is stored in.
 *
 * TODO expand the union if more types are added.
 */
struct config_pair {
	char *key;

	/* A config either defines a number of a string */
	union {
		uint16_t number;
		char cstring[MAX_CONFIG_LENGTH];
	};
	CONFIG_TYPE type;

	config_pair() = default;

	config_pair(char *key, char *value, CONFIG_TYPE type)
		: key(key)
		, type(type)
		{
			if (type == CONFIG_TYPE::NUMBER)
				number = atoi(value);
			else if (type == CONFIG_TYPE::CSTRING)
				strncpy(cstring, value, MAX_CONFIG_LENGTH);
		};
};	

/**
 * @brief Converts a string that is the name of a config type
 * 	  into the corresponding CONFIG_TYPE.
 *
 * @param type_str The string to be converted.
 */
CONFIG_TYPE get_type(const char *type_str);
	
/**
 * @brief Reads a config file and places the configuration
 * 	  information in the provided array.
 *
 * @param filename The name of the config file to read.
 * @param array	   The pre-allocated array that is used to return
 * 		   config_pair information to the caller.
 * @param size	   The size of array.
 *
 * @return 1 on error and 0 otherwise.
 */
uint8_t read_config_file(const char *filename, struct config_pair *array, uint8_t size);

/**
 * @brief Sets a variable based on an array of config_pairs.
 *
 * @param var	   The variable to be configured.
 * @param name	   The name (key) of the variable to be configured
 * @param array	   The array that contains configuration information
 * 		   for this variable.
 * @param size	   The size of array.
 *
 * @return 1 on error and 0 otherwise.
 */
uint8_t set_config_var(void *var, const char *name, struct config_pair *array, uint8_t size);

#endif
