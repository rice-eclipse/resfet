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

Tcp::ListenSocket::ListenSocket() {
    this->fd = -1;
    this->port = 0;
    this->listening = false;
}

Tcp::ListenSocket::ListenSocket(int port) : ListenSocket() {
    // Member defaults set by default ctor
    this->port = port;

    // Open a blank TCP socket
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockFd < 0) {
        // Socket could not be created
        throw OpFailureException();
    }

    this->fd = sockFd;

    // Configure the socket to allow other open connections on this device
    // (allow the local address to be reused)
    int optTrue = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*) &optTrue, sizeof(int)) < 0) {
        // Socket could not be configured as needed
        throw OpFailureException();
    }

    // Set up a sockaddr_in struct to carry the necessary info for bind()
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // clear memory for serverAddr
    serverAddr.sin_family = AF_INET; // protocol family
    serverAddr.sin_port = htons(port); // port number
    serverAddr.sin_addr.s_addr = INADDR_ANY; // local IP address

    // Bind the socket to the specified port
    if (bind(sockFd, (sockaddr*) &serverAddr, sizeof(sockaddr_in)) < 0) {
        // Failed to bind the socket, can't listen
        throw OpFailureException();
    }
}

Tcp::ListenSocket::~ListenSocket() { /* default destructor */ }

void Tcp::ListenSocket::listen() {
    // Attempt a listen
    if (::listen(fd, TCP_BACKLOG) < 0) {
        // Listen failure
        throw OpFailureException();
    }

    listening = true;
}

Tcp::ConnSocket Tcp::ListenSocket::accept() {
    if (!listening) {
        // Cannot accept on a socket that is not listening!
        throw BadSocketException();
    }
    
    sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(sockaddr_in);

    // Wait for a connection request on fd and accept it
    int connFd = ::accept(fd, (sockaddr*) &clientAddr, &clientSize);

    if (connFd < 0) {
        // Accept failure (resistance is futile)
        throw OpFailureException();
    }

    Tcp::ConnSocket coSock;
    coSock.fd = connFd;
    coSock.open = true;

    // Retrieve info about the client
    int err = getnameinfo((sockaddr*) &clientAddr, sizeof(sockaddr_in), coSock.clientHost,
                          CLIENT_HOST_SIZE, coSock.clientServ, CLIENT_SERV_SIZE, 0);

    // In case of error, set both hostname and service to "Unknown"
    if (err != 0) {
        std::strcpy(coSock.clientHost, "Unknown");
        std::strcpy(coSock.clientServ, "Unknown");
    }

    return coSock;
}

void Tcp::ListenSocket::close() {
    listening = false;

    if (fd > -1) {
        if (::close(fd) < 0) {
            // Close failure
            throw OpFailureException();
        }
    }

    fd = -1; // indicate the FD has been closed
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
    fd = -1;
    open = false;

    // Set hostname and service to empty strings
    std::strcpy(clientHost, "");
    std::strcpy(clientServ, "");
}

Tcp::ConnSocket::~ConnSocket() { /* default destructor */ }

uint8_t Tcp::ConnSocket::recvByte() {
    // Delegate to recvBuf()
    uint8_t b; // the "buffer"
    recvBuf(&b, 1);
    return b;
}

void Tcp::ConnSocket::recvBuf(uint8_t* buf, size_t n) {
    // Ensure the socket is ready for a receive
    if (!open) {
        // Cannot receive on a socket that is not open!
        throw BadSocketException();
    }

    size_t numToRead = n;
    ssize_t numRead;
    uint8_t* readPos = buf;

    // Recv until all bytes are read
    while (numToRead > 0) {
        numRead = ::recv(fd, (void*) readPos, numToRead, 0);
        
        if (numRead == 0) {
            // Client has closed the connection
            open = false;
            throw ClientDisconnectException();
        } else if (numRead == -1) {
            // Misc error with ::recv()
            throw OpFailureException();
        }

        numToRead -= numRead;
        readPos += numRead;
    }
}

void Tcp::ConnSocket::sendByte(uint8_t b) { 
    // Delegate to sendBuf()
    sendBuf(&b, 1);
}

void Tcp::ConnSocket::sendBuf(uint8_t* buf, size_t n) {
    // Ensure the socket is ready for a send
    if (!open) {
        // Cannot send on a socket that is not open!
        throw BadSocketException();
    }

    size_t numToSend = n;
    ssize_t numSent;
    uint8_t* sendPos = buf;

    // Send until all bytes are sent
    while (numToSend > 0) {
        numSent = ::send(fd, (void*) sendPos, numToSend, 0);

        if (numSent == -1) {
            // Misc error with ::send()
            throw OpFailureException();
            // TODO: handle client disconnect?
        }

        numToSend -= numSent;
        sendPos += numSent;
    }
}

void Tcp::ConnSocket::close() {
    open = false;

    if (fd > -1) {
        if (::close(fd) < 0) {
            // Close failure
            throw OpFailureException();
        }
    }

    fd = -1; // indicate the FD has been closed
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

bool Tcp::ConnSocket::isOpen() {
    return open;
}
