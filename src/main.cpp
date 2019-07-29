#include <cstdint>
#include <iostream>
#include <string.h>

#include "networking/Tcp.hpp"
#include "logger/logger.hpp"
#include "config/config.hpp"

// Simple config file input example
int main(int argc, char** argv) {
    Logger status_logger("Status", "StatusLog", LogLevel::DEBUG);
    ConfigMapping config;
    char engine[MAX_CONFIG_LENGTH], hey[MAX_CONFIG_LENGTH], foo[MAX_CONFIG_LENGTH], jig[MAX_CONFIG_LENGTH];
    int port;

    if (argc < 2) {
        status_logger.error("Please provide a config filename\n");
        return -1;
    }

    if (config.readFrom(argv[1]) != 0) {
        status_logger.error("File `%s` could not be read\n", argv[1]);
        return -1;
    }
    if (config.getString("", "hey", hey, MAX_CONFIG_LENGTH) != 0) {
        status_logger.error("Error retrieving from null section\n");
        return -1;
    }
    if (config.getInt("NUMBER", "port", &port) != 0) {
	   status_logger.error("Error retrieving port from config\n");
       return -1;
    }
    if (config.getString("CSTRING", "engine", engine, MAX_CONFIG_LENGTH) != 0) {
	   status_logger.error("Error retrieving engine from config\n");
       return -1;
    }
    if (config.getString("BLAH", "foo", foo, MAX_CONFIG_LENGTH) != 0) {
        status_logger.error("Error retrieving foo\n");
        return -1;
    }

    status_logger.info("Value of `hey` is %s", hey);
    status_logger.info("Port from config is %d\n", port);
    status_logger.info("Engine from config is %s\n", engine);
    status_logger.info("Value of `foo` is %s\n", foo);

    return 0;
}
