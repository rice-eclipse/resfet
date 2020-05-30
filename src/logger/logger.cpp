/**
 * @file logger.cpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Implementation of classes and methods in logger.hpp.
 * @version 0.1
 * @date 2019-07-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h> 
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "logger/logger.hpp"
#include "time/time.hpp"

Logger::Logger(const char *name, const char *fname, LogLevel log_level)
	: name(name)
	, log_level(log_level)
	, file_fd(-1)
  	{
		char time_buf[MAX_TIME_BUF_LEN];

		/* Set start time of RESFET and get the formatted name into time_buf. */
		set_start_time();
		get_formatted_time(time_buf);

		/* Set the name of the file. */
		strcpy(filename, "logs/");
		strcat(filename, time_buf);
		strcat(filename, "/");
		strcat(filename, fname);
		strcat(filename, "_");
		strcat(filename, time_buf);
		strcat(filename, ".log");

		/* Print for debugging. */
		printf("Filename: %s\n", filename);

		/* Create the log file. */
		create_log_file();

		// TODO: write any initial info (e.g. config files) to the log file
	}

int Logger::create_log_file() {	
	struct stat st;

	char time_buf[MAX_TIME_BUF_LEN];

        /* Set start time of RESFET and get the formatted name into time_buf. */
        set_start_time();
	get_formatted_time(time_buf);

	/* Create the logs directory if it does not exist */
	if (stat("logs", &st) == -1) {
		if (mkdir("logs", 0777) == -1)
			dprintf(STDERR_FILENO, "Create logs directory unsuccessful: %s\n",
						strerror(errno));
	}

	/* Create a new sub-directory in logs/ to store logs related to current process. */
	char createdir[MAX_TIME_BUF_LEN + 6] = "logs/";
	strcat(createdir, time_buf);
	
	if (stat(createdir, &st) == -1) {
		if (mkdir(createdir, 0777) == -1)
			dprintf(STDERR_FILENO, "Create logs subdirectory unsuccessful: %s\n",
						strerror(errno));
	}

	/*
	 * Open the file for writing (will delete previous contents).
	 * This assumes there are no shared filenames since they should
	 * contain the time of creation.
	 */
	if ((file_fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0777)) == -1) {
		dprintf(STDERR_FILENO, "Create log file unsuccessful: %s\n",
				strerror(errno));
		return -1;
	}

	return 0;
}

char *Logger::getFilename() {
	return filename;
}

void Logger::log(const char *format, LogLevel level, va_list argList) {
	/* Check the log file exists */
	if (file_fd == -1) {
		dprintf(STDERR_FILENO, "Attempting to log when log file is null\n");
		return;
	}

	/* Only log messages if the priority is high enough */
	if (log_level < level)
		return;

	/* Save the formatted message in the internal buffer */
	vsnprintf(buf, MAX_BUF_LEN, format, argList);

	/* Null-terminate the buffer, for safety */
	buf[MAX_BUF_LEN - 1] = '\0';

	/* Write the formatted message, and other information, to the log file */	
	dprintf(file_fd, "[%s][%s][%lu] %s", name, LogLevelStrings[level], get_elapsed_time_ms(), buf);
	
	/* Write to stdout */
	dprintf(STDOUT_FILENO, "[%s][%s][%lu] %s", name, LogLevelStrings[level], get_elapsed_time_ms(), buf);
}

void Logger::error(const char *format, ...) {
	/* Pass in the format string and variable args */
	va_list argList;
	va_start(argList, format);
	log(format, LogLevel::ERROR, argList);
	va_end(argList);
}

void Logger::info(const char *format, ...) {
	/* Pass in the format string and variable args */
	va_list argList;
	va_start(argList, format);
	log(format, LogLevel::INFO, argList);
	va_end(argList);
}

void Logger::debug(const char *format, ...) {
	/* Pass in the format string and variable args */
	va_list argList;
	va_start(argList, format);
	log(format, LogLevel::DEBUG, argList);
	va_end(argList);
}

void Logger::verbatim(const char *format, ...) {
	/* Check the log file exists */
	if (file_fd == -1) {
		dprintf(STDERR_FILENO, "Attempting to log when log file is null\n");
		return;
	}

	/* Pass in the format string and variable args */
	va_list argList;
	va_start(argList, format);

	/* Save the formatted message in the internal buffer */
	vsnprintf(buf, MAX_BUF_LEN, format, argList);

	va_end(argList);

	/* Null-terminate the buffer, for safety */
	buf[MAX_BUF_LEN - 1] = '\0';

	/* Write the formatted message, and other information, to the log file */	
	dprintf(file_fd, "%s", buf);
	
	/* Write to stdout */
	dprintf(STDOUT_FILENO, "%s", buf);
}

void Logger::data(uint8_t *data, size_t size) {
	/* Check the log file exists */
	if (file_fd == -1) {
		dprintf(STDERR_FILENO, "Attempting to log when log file is null\n");
		return;
	}

	write(file_fd, data, size);
}
