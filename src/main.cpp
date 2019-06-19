#include <cstdint>
#include <iostream>

#include "networking/Tcp.hpp"

// Simple recv test for TCP interface
int main() {
    // Try to open a socket for listening
    Tcp::ListenSocket liSock;
    try {
        liSock = Tcp::ListenSocket(1234);
        liSock.listen();
    } catch (Tcp::OpFailureException&) {
        std::cerr << "Could not create/open listening socket" << std::endl;
        return -1;
    }

    Tcp::ConnSocket coSock;
    while (true) {
        // Try to accept an incoming request
        try {
            coSock = liSock.accept();
        } catch (Tcp::OpFailureException&) {
            std::cerr << "Unable to accept a connection" << std::endl;
            return -1;
        }
        std::cout << "Connected to client!\n"
                  << "Hostname: " << coSock.getClientHostname() << "\n"
                  << "Service: " << coSock.getClientService() << std::endl;
        
        // Read individual bytes until '0', then quit and wait for another request
        uint8_t read;
        try {
            while ((read = coSock.recvByte()) != '0') {
                std::cout << "Read byte: " << read << std::endl;
            }
            std::cout << "Read 0: " << read << std::endl;
        } catch (Tcp::ClientDisconnectException&) {
            std::cerr << "Client disconnected prematurely, waiting for new connection..."
                      << std::endl;
        } catch (Tcp::OpFailureException&) {
            std::cerr << "Problem reading, closing connection" << std::endl;
        }

        coSock.close();
    }

    liSock.close();
    
    return 0;
}
