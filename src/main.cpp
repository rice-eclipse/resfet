#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <iostream>
#include <string.h>
#include <thread>
#include <bcm2835.h>

#include "networking/Udp.hpp"
#include "networking/Tcp.hpp"
#include "logger/logger.hpp"
#include "config/config.hpp"
#include "thread/thread.hpp"
#include "visitor/worker_visitor.hpp"
#include "visitor/luna_visitor.hpp"
#include "visitor/titan_visitor.hpp"
#include "init/init.hpp"

// lol
#define LUNA 0
#define TITAN 1

// Defaults for pressure cutoff
#define DEFAULT_PRESSURE_MAX 800
#define DEFAULT_PRESSURE_MIN 300
#define DEFAULT_PRESSURE_SLOPE -0.3
#define DEFAULT_PRESSURE_YINT 1108.1

// Global lock for ignition state
// TODO: move this to a more appropriate place?
std::atomic<bool> ignitionOn;   

// Global lock for extreme low/high pressure, for safety shutoff
std::atomic<bool> pressureShutoff;   

// Simple send test for UDP interface
int main(int argc, char **argv) {
    ConfigMapping config_map;
    bool engine_type;
    char address[16];
    uint32_t port;

    if (argc < 2) {
	// TODO use a logger
	printf("Please provide the path to the config file\n");
	return (1);
    }

    printf("Starting RESFET with config file: %s\n", argv[1]);

    if (config_map.readFrom(argv[1]) != 0) {
	    printf("Error reading config file!\n");
    	    return (1);
    }

#ifndef MOCK
    if (!bcm2835_init()) {
	    std::cerr << "bcm2835_init failed. Are you running as root on RPI?\n";
	    return (1);
    }

    if (initialize_spi() != 0) {
	    std::cerr << "spi init failed. Are you running as root on RPI?\n";
	    return (1);
    }
#endif

    // Mark ignition as off
    ignitionOn.store(false);

    // Mark no pressure shutoff
    pressureShutoff.store(false);

    // Set up the socket
    config_map.getString("Network", "address", address, 16);
    config_map.getInt("Network", "port", &port);
    Logger network_logger("Networking", "NetworkLog", LogLevel::DEBUG);
    Udp::OutSocket sock;
    std::mutex sockMtx;
    try {
        sock.setDest(address, port);  
    } catch (Udp::OpFailureException& ofe) {
        network_logger.error("Could not set destination\n");
        return -1;
    }
  
    // Open the socket up for sending
    try {
        sock.enable();
	network_logger.info("Successfully started UDP server on %s:%d\n", address, port);
    } catch (Udp::OpFailureException& ofe) {
        network_logger.error("Could not open socket\n");
        return -1;
    }

    SENSOR lcs[4] = {
        SENSOR::LC1,
        SENSOR::LC2,
        SENSOR::LC3,
        SENSOR::LC4
    };

    SENSOR pts[4] = {
        SENSOR::PT1,
        SENSOR::PT2,
        SENSOR::PT3,
        SENSOR::PT4,
     };

    SENSOR tcs[3] = {
        SENSOR::TC1,
        SENSOR::TC2,
        SENSOR::TC3
    };
    
    // Retrieve pressure cutoff info from the map
    double pressureMax = DEFAULT_PRESSURE_MAX,
           pressureMin = DEFAULT_PRESSURE_MIN,
           pressureSlope = DEFAULT_PRESSURE_SLOPE,
           pressureYint = DEFAULT_PRESSURE_YINT;
    int readAll = config_map.getDouble("Pressure", "pressure_max", &pressureMax) +
                  config_map.getDouble("Pressure", "pressure_min", &pressureMin) +
                  config_map.getDouble("Pressure", "pressure_slope", &pressureSlope) +
                  config_map.getDouble("Pressure", "pressure_yint", &pressureYint);
    if (readAll != 0) {
        printf("[main] WARNING: failed to read pressure shutoff values, using defaults\n");
    }
    
    // TODO only PT thread needs the shutoff
    PeriodicThread lc_thread("Load Cell Thread", SENSOR_FREQS[SENSOR::LC1], lcs, 4, pressureMax, pressureMin, pressureSlope, pressureYint, &sock);
    PeriodicThread pt_thread("Pressure Transducer Thread", SENSOR_FREQS[SENSOR::PT1], pts, 4, pressureMax, pressureMin, pressureSlope, pressureYint, &sock);
    PeriodicThread tc_thread("Thermocouple Thread", SENSOR_FREQS[SENSOR::TC1], tcs, 3, pressureMax, pressureMin, pressureSlope, pressureYint, &sock);
    lc_thread.start();
    pt_thread.start();
    tc_thread.start();

    Tcp::ListenSocket liSock;
    try {
	    liSock = Tcp::ListenSocket(1234);
	    liSock.listen();
    } catch (Tcp::OpFailureException&) {
	    network_logger.error("Could not create/open listening socket\n");
	    return (-1);
    }

    Tcp::ConnSocket coSock;

    WorkerVisitor *visitor;
    config_map.getBool("Main", "engine_type", &engine_type);
    if (engine_type == LUNA) {
        printf("Starting LUNA visitor\n");
	    visitor = new LunaVisitor(config_map);
#ifndef MOCK
	    initialize_pins();
#endif
    } else {
        printf("Starting TITAN visitor\n");
	    visitor = new TitanVisitor(config_map);
#ifndef MOCK
	    titan_initialize_pins();
#endif
    }

    while (true) {
	    try {
		    coSock = liSock.accept();
	    } catch (Tcp::OpFailureException&) {
		    network_logger.error("Unable to accept a connection\n");
		    return (-1);
	    }

	    network_logger.info("Connected to client: %s\n", coSock.getClientHostname().c_str());

	    uint8_t read;
	    try {
		    while ((read = coSock.recvByte()) != '0') {
			    network_logger.info("Received command: (%d)\n", read);
#ifndef MOCK
			    visitor->visitCommand((COMMAND) read);
#endif
		    }
	    } catch (Tcp::ClientDisconnectException&) {
		    network_logger.info("Client disconnected prematurely\n");
	    } catch (Tcp::OpFailureException&) {
		    network_logger.info("Problem reading, closing connection\n");
	    }

        coSock.close();
    }
    
    liSock.close();
    sock.close();

    return 0;
}
