#include <cstdint>
#include <iostream>
#include <string.h>

#include "networking/Udp.hpp"
#include "logger/logger.hpp"
#include "thread/thread.hpp"

// Simple send test for UDP interface
int main() {
    Logger network_logger("Networking", "NetworkLog", LogLevel::DEBUG);
  
    // Set up the socket
    Udp::OutSocket sock;
    try {
        sock.setDest("127.0.0.1", 4444);
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

    // Send a single byte
    try {
        sock.sendByte(5);
    } catch (Udp::BadOutSocketException& bse) {
        network_logger.error("Socket is not open\n");
        return -1;
    } catch (Udp::OpFailureException& ose) {
        network_logger.error("Unable to send byte\n");
        return -1;
    }

    // Send multiple bytes
    char buf[24] = "Hello, message for you!";
    try {
        sock.sendBuf((uint8_t*) buf, 24);
    } catch (Udp::BadOutSocketException& bse) {
        network_logger.error("Socket is not open\n");
        return -1;
    } catch (Udp::OpFailureException& ose) {
        network_logger.error("Unable to send bytes\n");
        return -1;
    }

    // Close the socket
    try {
        sock.close();
    } catch (Udp::OpFailureException& ose) {
        network_logger.error("Could not close socket\n");
        return -1;
    }

    return 0;
}
