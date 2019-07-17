/**
 * @file config.cpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A simple parser for key-value config files.
 * @version 0.1
 * @date 2019-07-17
 * 
 * @copyright Copyright (c) 2019
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config/config.hpp"

CONFIG_TYPE get_type(const char *type_str) {
	if (strcmp("CSTRING", type_str) == 0)
		return CONFIG_TYPE::CSTRING;
	else if (strcmp("NUMBER", type_str) == 0)
		return CONFIG_TYPE::NUMBER;
}
	
uint8_t read_config_file(const char *filename, struct config_pair *array, uint8_t size) {
	FILE *file;
	uint8_t match, index = 0;
	// char line[MAX_LINE_LENGTH];
	char *line = new char[MAX_LINE_LENGTH];
	char *key = new char[MAX_CONFIG_LENGTH], *value = new char[MAX_CONFIG_LENGTH];
	char *type_str = new char[MAX_CONFIG_LENGTH];
	CONFIG_TYPE type;
	size_t len;


	if ((file = fopen(filename, "r")) == NULL) {
		dprintf(STDERR_FILENO, "Open config file %s unsuccessful: %s\n",
				filename, strerror(errno));
		return 1;
	}

	while ((getline(&line, &len, file)) != -1) {
		if (line[0] == '#' || line[0] == '\n')
			continue;
		else if ((match = sscanf(line, "[%s]\n", type_str)) != 0) {
			type = get_type(type_str);
		} else if ((match = sscanf("%s=%s\n", key, value)) != 0) {
			array[index++] = config_pair(key, value, type);
		} else {
			dprintf(STDERR_FILENO, "Error reading config line: %s\n",
					line);
		}
	}

}

uint8_t set_config_var(void *var, const char *name, struct config_pair *array, uint8_t size) {
	uint8_t index = 0;
	struct config_pair elem;

	while(index < size) {
		elem = array[index++];
		if (strcmp(elem.key, name) == 0) {
			if (elem.type == CONFIG_TYPE::CSTRING)
				strncpy((char *)var, elem.cstring, MAX_CONFIG_LENGTH);
			else if (elem.type == CONFIG_TYPE::NUMBER)
				*(uint16_t *)var = elem.number;
			return 0;
		}
	}

	return 1;
}
