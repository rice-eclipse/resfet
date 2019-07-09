/**
 * @file time.c
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief A set of functions for determining and formatting timestamps.
 * @version 0.1
 * @date 2019-07-09
 * 
 * @copyright Copyright (c) 2019
 */

#include <stdio.h>
#include <time.h>

#include "time/time.hpp"

static timestamp_t start_time = 0;

void set_start_time() {
	if (start_time == 0) {
		start_time = get_elapsed_time_ns();
	}
}

void get_formatted_time(char *time_buf) {
	struct tm date;
	struct timespec tp;

	clock_gettime(CLOCK_REALTIME, &tp);

	localtime_r((const time_t *)&tp.tv_sec, &date);

	// TODO daylight savings?
	snprintf(time_buf, MAX_TIME_BUF_LEN, "%04d-%02d-%02dT%02d:%02d:%02d",
		 date.tm_year + 1900, date.tm_mon + 1, date.tm_mday,
		 date.tm_hour, date.tm_min, date.tm_sec);
}

timestamp_t get_elapsed_time_s() {
	return get_elapsed_time_ms() / 1000;
}

timestamp_t get_elapsed_time_ms() {
	return get_elapsed_time_us() / 1000;
}

timestamp_t get_elapsed_time_us() {
	return get_elapsed_time_ns() / 1000;
}

timestamp_t get_elapsed_time_ns() {
	struct timespec tp;

	clock_gettime(CLOCK_REALTIME, &tp);
	return (uint64_t)tp.tv_sec * 1000000000 + (uint64_t)tp.tv_nsec - start_time;
}
