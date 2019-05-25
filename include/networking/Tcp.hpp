/**
 * @file tcp.hpp
 * @author Andrew Obler (ajo2@rice.edu)
 * @brief Methods related to TCP functionality.
 * @version 0.1
 * @date 2019-05-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __TCP_HPP
#define __TCP_HPP

/**
 * @brief Backlog for pending TCP connections on listening sockets
 */
#define TCP_BACKLOG 2

/**
 * @brief Contains all relevant functions for the TCP side of the engine
 *        controller, used for commands such as valve open/close and ignition.
 */
namespace Tcp {
    /**
     * @brief Open a socket to listen for incoming TCP connections.
     * 
     * @param port the port on which to open the socket
     * @return the file descriptor for the socket (-1 if unsuccessful)
     */
    int initListen(int port);

    /**
     * @brief Waits for a connection request and accepts it.
     * 
     * @param listenFd the listening file descriptor on which to wait
     * @return the file descriptor for the resulting connected socket (-1 if
     *         unsuccessful)
     */
    int acceptConn(int listenFd);
}

#endif
