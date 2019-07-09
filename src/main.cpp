#include <cstdint>
#include <iostream>
#include <string.h>

#include "networking/Tcp.hpp"
#include "logger/logger.hpp"

// Simple recv test for TCP interface
int main() {
    Logger network_logger ("Networking", "NetworkLog", LogLevel::DEBUG);

    // Try to open a socket for listening
    Tcp::ListenSocket liSock;
    try {
        liSock = Tcp::ListenSocket(1234);
        liSock.listen();
    } catch (Tcp::OpFailureException&) {
		network_logger.error("Could not create/open listening socket\n");	
        return -1;
    }

    Tcp::ConnSocket coSock;
    while (true) {
        // Try to accept an incoming request
        try {
            coSock = liSock.accept();
        } catch (Tcp::OpFailureException&) {
			network_logger.error("Unable to accept a connection\n");	
            return -1;
        }
		network_logger.info("Connected to client!\n");
		network_logger.info("Hostname: %s\n", coSock.getClientHostname().c_str());
		network_logger.info("Service: %s\n", coSock.getClientService().c_str());
        
        // Read individual bytes until '0', then quit and wait for another request
        uint8_t read;
        try {
            while ((read = coSock.recvByte()) != '0') {
		network_logger.info("Read byte: %c\n", read);
            }
		network_logger.info("Read 0: %c\n", read);
        } catch (Tcp::ClientDisconnectException&) {
		network_logger.info("Client disconnected prematurely, waiting for new connection...\n");
        } catch (Tcp::OpFailureException&) {
		network_logger.info("Problem reading, closing connection\n");
        }

        coSock.close();
    }

    liSock.close();
    
    return 0;
}
