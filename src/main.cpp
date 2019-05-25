#include <iostream>

#include "networking/Tcp.hpp"

int main() {
    int listenFd = Tcp::initListen(1234);

    while (true) {
        int connFd = Tcp::acceptConn(listenFd);
    }
    
    return 0;
}
