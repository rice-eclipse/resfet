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

Logger::Logger(const char *name, const char *filename, LogLevel log_level)
	: name(name)
  	, filename(filename)
	, log_level(log_level)
	, file_fd(-1)
  	{
		create_log_file();
	}

int Logger::create_log_file() {	
	struct stat st;

	if (stat("logs", &st) == -1)
		mkdir("logs", 0700);
	

	if ((file_fd = open(filename, O_CREAT | O_RDWR | O_TRUNC)) == -1) {
		dprintf(STDERR_FILENO, "Create log file unsuccessful: %s\n",
				strerror(errno));
		return -1;
	}

	return 0;
}

void Logger::log(const char *msg, LogLevel level) {
	if (file_fd == -1) {
		dprintf(STDERR_FILENO, "Attempting to log when log file is null\n");
		return;
	}

	if (log_level < level)
		return;

	// TODO time
	dprintf(file_fd, "[%s][%s][%s] %s", name, LogLevelStrings[(int)level], "time", msg);

	dprintf(STDOUT_FILENO, "[%s][%s][%s] %s", name, LogLevelStrings[(int)level], "time", msg);
}

void Logger::error(const char *msg) {
	log(msg, LogLevel::ERROR);
}

void Logger::info(const char *msg) {
	log(msg, LogLevel::INFO);
}

void Logger::debug(const char *msg) {
	log(msg, LogLevel::DEBUG);
}
