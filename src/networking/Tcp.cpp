/**
 * @file Tcp.cpp
 * @author Andrew Obler (ajo2@rice.edu)
 * @brief Implementation of classes and methods in Tcp.hpp.
 * @version 0.1
 * @date 2019-05-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <arpa/inet.h> // for socket interface functions
#include <cstdint> // for uint8_t
#include <cstring> // for memset()
#include <netdb.h> // for socket interface functions
#include <string>
#include <sys/types.h>
#include <sys/socket.h> // for socket interface functions
#include <unistd.h> // for functions on file descriptors

#include "networking/Tcp.hpp"

// ListenSocket methods

Tcp::ListenSocket::ListenSocket(int port) {
    // Set members
    this->port = port;
    this->listening = false;

    // Open a blank TCP socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        // Socket could not be created
        // TODO: throw SocketCreateFailureException
    }

    this->fd = fd;

    // Configure the socket to allow other open connections on this device
    // (allow the local address to be reused)
    int optTrue = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*) &optTrue, sizeof(int)) < 0) {
        // Socket could not be configured as needed
        // TODO: throw SocketCreateFailureException
    }

    // Set up a sockaddr_in struct to carry the necessary info for bind()
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // clear memory for serverAddr
    serverAddr.sin_family = AF_INET; // protocol family
    serverAddr.sin_port = htons(port); // port number
    serverAddr.sin_addr.s_addr = INADDR_ANY; // local IP address

    // Bind the socket to the specified port
    if (bind(fd, (sockaddr*) &serverAddr, sizeof(sockaddr_in)) < 0) {
        // Failed to bind the socket
        // TODO: throw SocketCreateFailureException
    }
}

Tcp::ListenSocket::~ListenSocket() {
    // Close the socket before destruction to avoid file leaks
    close();
}

void Tcp::ListenSocket::listen() {
    // Attempt a listen
    if (::listen(fd, TCP_BACKLOG) < 0) {
        // TODO: throw ListenFailureException
    }

    listening = true;
}

Tcp::ConnSocket Tcp::ListenSocket::accept() {
    if (!listening) {
        // TODO: throw?
    }
    
    sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(sockaddr_in);

    // Wait for a connection request on fd and accept it
    int connFd = ::accept(fd, (sockaddr*) &clientAddr, &clientSize);

    if (connFd < 0) {
        // TODO: throw AcceptFailureException
    }

    Tcp::ConnSocket coSock;
    coSock.fd = connFd;
    coSock.valid = true;

    // Retrieve info about the client
    int err = getnameinfo((sockaddr*) &clientAddr, sizeof(sockaddr_in), coSock.clientHost,
                          CLIENT_HOST_SIZE, coSock.clientServ, CLIENT_SERV_SIZE, 0);

    // In case of error, set both hostname and service to "Unknown"
    if (err != 0) {
        // TODO: throw?
        std::strcpy(coSock.clientHost, "Unknown");
        std::strcpy(coSock.clientServ, "Unknown");
    }

    return coSock;
}

void Tcp::ListenSocket::close() {
    listening = false;

    if (::close(fd) < 0) {
        // TODO: throw CloseFailureException
    }
}

int Tcp::ListenSocket::getPort() {
    return port;
}

int Tcp::ListenSocket::getFd() {
    return fd;
}

bool Tcp::ListenSocket::isListening() {
    return listening;
}

// ConnSocket methods

Tcp::ConnSocket::ConnSocket() {
    valid = false;

    // Set hostname and service to empty strings
    std::strcpy(clientHost, "");
    std::strcpy(clientServ, "");
}

Tcp::ConnSocket::~ConnSocket() { /* default destructor */ }

uint8_t Tcp::ConnSocket::recvByte() {
    // Ensure the socket is ready for a receive
    if (!valid) {
        // TODO: throw InvalidSocketException
    }

    uint8_t received;

    // Try to read a byte into received
    ssize_t numRead = ::recv(fd, (void*) &received, sizeof(uint8_t), 0);
    
    if (numRead == 0) {
        // Client has closed the connection
        // TODO: throw PrematureCloseException
    } else if (numRead == -1) {
        // Misc error with ::recv()
        // TODO: throw RecvFailureException
    }

    return received;
}

void Tcp::ConnSocket::recvBuf(uint8_t* buf, size_t n) {
    // Ensure the socket is ready for a receive
    if (!valid) {
        // TODO: throw InvalidSocketException
    }

    size_t numToRead = n;
    ssize_t numRead;
    uint8_t* readPos = buf;

    // Recv until all bytes are read
    while (numToRead > 0) {
        numRead = ::recv(fd, (void*) readPos, numToRead, 0);
        
        if (numRead == 0) {
            // Client has closed the connection
            // TODO: throw PrematureCloseException
        } else if (numRead == -1) {
            // Misc error with ::recv()
            // TODO: throw RecvFailureException
        }

        numToRead -= numRead;
        readPos += numRead;
    }
}

void Tcp::ConnSocket::sendByte(uint8_t b) { /* TODO */ }

void Tcp::ConnSocket::sendBuf(uint8_t* buf, size_t n) { /* TODO */ }

void Tcp::ConnSocket::close() {
    valid = false;

    if (::close(fd) < 0) {
        // TODO: throw CloseFailureException
    }
}

int Tcp::ConnSocket::getFd() {
    return fd;
}

std::string Tcp::ConnSocket::getClientHostname() {
    return std::string(clientHost);
}

std::string Tcp::ConnSocket::getClientService() {
    return std::string(clientServ);
}

bool Tcp::ConnSocket::isValid() {
    return valid;
}
