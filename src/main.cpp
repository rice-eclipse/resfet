#include <cstdint>
#include <iostream>

#include "networking/Tcp.hpp"
#include "logger/logger.hpp"

// Simple recv test for TCP interface
int main() {

    std::string send_string;
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

		send_string = "Hostname: " + coSock.getClientHostname() + "\n";
		network_logger.info(send_string.c_str());

		send_string = "Service: " + coSock.getClientService() + "\n";
		network_logger.info(send_string.c_str());
        
        // Read individual bytes until '0', then quit and wait for another request
        uint8_t read;
        try {
            while ((read = coSock.recvByte()) != '0') {
		send_string = "Read byte: " + std::string((char *)&read) + "\n";
		network_logger.info(send_string.c_str());
            }
		send_string = "Read 0: " + std::string((char *)&read) + "\n";
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
