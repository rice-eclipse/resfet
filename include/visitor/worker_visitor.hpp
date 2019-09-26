/**
 * @file worker_visitor.hpp
 * @author Andrew Obler (ajo2@rice.edu)
 * @author Tommy Yuan (ty19@rice.edu)
 * @brief Visitor for processing received commands 
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef __WORKER_VISITOR_HPP
#define __WORKER_VISITOR_HPP

#include <mutex>
#include <vector>
#include <time.h>
#include <stdint.h>

#include "time/time.hpp"
#include "config/config.hpp"
#include "logger/logger.hpp"

// Time between checks for ignition state, in milliseconds (see WorkerVisitor::doIgn())
#define IGN_CHECK_MS 50

enum COMMAND: uint8_t {
    UNSET_VALVE1 = 0,
    SET_VALVE1,
    UNSET_VALVE2,
    SET_VALVE2,
    UNSET_VALVE3,
    SET_VALVE3,
    START_IGNITION,
    STOP_IGNITION,
    SET_WATER,
    UNSET_WATER,
    SET_GITVC,
    UNSET_GITVC,
    LEAK_CHECK,
    FILL,
    FILL_IDLE,
    DEF,
    TAPE_ON,
    TAPE_OFF,
    NUM_COMMANDS
};

extern const char* command_names[NUM_COMMANDS];

/**
 * @brief Defines the superclass for the Luna and Titan visitors.
 * 	  A visitor "visits" a received command performs the appropriate
 *	  task based on the command type.
 * 	  Based on a design by Arne Mertz.
 *        See https://arne-mertz.de/2016/04/visitor-pattern-part-2-enum-based-visitor/
 */
class WorkerVisitor {
	private:
		/**
		 * @brief Configurations to associate with this visitor, e.g. GITVC
		 * 		  and burn time.
		 */
		ConfigMapping config;

		/**
		 * @brief Whether the ignition sequence is currently enabled.
		 */
                bool burn_on;

		/**
		 * @brief Mutex that is used to read/write burn_on across threads.
		 */
		std::mutex burnMtx;

		/**
		 * @brief The amount of time to wait between starting ignition and
		 *        opening the main valve.
		 */
		uint32_t preignite_ms;

		/**
		 * @brief The duration of the hotflow, from starting ignition to 
		 *        stopping ignition and closing the main valve.
		 */
		uint32_t hotflow_ms;

	public:
		/**
		 * @brief The logger for this worker.
		 */
		Logger logger;

		/**
		 * @brief Default constructor. Initializes WorkerVisitor::config to an empty ConfigMapping. 
		 */
		WorkerVisitor();

		/**
		 * @brief The constructor for this base class.
		 * 
		 * @param config the configuration mapping to associate with this visitor
		 */
		WorkerVisitor(ConfigMapping& config);

		/**
		 * @brief Visits a command by performing the function associated
		 * 	  with that command.
		 */
                virtual void visitCommand(COMMAND c);

			
		/**
		 * @brief Function that is performed by the thread created in WorkerVisitor::doIgn().
		 * 
		 * @param time the total burn time in milliseconds
		 * @param pBurnOn a pointer to the corresponding WorkerVisitor::burn_on
		 * @param pMtx a mutex that is locked to check pBurnOn
		 */
		static void ignThreadFunc(timestamp_t time, bool* pBurnOn, std::mutex* pMtx);

        /**
         * @brief Operation corresponding to the beginning of ignition. 
         */
        void doIgn();
};

#endif // __WORKER_VISITOR_HPP
