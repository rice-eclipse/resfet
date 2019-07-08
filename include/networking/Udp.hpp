/**
 * @file Udp.hpp
 * @author Andrew Obler (ajo2@rice.edu)
 * @brief Methods related to UDP functionality.
 * @version 0.1
 * @date 2019-05-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __UDP_HPP
#define __UDP_HPP

#include <cstddef> // for size_t
#include <cstdint> // for uint8_t
#include <exception>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

// Max bytes for destination hostnames
#define UDP_DEST_HOST_SIZE 128

// Max bytes for destination services
#define UDP_DEST_SERV_SIZE 128

/**
 * @brief Contains all relevant functions for the UDP side of the engine
 *        controller, used to send data collected from sensors back to the base
 *        station.
 */
namespace Udp {
    // Forward-declarations of classes

    class OutSocket;

    class OpFailureException;

    class BadOutSocketException;

    /**
     * @brief Represents a one-way datagram socket for sending UDP packets. 
     */
    class OutSocket {
        private:
            /**
             * @brief The file descriptor currently associated with thi
             *        socket. 
             */
            int fd;

            /**
             * @brief The destination that this socket is currently tethered
             *        to. 
             */
            sockaddr_in dest;

            /**
             * @brief Buffer which stores the address/hostname of the
             *        destination as a string.
             */
            char destHost[UDP_DEST_HOST_SIZE];

            /**
             * @brief Buffer which stores the port/service of the client as a
             *        string. 
             */
            char destServ[UDP_DEST_SERV_SIZE];

            /**
             * @brief Whether the socket is currently open and ready for
             *        sending to its destination. 
             */
            bool open;

        public:
            /**
             * @brief Create a blank, nonfunctional socket with an empty
             *        destination. 
             */
            OutSocket();
            
            /**
             * @brief Create a closed socket with the given address and port as
             *        the current destination.
             * 
             * @param addr the destination address as an IPv4 string
             * @param port the destination port as an integer
             * @throw OpFailureException if the given address is invalid
             */
            OutSocket(char* const addr, int port);

            /**
             * @brief Destroy this socket. The socket must be closed manually
             *        prior to destruction! 
             */
            ~OutSocket();

            /**
             * @brief Open this socket for sending to its current destination.
             *        If already open, will reopen the socket and overwrite the
             *        file descriptor without closing it!
             * 
             * @throw OpFailureException if the socket could not be opened or
             *        configured properly
             */
            void enable();

            /**
             * @brief Change the destination of this socket.
             * 
             * @param addr the new destination address as an IPv4 string
             * @param port the new destination port as an integer
             * @throw OpFailureException if the given address is invalid
             */
            void setDest(char* const addr, int port);

            /**
             * @brief Send a single byte to the current destination.
             * 
             * @param b the byte to send
             * @throw BadOutSocketException if the socket is not ready for a
             *        send
             * @throw OpFailureException if the send fails despite the socket
             *        being ready
             */
            void sendByte(uint8_t b);

            /**
             * @brief Send any number of bytes to the current destination from
             *        a buffer.
             * 
             * @param buf the buffer from which to send bytes
             * @param n the number of bytes to send
             * @throw BadOutSocketException if the socket is not ready for a
             *        send
             * @throw OpFailureException if the send fails despite the socket
             *        being ready
             */
            void sendBuf(uint8_t* buf, size_t n);

            /**
             * @brief Close this socket, rendering it unable to send until
             *        enable() is called again.
             * 
             * @throw OpFailureException if the socket cannot be closed
             *        properly
             */
            void close();

            /**
             * @brief Get the file descriptor associated with this socket.
             * 
             * @return the FD as an integer
             */
            int getFd();

            /**
             * @brief Get the current destination hostname/address in string
             *        form.
             * 
             * @return the hostname/address as a string
             */
            std::string getDestHost();

            /**
             * @brief Get the current destination port/service in string form.
             * 
             * @return the port/service as a string
             */
            std::string getDestServ();

            /**
             * @brief Get whether this socket is open and ready for sending.
             * 
             * @return true if the socket is open
             * @return false otherwise
             */
            bool isOpen();
    };

    /**
     * @brief Indicates that a requested socket operation (e.g. send, close)
     *        was unsuccessful.
     */
    class OpFailureException : public std::exception {
        public:
            virtual const char* what() const noexcept {
                return "DGram socket operation failure";
            }
    };

    /**
     * @brief Indicates that an OutSocket does not meet the conditions required
     *        to perform an operation (e.g. trying to send when the socket is
     *        closed). 
     */
    class BadOutSocketException : public std::exception {
        public:
            virtual const char* what() const noexcept {
                return "DGram OutSocket not ready";
            }
    };
}

#endif
