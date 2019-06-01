#include <cstdint>
#include <iostream>

#include "networking/Tcp.hpp"

int main() {
    Tcp::ListenSocket liSock(1234);
    liSock.listen();

    while (true) {
        Tcp::ConnSocket coSock = liSock.accept();
        std::cout << "Connected to client!\n"
                  << "Hostname: " << coSock.getClientHostname() << "\n"
                  << "Service: " << coSock.getClientService() << std::endl;
        uint8_t read;
        while ((read = coSock.recvByte()) != '0') {
            std::cout << "Read byte: " << read << std::endl;
        }
        std::cout << "Read quit byte (0): " << read << std::endl;
        coSock.close();
    }

    liSock.close();
    
    return 0;
}
