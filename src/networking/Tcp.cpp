/**
 * @file Tcp.cpp
 * @author Andrew Obler (ajo2@rice.edu)
 * @brief Implementation of methods in Tcp.hpp.
 * @version 0.1
 * @date 2019-05-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

#include "networking/Tcp.hpp"

int Tcp::initListen(int port) {
    // Open a blank socket to start
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        // TODO: have more specific handling of errno
        return -1;
    }

    // Configure the socket to allow other open connections on this device
    // (allow the local address to be reused)
    int optTrue = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *) &optTrue, sizeof(int)) < 0) {
        // TODO: errno handling
        return -1;
    }

    // Set up a sockaddr_in struct to carry the necessary info for bind()
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // clear memory for serverAddr
    serverAddr.sin_family = AF_INET; // protocol family
    serverAddr.sin_port = htons(port); // port number
    serverAddr.sin_addr.s_addr = INADDR_ANY; // local IP address

    // Bind the socket to the specified port
    if (bind(fd, (sockaddr *) &serverAddr, sizeof(sockaddr_in)) < 0) {
        // TODO: errno handling
        return -1;
    }

    // Start the socket listening for new connections
    if (listen(fd, TCP_BACKLOG) < 0) {
        // TODO: errno handling
        return -1;
    }

    return fd;
}

int Tcp::acceptConn(int listenFd) {
    sockaddr_in clientAddr; // TODO: print out info about client?
    socklen_t clientSize = sizeof(sockaddr_in);
    int connFd = accept(listenFd, (sockaddr *) &clientAddr, &clientSize);
    if (connFd < 0) {
        // TODO: errno handling
        return -1;
    }

    std::cout << "Connected" << std::endl; // TODO: logger

    return connFd;
}
