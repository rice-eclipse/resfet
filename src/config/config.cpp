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
#include <limits>
#include <string>
#include <unordered_map>

#include "config/config.hpp"

// Build format strings from buffer limits defined in config.hpp
static std::string header_format_str = "[%"
								   	 + std::to_string(MAX_CONFIG_LENGTH)
								   	 + "[^]]]";
static std::string key_value_format_str = "%"
									  	+ std::to_string(MAX_CONFIG_LENGTH)
									  	+ "[^=]=%"
									  	+ std::to_string(MAX_CONFIG_LENGTH)
									  	+ "s";

static const char* header_format = header_format_str.c_str();
static const char* key_value_format = key_value_format_str.c_str();

ConfigMapping::ConfigMapping() {
	// Create blank private mapping
	map = std::unordered_map<std::string,
							 std::unordered_map<std::string, std::vector<std::string>>>();
}

ConfigMapping::~ConfigMapping() { /* default destructor */ }

uint8_t ConfigMapping::readFrom(const char* filename) {
	std::fstream file;
	char line[MAX_LINE_LENGTH];
	char header_str[MAX_CONFIG_LENGTH], key[MAX_CONFIG_LENGTH], value[MAX_CONFIG_LENGTH];
	char* current_section = (char*) "";
	size_t len, line_count = 0;

	// Try to open the given file
	file.open(filename, std::fstream::in);
	if (!file) {
		#ifdef __EXTRA_DEBUG_LOG
			std::cerr << "Unable to open config file " << filename << std::endl;
		#endif
		map.clear();
		return 1;
	}

	// Clear out mapping
	map.clear();

	while (!file.eof()) {
		file.getline(line, MAX_LINE_LENGTH);
		line_count++;

		// Skip commented and empty lines
		if (line[0] == '#' || line[0] == ';' || line[0] == '\r'
						   || line[0] == '\n' || line[0] == '\0')
			continue;

		// Encountered a line that is too long or otherwise failed to read.
		if (file.fail()) {
			#ifdef __EXTRA_DEBUG_LOG
				std::cerr << "Line " << line_count
						<< " is too long or failed to be read. Map may be incomplete"
						<< std::endl;
			#endif
			file.close();
			map.clear();
			return 1;
		}

		// Attempt to match headers (that define sections)
		if (sscanf(line, header_format, header_str) == 1) {
			current_section = header_str; // mark current section
		// Attempt to match key-value pairs      
		} else if (sscanf(line, key_value_format, key, value) == 2) {
			if (!isPresent(current_section, key))
				map[current_section][key] = std::vector<std::string>();
			map[current_section][key].push_back(value);
		// No match found, line is malformed; skip it
		} else {
			#ifdef __EXTRA_DEBUG_LOG
				std::cerr << "Malformed config line " << line_count
						<< ": `" << line << "`, line is skipped" << std::endl;
			#endif
			file.close();
			map.clear();
			return 1;
		}
	}

	file.close();

	return 0;
}

uint8_t ConfigMapping::writeTo(const char* filename) {
	// TODO
	return 1;
}

bool ConfigMapping::isPresent(const char* section, const char* key) {
	if (section == NULL || key == NULL) {
		return false;
	}

	if (map.find(section) == map.end()) {
		return false;
	}
	if (map[section].find(key) == map[section].end()) {
		return false;
	}
	return true;
}

uint8_t ConfigMapping::getString(const char* section, const char* key, char* dest, size_t n) {
	// Make sure the provided key is present
	if (!isPresent(section, key)) {
		#ifdef __EXTRA_DEBUG_LOG
			std::cerr << "Key `" << key << "` not found in section `" << section
					<< "`" << std::endl;
		#endif
		return 1;
	}
	std::strncpy(dest, map[section][key][0].c_str(), n);
	return 0;
}

uint8_t ConfigMapping::getInt(const char* section, const char* key, uint32_t* dest) {
	// Make sure the provided key is present
	if (!isPresent(section, key)) {
		#ifdef __EXTRA_DEBUG_LOG
			std::cerr << "Key `" << key << "` not found in section `" << section
					<< "`" << std::endl;
		#endif
		return 1;
	}
	*dest = atoi(map[section][key][0].c_str());
	return 0;
}

uint8_t ConfigMapping::getDouble(const char* section, const char* key, double* dest) {
        // Make sure the provided key is present
        if (!isPresent(section, key)) {
		#ifdef __EXTRA_DEBUG_LOG
			std::cerr << "Key `" << key << "` not found in section `" << section
					<< "`" << std::endl;
		#endif
		return 1;
	}
        *dest = atof(map[section][key][0].c_str());
        return 0;
}

uint8_t ConfigMapping::getBool(const char* section, const char* key, bool* dest) {
	uint32_t temp;
	getInt(section, key, &temp);
	*dest = (bool)temp;
	return 0;
}

uint8_t ConfigMapping::getVector(const char* section, const char* key, std::vector<uint32_t>* dest) {
	uint32_t index = 0;
	std::vector<std::string> v;

	// Make sure the provided key is present
	if (!isPresent(section, key)) {
		#ifdef __EXTRA_DEBUG_LOG
			std::cerr << "Key `" << key << "` not found in section `" << section
					<< "`" << std::endl;
		#endif
		return 1;
	}

	v = map[section][key];

	for (; index < v.size(); index++)
		dest->push_back(atoi(v[index].c_str()));
	return 0;
}
