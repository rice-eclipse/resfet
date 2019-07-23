/**
 * @file config.cpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A simple parser for key-value config files.
 * @version 0.1
 * @date 2019-07-17
 * 
 * @copyright Copyright (c) 2019
 */

#include <fstream>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>

#include "config/config.hpp"

ConfigMapping::ConfigMapping() {
	// Create blank private mapping
	map = std::unordered_map<std::string,
							 std::unordered_map<std::string, std::string>>();
}

ConfigMapping::~ConfigMapping() { /* default destructor */ }

uint8_t ConfigMapping::readFrom(char* filename) {
	std::fstream file;
	char line[MAX_LINE_LENGTH];
	char header_str[MAX_CONFIG_LENGTH], key[MAX_CONFIG_LENGTH], value[MAX_CONFIG_LENGTH];
	char* current_section = (char*) "";
	size_t len, line_count = 0;

	// Try to open the given file
	file.open(filename, std::fstream::in);
	if (!file) {
		std::cerr << "Unable to open config file " << filename << std::endl;
		return 1;
	}

	// Clear out mapping
	map = std::unordered_map<std::string,
							 std::unordered_map<std::string, std::string>>();

	while (!file.eof()) {
		file.getline(line, MAX_LINE_LENGTH);
		line_count++;

		// Skip commented and empty lines
		if (line[0] == '#' || line[0] == '\r' || line[0] == '\n' || line[0] == '\0')
			continue;

		// ----- TODO: sketchy stuff below! -----
		// Using sscanf() in this fashion can result in a buffer overflow
		// if the given string is longer than MAX_CONFIG_LENGTH. We may want to
		// look into alternative parsing functions.
		// ----- end sketchy stuff -----
		
		// Attempt to match headers (that define sections)
		if (sscanf(line, "[%[^]]]", header_str) == 1) {
			current_section = header_str; // mark current section
		// Attempt to match key-value pairs      
		} else if (sscanf(line, "%[^=]=%s", key, value) == 2) {
			map[current_section][key] = value;
		// Attempt to match an undefined key
		} else if (sscanf(line, "%[^=]=", key) == 1) {
			map[current_section][key] = "";
		// No match found, line is malformed
		} else {
			std::cerr << "Error reading config line " << line_count
					  << ": `" << line << "`, line is skipped" << std::endl;
		}
	}

	file.close();

	return 0;
}

uint8_t ConfigMapping::writeTo(char* filename) {
	// TODO
	return 1;
}

bool ConfigMapping::isPresent(char* section, char* key) {
	if (map.find(section) == map.end()) {
		return false;
	}
	if (map[section].find(key) == map[section].end()) {
		return false;
	}
	return true;
}

uint8_t ConfigMapping::getString(char* section, char* key, char* dest, size_t n) {
	// Make sure the provided key is present
	if (!isPresent(section, key)) {
		std::cerr << "Key `" << key << "` not found in section `" << section
				  << "`" << std::endl;
		return 1;
	}
	std::strncpy(dest, map[section][key].c_str(), n);
	return 0;
}

uint8_t ConfigMapping::getInt(char* section, char* key, int* dest) {
	// Make sure the provided key is present
	if (!isPresent(section, key)) {
		std::cerr << "Key `" << key << "` not found in section `" << section
				  << "`" << std::endl;
		return 1;
	}
	*dest = atoi(map[section][key].c_str());
	return 0;
}
