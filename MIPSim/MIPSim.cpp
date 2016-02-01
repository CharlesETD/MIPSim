/**
 * Author:		Charles Duncan (CharlesETD@gmail.com)
 * Created:		Jan 31, 2016
 * Compiler:	MSVS Express 2012
 * OS:			Windows 7 Home Premium SP1 x64
 *
 * © Copyright 2016 Charles Duncan (CharlesETD@gmail.com)
 */

// Includes ******************************************************************
#include <iostream>
#include <string>
#include "MIPSCPU.h"
#include "MIPSRAM.h"
#include "MIPSSystem.h"
#include "MIPSWord.h"

// Global Constants **********************************************************
/**
 * Usage message displayed is invalid input is given.
 */
static const std::string& USAGE_MSG	= "MIPSim [ROM filename] -d";

/**
 * Title and copyright notice.
 */
static const std::string& TITLE_MSG	= "MIPSim: MIPS Simulator in C++\nCopyright 2016 Charles Duncan (CharlesETD@gmail.com)";

/**
 * Message displayed when rom cannot be loaded.
 */
static const std::string& FILE_ERROR_MSG = "Error: Failed to load ROM.";

/**
 * 2^20 words of memory.
 */
static const MIPSUWord CAPACITY = 1048576;

// Method Declarations *******************************************************
/**
 * Entry point.
 * @param argc, number of args.
 * @param argv, values of args.
 * @return 0 if no errors occured, otherwise an error occured.
 */
int main (int argc, char* argv[]);

/**
 * Performs unit tests on MIPSRAM.
 */
void ramTest (void);

// Method Definitions ********************************************************
int main (int argc, char* argv[]) {

	bool debugMode = false;
	std::string filename;

#ifdef _DEBUG
	MIPSRAM::runUnitTests ();
	MIPSCPU::runUnitTests ();
	MIPSSystem::runUnitTests ();
#endif

	if (!(argc == 2 || argc == 3)) {

		std::cout << USAGE_MSG << std::endl;
		return -1;

	} else if (argc == 3) {

		debugMode = true;

		if (std::string (argv[1]) == "-d") {

			filename = argv[2];

		} else if (std::string (argv[2]) == "-d") {

			filename = argv[1];

		} else {

			std::cout << USAGE_MSG << std::endl;
			return -1;

		}

	} else {

		filename = argv[1];
		
	}

	MIPSSystem system (CAPACITY, debugMode);

	std::cout << TITLE_MSG << std::endl;

	if (!system.loadROM (filename)) {

		std::cout << FILE_ERROR_MSG << std::endl;
		return -1;

	}

	while (system.isRunning ()) {

		system.tick ();

	}

	return 0;

}