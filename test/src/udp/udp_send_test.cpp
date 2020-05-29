#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ADDR "192.168.1.138"
#define PORT 4444

int main() {
    int sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd < 0) {
        std::cerr << "ERROR: could not create sending socket" << std::endl;
	return -1;
    }
    std::cout << "Successfully created sending socket" << std::endl;
    in_addr addr;
    if (inet_aton(ADDR, &addr) == 0) {
        std::cerr << "ERROR: ADDR is invalid" << std::endl;
	close(sockFd);
	return -1;
    }
    int port = htons(PORT);
    sockaddr_in dest = { AF_INET, port, addr };
    
    timespec sleepTime = { 0, 250000000 };
    char send = 'A';
    ssize_t numSent;
    while (true) {
        numSent = sendto(sockFd, (void*) &send, 1, 0, (sockaddr*) &dest, sizeof(sockaddr_in));
        if (numSent == -1) {
            std::cerr << "ERROR: sendto() failed" << std::endl;
	    close(sockFd);
	    return -1;
        } else if (numSent == 0) {
	    std::cout << "WARNING: send '" << send << "' failed, continuing" << std::endl;
	} else {
	    std::cout << "Sent '" << send << "'" << std::endl;
	}
	send++;
        nanosleep(&sleepTime, NULL);
    }
    close(sockFd);
    return 0;
}
