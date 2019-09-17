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

#ifndef LUNA_VISITOR_HPP
#define LUNA_VISITOR_HPP

#include <stdint.h>

#include "worker_visitor.hpp"
#include "commands/tcp_commands.hpp"

/**
 * @brief The visitor that performs standard tasks for Luna.
 *
 * Commands are processed unbuffered as they are received over
 * the network.
 */
class luna_visitor : public worker_visitor {
    private:
	/**
	 * @brief Whether GITVC Is currently on.
	 */
        bool gitvc_on;

	/**
	 * @brief The number of GITVC actuations that have occured.
	 */
        uint8_t  gitvc_count;

    public:
	/**
	 * @brief Visits a command by performing the function associated
	 * 	  with that command.
	 */
        void visitCommand(COMMAND c) override;

	/**
	 * @brief The constructor for a Luna visitor.
	 * TODO access args read from configs.
	 */
        luna_visitor();
};

#endif //LUNA_VISITOR_HPP
