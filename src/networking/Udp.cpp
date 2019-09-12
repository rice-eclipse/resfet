/**
 * @file Udp.cpp
 * @author Andrew Obler (ajo2@rice.edu)
 * @brief Implentation of classes and methods in Udp.hpp.
 * @version 0.1
 * @date 2019-06-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "networking/Udp.hpp"

Udp::OutSocket::OutSocket() {
    // Indicate an invalid/nonexistent socket
    this->fd = -1;
    this->dest = { AF_INET, INADDR_ANY, 0 }; // dummy "destination"
    this->open = false;
    std::strcpy(this->destHost, "");
    std::strcpy(this->destServ, "");
}

Udp::OutSocket::OutSocket(char* addr, int port) : OutSocket() {
    // In addition to default parameters, set dest as given
    setDest(addr, port);
}

Udp::OutSocket::~OutSocket() { /* default destructor */ }

void Udp::OutSocket::enable() {
    // Try to open an unbound UDP socket
    int sockFd = ::socket(AF_INET, SOCK_DGRAM, 0);

    if (sockFd == -1) {
        // Could not create the socket
        throw OpFailureException();
    }

    // Allow other open connections
    int optTrue = 1;
    if (::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (void*) &optTrue, sizeof(int)) < 0) {
        // Could not configure as needed
        throw OpFailureException();
    }

    // Set members appropriately before return
    this->fd = sockFd;
    this->open = true;
}

void Udp::OutSocket::setDest(char* const addr, int port) {
    // Verify address
    in_addr ia;
    if (inet_aton(addr, &ia) == 0) {
        // addr is not a valid address
        throw OpFailureException();
    }

    // Set dest variable with given info
    this->dest = { AF_INET, htons(port), ia };

    // Get names corresponding to address and port
    int err = getnameinfo((sockaddr*) &dest, sizeof(sockaddr_in), destHost,
                          UDP_DEST_HOST_SIZE, destServ, UDP_DEST_SERV_SIZE, 0);
    
    // If unsuccessful, set both to "Unknown"
    if (err != 0) {
        std::strcpy(destHost, "Unknown");
        std::strcpy(destServ, "Unknown");
    }
}

void Udp::OutSocket::sendByte(uint8_t b) {
    // Delegate to sendBuf()
    sendBuf(&b, 1);
}

void Udp::OutSocket::sendBuf(uint8_t* buf, size_t n) {
    // Error if socket is not open
    if (!open) {
        throw BadOutSocketException();
    }

    size_t numToSend = n;
    ssize_t numSent;
    uint8_t* sendPos = buf;

    // Send until all bytes are sent
    while (numToSend > 0) {
        numSent = ::sendto(fd, (void*) sendPos, numToSend, 0, (sockaddr*) &dest,
                           sizeof(sockaddr_in));

        if (numSent == -1) {
            // Misc error with ::sendto()
            throw OpFailureException();
        }

        numToSend -= numSent;
        sendPos += numSent;
    }
}

void Udp::OutSocket::close() {
    open = false;

    if (fd > -1) {
        if (::close(fd) < 0) {
            // Close failure
            throw OpFailureException();
        }
    }

    fd = -1;
}

int Udp::OutSocket::getFd() {
    return fd;
}

std::string Udp::OutSocket::getDestHost() {
    return std::string(destHost);
}

std::string Udp::OutSocket::getDestServ() {
    return std::string(destServ);
}

bool Udp::OutSocket::isOpen() {
    return open;
}
