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

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

// The maximum length (in characters) for a line from a config file.
#define MAX_LINE_LENGTH 128

// The maximum length for a key, value, or header from a config file.
#define MAX_CONFIG_LENGTH 64

/*
 * Format of Config Files
 * 	Headers are enclosed in brackets and are used to separate
 * 	sections of a config file.
 * 	Example: [NUMBER]
 *
 * 	Key-value pairs are separated by an equals sign with no
 * 	spaces.
 * 	Example: port=1234
 * 
 *  Key-value pairs defined before the first header reside in the "null"
 *  section, whose name is the empty string "". This section can be
 *  custom-overwritten with the header [] later in the file.
 * 
 *  Lines beginning with '#' are ignored in parsing and can be used as
 *  comments.
 */

/**
 * @brief Represents the data of an entire config file.
 */
class ConfigMapping {
	private:
		/**
		 * @brief The central map structure.
		 * 
		 * The first layer of the map is the sections of the file, and the
		 * second layer is the values of keys in that section. For example, for
		 * a file that has
 		 * 
 		 * ...
 		 * [FOO]
 		 * bar=baz
 		 * ...
 		 * 
 		 * the map would have map["FOO"]["bar"] = "baz".
		 */
		std::unordered_map<std::string,
						   std::unordered_map<std::string, std::vector<std::string>>> map;
	
	public:
		/**
		 * @brief Create a blank ConfigMapping with an empty map. Call
		 * 		  readFrom() to populate the map with a file's contents.
		 */
		ConfigMapping();

		/**
		 * @brief Destroy this ConfigMapping. No special behavior. 
		 */
		~ConfigMapping();

		/**
		 * @brief Read the contents of a config file into this map.
		 * 
		 * @param filename the path of the file from which to read
		 * @return 1 on error, 0 otherwise
		 */
		uint8_t readFrom(const char* filename);

		/**
		 * @brief Write the contents of this map out as an INI file.
		 * 
		 * (still TODO!)
		 * 
		 * @param filename the path of the file to which to write
		 * @return 1 on error, 0 otherwise
		 */
		uint8_t writeTo(const char* filename);

		/**
		 * @brief Tell whether a key is present in this map.
		 * 
		 * @param section the section where the key should be located
		 * @param key 	  the key to search for
		 * @return true if the key is found in the specified section, false
		 * 		   otherwise 
		 */
		bool isPresent(const char* section, const char* key);

		/**
		 * @brief Get a value from the map as a string.
		 * 
		 * @param section the section from which to get the value
		 * @param key 	  the key corresponding to the desired value
		 * @param dest 	  a buffer into which to store the value
		 * @param n 	  the maximum number of characters to extract from the
		 * 				  value (should be the size of the dest buffer)
		 * @return 1 on error (i.e. if the key is not found), 0 otherwise
		 */
		uint8_t getString(const char* section, const char* key, char* dest, size_t n);

		/**
		 * @brief Get a value from the map as an integer.
		 * 
		 * @param section the section from which to get the value
		 * @param key 	  the key corresponding to the desired value
		 * @param dest 	  an int buffer into which to store the value
		 * @return 1 on error (i.e. if the key is not found), 0 otherwise
		 */
		uint8_t getInt(const char* section, const char* key, uint32_t* dest);

		/**
		 * @brief Get a value from the map as a boolean.
		 * 
		 * @param section the section from which to get the value
		 * @param key 	  the key corresponding to the desired value
		 * @param dest 	  a boolean address into which to store the value
		 * @return 1 on error (i.e. if the key is not found), 0 otherwise
		 */
		uint8_t getBool(const char* section, const char* key, bool* dest);

		/**
		 * @brief Get a vector of numbers from the map.
		 * 
		 * @param section the section from which to get the value
		 * @param key 	  the key corresponding to the desired value
		 * @param dest 	  a vector address into which to store the value
		 * @return 1 on error (i.e. if the key is not found), 0 otherwise
		 */
		uint8_t getVector(const char* section, const char* key, std::vector<uint32_t>* dest);
};

#endif
