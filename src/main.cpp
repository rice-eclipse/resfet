#include <cstdint>
#include <iostream>

#include "networking/Udp.hpp"

// Simple send test for UDP interface
int main() {
    // Set up the socket
    Udp::OutSocket sock;
    try {
        sock.setDest("127.0.0.1", 4444);
    } catch (Udp::OpFailureException& ofe) {
        std::cerr << "Could not set destination" << std::endl;
        return -1;
    }

    // Open the socket up for sending
    try {
        sock.enable();
    } catch (Udp::OpFailureException& ofe) {
        std::cerr << "Could not open socket" << std::endl;
        return -1;
    }

    // Send a single byte
    try {
        sock.sendByte(5);
    } catch (Udp::BadOutSocketException& bse) {
        std::cerr << "Socket is not open" << std::endl;
        return -1;
    } catch (Udp::OpFailureException& ose) {
        std::cerr << "Unable to send byte" << std::endl;
        return -1;
    }

    // Send multiple bytes
    char buf[24] = "Hello, message for you!";
    try {
        sock.sendBuf((uint8_t*) buf, 24);
    } catch (Udp::BadOutSocketException& bse) {
        std::cerr << "Socket is not open" << std::endl;
        return -1;
    } catch (Udp::OpFailureException& ose) {
        std::cerr << "Unable to send bytes" << std::endl;
        return -1;
    }

    // Close the socket
    try {
        sock.close();
    } catch (Udp::OpFailureException& ose) {
        std::cerr << "Could not close socket" << std::endl;
        return -1;
    }

    return 0;
}
