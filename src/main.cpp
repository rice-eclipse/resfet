#include <cstdint>
#include <iostream>
#include <string.h>

#include "commands/tcp_commands.hpp"
#include "networking/Udp.hpp"
#include "networking/Tcp.hpp"
#include "logger/logger.hpp"
#include "config/config.hpp"
#include "thread/thread.hpp"
#include "visitor/worker_visitor.hpp"
#include "visitor/luna_visitor.hpp"
#include "visitor/titan_visitor.hpp"

// Simple send test for UDP interface
int main() {
    Logger network_logger("Networking", "NetworkLog", LogLevel::DEBUG);

    // Set up the socket
    Udp::OutSocket sock;
    try {
        sock.setDest("127.0.0.1", 1234);
    } catch (Udp::OpFailureException& ofe) {
        network_logger.error("Could not set destination\n");
        return -1;
    }
  
    // Open the socket up for sending
    try {
        sock.enable();
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
    worker_visitor visitor;


    while (1) {
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
			    if (read < COMMAND::NUM_COMMANDS)
				    network_logger.info("Received command: %s (%d)\n", command_names[read], read);
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
