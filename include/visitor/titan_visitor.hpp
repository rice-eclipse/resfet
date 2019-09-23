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

#include "config/config.hpp"
#include "worker_visitor.hpp"

#ifndef __TITAN_VISITOR_HPP
#define __TITAN_VISITOR_HPP

/**
 * @brief The visitor that performs standard tasks for Titan.
 *
 * Commands are processed unbuffered as they are received over
 * the network.
 */
class TitanVisitor : private WorkerVisitor {
    public:
		/**
		 * @brief Visits a command by performing the function associated
		 * 	  with that command.
		 */
        void visitCommand(COMMAND c) override;
        
		/**
		 * @brief Default constructor. Initializes TitanVisitor::config to an
		 * 		  empty ConfigMapping.
		 */
        TitanVisitor();

		/**
		 * @brief The constructor for a Titan visitor.
		 * TODO access args read from configs.
		 */
        TitanVisitor(ConfigMapping& config);
};

#endif // __TITAN_VISITOR_HPP
