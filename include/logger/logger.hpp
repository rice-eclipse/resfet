/**
 * @file logger.hpp
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Logger with debug levels.
 * @version 0.1
 * @date 2019-07-08
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef __LOGGER_HPP
#define __LOGGER_HPP

// Maximum length of a formatted log message
#define MAX_BUF_LEN 256

#include <stdarg.h>

/**
 * @brief The log levels that determine what messages are send to
 * 		  stdout. TODO Should we use the same logger class to log
 * 		  data? Data should be logged at all log levels.
 *
 * In order of least to most output:
 * SILENT: No messages.
 * ERROR: Errors that prevent continued operation.
 * INFO: Errors and important events.
 * DEBUG: INFO plus additional debug messages.
 */
enum LogLevel {
	SILENT = 0,
	ERROR = 1,
	INFO = 2,
	DEBUG = 3
};

const char LogLevelStrings[][7] ={
	"SILENT",
	"ERROR",
	"INFO",
	"DEBUG"
};

/**
 * @brief A logger class that handles different log levels and logging 
 * 		  options.
 * 
 * TODO each instance has only one filename and file descriptor.
 * If we want to use fewer loggers we can provide the file in the
 * function calls instead of the constructor.
 */
class Logger {
	private:
		/**
		 * @brief The name of this logger
		 */
		const char *name;

		/**
		 * @brief The filename of the log file to write to
		 */
		char filename[MAX_BUF_LEN];

		/** 
		 * @brief The file descriptor to write to
		 */
		int file_fd;

		/**
		 * @brief The log level of this logger
		 */
		LogLevel log_level;

		/**
		 * @brief A temporary buffer that is used to format user input
		 */
		char buf[MAX_BUF_LEN];

		/*
		 * @brief Attempts to create a log file and any intermediate directories.
		 * 	  	  The default directory is logs/filename, where the logs/ directory
		 * 	  	  is relative to the current working directory.
		 * 
		 * @return 1 on success and 0 on failure.
		 */
		int create_log_file();

	public:
		/*
		 * @brief Constructor for the logger.
		 */
		Logger(const char *name, const char *filename, LogLevel log_level);

		/*
		 * @brief Logs a message if the logger's log level is at least as high
		 * 	  	  as the provided log level. 
		 * @msg	  The message to log.
		 * @param level The log level of the message.
		 */
		void log(const char *format, LogLevel level, va_list argList);

		/*
		 * @brief Logs an ERROR level message.
		 * @msg	  The message to log.
		 */
		void error(const char *format, ...);

		/*
		 * @brief Logs an INFO level message.
		 * @msg	  The message to log.
		 */
		void info(const char *format, ...);

		/*
		 * @brief Logs a DEBUG level message.
		 * @msg	  The message to log.
		 */
		void debug(const char *format, ...);
};

#endif
