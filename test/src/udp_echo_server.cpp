/**
 * @file udp_server.cpp
 * @author Andrew Obler (ajo2@rice.edu)
 * @brief Basic test/echo server for UDP datagrams.
 * @version 0.1
 * @date 2019-07-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <cstdlib>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "circular_buffer/circular_buffer.hpp"

/**
 * @brief Size of the buffer used when receiving bytes to echo. 
 */
#define TEST_RECV_BUF_SIZE 1024

int main(int argc, char** argv) {
    // Get user's requested port number
    int port;
    if (argc < 2) {
        port = 1234;
        std::cout << "No port given, setting it to 4444" << std::endl;
    } else {
        port = std::atoi(argv[1]);
        std::cout << "Attempting to open server on port " << port << std::endl;
    }

    // Create and bind a socket for reception of UDP packets
    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        std::cerr << "Could not create server socket" << std::endl;
        return -1;
    }

    sockaddr_in sock = { AF_INET, htons(port), INADDR_ANY };
    if (::bind(fd, (sockaddr*) &sock, sizeof(sockaddr_in)) < 0) {
        std::cerr << "Unable to bind socket" << std::endl;
        return -1;
    }

    // Receive bytes and print them, forever
    int num;
    uint8_t *buf = new uint8_t[TEST_RECV_BUF_SIZE];
    struct data_header header;
    struct data_item item;
    uint8_t *recv_ptr;
    std::cout << "Starting recv loop" << std::endl;
    while (true) {
        num = ::recv(fd, (void*) buf, TEST_RECV_BUF_SIZE, 0);
        std::cout << "Received " << num << " bytes:" << std::endl;
        if (num < 0) {
            std::cerr << "Error receiving" << std::endl;
        } else {
	    recv_ptr = buf;
	    printf("Header: %lu %lu\n", ((struct data_header *)recv_ptr)->sensor,
			    		((struct data_header *)recv_ptr)->length);
	    recv_ptr += sizeof(struct data_header);
	    for(;recv_ptr - buf < num; recv_ptr += sizeof(struct data_item)) {
		    printf("Data: %lu %lu\n", ((struct data_item *)recv_ptr)->reading,
						((struct data_item *)recv_ptr)->timestamp);
	    }

            // buf[num] = '\0'; // null-terminate for printing
            // std::cout << buf << std::endl;
        }
    }

    ::close(fd);
}
