/**
 * @file Tcp.hpp
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

#include <cstddef> // for size_t
#include <cstdint> // for uint8_t
#include <string>

// Backlog for pending connections on listening sockets
#define TCP_BACKLOG 2

// Size of buffer in which to store client's hostname
#define CLIENT_HOST_SIZE 128

// Size of buffer in which to store client's service
#define CLIENT_SERV_SIZE 128

/**
 * @brief Contains all relevant functions for the TCP side of the engine
 *        controller, used for commands such as valve open/close and ignition.
 */
namespace Tcp {
    // Forward-declarations of classes

    class ListenSocket;

    class ConnSocket;

    /**
     * @brief Represents a socket used to listen for incoming connections. 
     */
    class ListenSocket {
        private:
            // Members

            /**
             * @brief The port on which this socket will listen. Cannot be
             *        changed after construction.
             */
            int port;

            /**
             * @brief The file descriptor associated with this socket. 
             */
            int fd;

            /**
             * @brief Whether this socket is currently listening for
             *        connections. 
             */
            bool listening;

        public:
            // Constructors and destructors

            /**
             * @brief Create a new ListenSocket associated with the given port.
             *        The socket is not yet listening.
             * 
             * @param port the port to listen on
             * @throw SocketCreateFailureException if the socket could not be
             *        created TODO
             */
            ListenSocket(int port);

            /**
             * @brief Destroy this socket. Also closes the socket. 
             */
            ~ListenSocket();

            // Methods

            /**
             * @brief Start the socket listening for connections on port.
             * 
             * @throw ListenFailureException if the listen fails TODO
             */
            void listen();

            /**
             * @brief Accept an incoming connection on this socket. Blocks
             *        until such a connection request is available. The created
             *        connection must be closed separately.
             * 
             * @return a ConnSocket representing the accepted connection
             * @throw AcceptFailureException if the accept fails TODO
             */
            ConnSocket accept();

            /**
             * @brief Close this socket. Any unaccepted requests are rejected.
             * 
             * @throw CloseFailureException if the socket could not be closed
             */
            void close();

            // Getters

            /**
             * @brief Get the port on which this socket is listening.
             * 
             * @return the port as an int
             */
            int getPort();

            /**
             * @brief Get the file descriptor coupled to this socket.
             * 
             * @return the FD as an int
             */
            int getFd();

            /**
             * @brief Get whether this socket is currently listening.
             * 
             * @return true if the socket is listening
             * @return false otherwise
             */
            bool isListening();
    };

    /**
     * @brief Represents a connection accepted via ListenSocket::accept. Cannot
     *        be constructed normally.
     */
    class ConnSocket {
        private:
            /**
             * @brief The file descriptor associated with this socket. 
             */
            int fd;

            /**
             * @brief Buffer which stores the address/hostname of the client as
             *        a string.
             */
            char clientHost[CLIENT_HOST_SIZE];

            /**
             * @brief Buffer which stores the port/service of the client as a
             *        string. 
             */
            char clientServ[CLIENT_SERV_SIZE];

            /**
             * @brief Whether the socket is currently open and ready for
             *        sending/receiving.
             */
            bool valid;

            // Constructors

            /**
             * @brief Create a blank, invalid socket with empty hostname and
             *        service. No connection is established.
             */
            ConnSocket();

            // Friends

            /**
             * @brief ListenSocket has private access to call constructor.
             * 
             * @relates ListenSocket
             */
            friend ListenSocket;
        
        public:
            // Destructors

            /**
             * @brief Destroy this socket. No special behavior.
             */
            ~ConnSocket();

            // Methods

            /**
             * @brief Read a single byte from the client. Blocks until such a
             *        byte is available to read.
             * 
             * @return the byte that was read
             * @throw PrematureCloseException if the connection is closed
             *        before a byte could be read TODO
             * @throw InvalidSocketException if the socket is not open TODO
             * @throw RecvFailureException if there is some other error reading
             *        TODO
             */
            uint8_t recvByte();

            /**
             * @brief Read a certain number of bytes into a given preallocated
             *        buffer. Blocks until all bytes are read.
             * 
             * @param buf the buffer in which to store the bytes. Must have
             *        size at least n
             * @param the number of bytes to read
             * @throw PrematureCloseException if the connection is closed
             *        before n bytes could be read TODO
             * @throw InvalidSocketException if the socket is not open TODO
             * @throw RecvFailureException if there is some other error reading
             *        TODO
             */
            void recvBuf(uint8_t* buf, size_t n);

            /**
             * @brief Write a single byte to the client. Blocks until the byte
             *        is sent.
             * 
             * @param b the byte to send
             * @throw PrematureCloseException if the connection is closed
             *        before the byte could be written TODO
             * @throw InvalidSocketException if the socket is not open TODO
             * @throw SendFailureException if there is some other error writing
             *        TODO
             */
            void sendByte(uint8_t b);

            /**
             * @brief Write a certain number of bytes from a given buffer to
             *        the client. Blocks until all bytes are written.
             * 
             * @param buf the buffer from which to send. Must be of size at
             *        least n
             * @param n the number of bytes to send
             * @throw PrematureCloseException if the connection is closed
             *        before n bytes could be written TODO
             * @throw InvalidSocketException if the socket is not open TODO
             * @throw SendFailureException if there is some other error writing
             *        TODO
             */
            void sendBuf(uint8_t* buf, size_t n);

            /**
             * @brief Close this socket. Any unread data from the client is
             *        lost.
             * 
             * @throw CloseFailureException if the socket could not be closed
             */
            void close();

            // Getters

            /**
             * @brief Get the file descriptor coupled to this socket.
             * 
             * @return the FD as an int
             */
            int getFd();

            /**
             * @brief Get the hostname (address) of the client.
             * 
             * @return the hostname as a string
             */
            std::string getClientHostname();

            /**
             * @brief Get the service (port) of the client.
             * 
             * @return the service as a string
             */
            std::string getClientService();

            /**
             * @brief Get whether the socket is open (i.e., has not yet been
             *        closed with close()).
             * 
             * @return true if open
             * @return false otherwise
             */
            bool isValid();
    };
}

#endif
