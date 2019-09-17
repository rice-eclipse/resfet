/**
 * @file luna_visitor.hpp
 * @author Andrew Obler (ajo2@rice.edu)
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Visitor for processing received commands 
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 */

#include <stdint.h>

#include "worker_visitor.hpp"
#include "commands/tcp_commands.hpp"

#ifndef TITAN_VISITOR_HPP
#define TITAN_VISITOR_HPP

/**
 * @brief The visitor that performs standard tasks for Titan.
 *
 * Commands are processed unbuffered as they are received over
 * the network.
 */
class titan_visitor : public worker_visitor {
    public:
	/**
	 * @brief Visits a command by performing the function associated
	 * 	  with that command.
	 */
        void visitCommand(COMMAND c) override;
        
	/**
	 * @brief The constructor for a Titan visitor.
	 * TODO access args read from configs.
	 */
        titan_visitor();
};

#endif //TITAN_VISITOR_HPP
