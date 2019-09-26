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
#ifndef __LUNA_VISITOR_HPP
#define __LUNA_VISITOR_HPP 
#include <stdint.h>

#include "config/config.hpp"
#include "worker_visitor.hpp"

/**
 * @brief The visitor that performs standard tasks for Luna.
 *
 * Commands are processed unbuffered as they are received over
 * the network.
 */
class LunaVisitor : private WorkerVisitor {
    private:
		/**
		 * @brief Whether GITVC Is currently on.
		 */
        bool gitvc_on;

	/**
	 * @brief Whether this test requires GITVC.
	 */
	bool use_gitvc;

		/**
		 * @brief The number of GITVC actuations that have occured.
		 */
        uint8_t  gitvc_count;

	/**
	 * @brief Time delay between starting ignition and starting GITVC.
	 */
	uint32_t time_between_gitvc_ms;

	/**
	 * @brief Time delay between closing the GITVC valve and opening it again.
	 */
	uint32_t gitvc_wait_time_ms;

	/**
	 * @brief Durations that the GITVC valve is kept open.
	 */
	std::vector<uint32_t> gitvc_times_ms;

    public:
		/**
		 * @brief Visits a command by performing the function associated
		 * 	  with that command.
		 */
        void visitCommand(COMMAND c) override;

		/**
		 * @brief Perform GITVC actuations according to the data in WorkerVisitor::config.
		 */
		void doGITVC();

		/**
		 * @brief Default constructor. Initializes LunaVisitor::config to an
		 * 		  empty ConfigMapping.
		 */
        LunaVisitor();

		/**
		 * @brief The constructor for a Luna visitor.
		 * TODO access args read from configs.
		 */
        LunaVisitor(ConfigMapping& config);
};

#endif // __LUNA_VISITOR_HPP
