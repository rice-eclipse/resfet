/**
 * @file time.hpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A set of functions for determining and formatting timestamps.
 * @version 0.1
 * @date 2019-07-09
 * 
 * @copyright Copyright (c) 2019
 */

#include <stdint.h>

#define MAX_TIME_BUF_LEN 128

/* A long time in nanoseconds */
typedef uint64_t timestamp_t;

/**
 * @brief Sets the start time if it has not been set already
 */
void set_start_time();

/**
 * @brief Gets time formatted according to ISO 8601 
 */
void get_formatted_time(char *time_buf);

/**
 * @brief Gets the elapsed time in seconds 
 */
timestamp_t get_elapsed_time_s();

/**
 * @brief Gets the elapsed time in milliseconds 
 */
timestamp_t get_elapsed_time_ms();

/**
 * @brief Gets the elapsed time in microseconds 
 */
timestamp_t get_elapsed_time_us();

/**
 * @brief Gets the elapsed time in nanoseconds 
 */
timestamp_t get_elapsed_time_ns();
