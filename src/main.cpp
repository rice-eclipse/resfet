#include <cstdint>
#include <iostream>
#include <string.h>

#include "networking/Udp.hpp"
#include "networking/Tcp.hpp"
#include "logger/logger.hpp"
#include "config/config.hpp"
#include "thread/thread.hpp"
#include "visitor/worker_visitor.hpp"
#include "visitor/luna_visitor.hpp"
#include "visitor/titan_visitor.hpp"

// lol
#define LUNA 0
#define TITAN 1

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

    // Set up the socket
    // TODO use config file to set port, address
    config_map.getString("Network", "address", address, 16);
    config_map.getInt("Network", "port", &port);
    Logger network_logger("Networking", "NetworkLog", LogLevel::DEBUG);
    Udp::OutSocket sock;
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

    /* Set up a thread for reading load cells */
    SENSOR sensors[4] = { SENSOR::LC_MAIN, SENSOR::LC1, SENSOR::LC2, SENSOR::LC3 };
    PeriodicThread per_thread(SENSOR_FREQS[sensors[0]], sensors, 4, &sock);
    per_thread.start();

    Tcp::ListenSocket liSock;
    try {
	    liSock = Tcp::ListenSocket(1234);
	    liSock.listen();
    } catch (Tcp::OpFailureException&) {
	    network_logger.error("Could not create/open listening socket\n");
	    return (-1);
    }

    Tcp::ConnSocket coSock;
    // TODO pick the right visitor

    WorkerVisitor *visitor;
    config_map.getBool("", "engine_type", &engine_type);
    if (engine_type == LUNA)
	    visitor = new LunaVisitor(config_map);
    else
	    visitor = new TitanVisitor(config_map);

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
			    // TODO only need this check because we're not synchronized with dashboard
			    // if (read < COMMAND::NUM_COMMANDS)
				    //network_logger.info("Received command: %s (%d)\n", command_names[read], read);
			    network_logger.info("Received command: (%d)\n", read);
			    visitor->visitCommand((COMMAND)read);
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
