/**
 * Author:		Charles Duncan (CharlesETD@gmail.com)
 * Created:		Jan 31, 2015
 * Compiler:	MSVS Express 2012
 * OS:			Windows 7 Home Premium SP1 x64
 *
 * © Copyright 2016 Charles Duncan (CharlesETD@gmail.com)
 */

// Includes ******************************************************************
#include <string>
#include "MIPSCPU.h"
#include "MIPSRAM.h"
#include "Registers.h"

#pragma once
#ifndef MIPS_SYSTEM_H
#define MIPS_SYSTEM_H

/**
 * A driver class for the MIPSCPU and MIPSRAM.
 */
class MIPSSystem {

private:
	// Private Static Constants **********************************************
	static const std::string& MSG_CANNOT_TICK;		/**< Exception displayed when the program cannot run but is still ticked. >*/
	static const std::string& MSG_DEBUG_CONTROLS;	/**< Message displayed indicating available controls. >*/

	// Private Constants *****************************************************
	const MIPSUWord MEMORY_CAPACITY;				/**< How many words of virtual memory the simulator has. >*/

	// Private Variables *****************************************************
	MIPSRAM m_ram;									/**< The main memory. >*/
	MIPSCPU m_cpu;									/**< The system CPU. >*/
	bool m_isDebugMode;								/**< Whether or not debug mode is enabled. >*/
	bool m_isRunning;								/**< Is the system running. >*/

public:
	// Public Ctors/Dtors ****************************************************
	/**
	 * Ctor.
	 * @param capacity number of memory words.
	 * @param debugMode should debug mode be used.
	 */
	MIPSSystem (const MIPSUWord capacity, const bool debugMode = false);

	// Accessors *************************************************************
	/**
	 * Gets whether the program is running or not.
	 * @return true if running, otherwise false.
	 */
	bool isRunning (void) const;

	// Public Methods ********************************************************
	/**
	 * Updates the system.
	 * @throw runtime_error if called after the program stops running.
	 */
	void tick (void);

	/**
	 * Resets the CPU and RAM.
	 */
	void reset (void);

	/**
	 * Loads a ROM file into the RAM.
	 * @param filename of ROM to load.
	 * @return true if loaded, otherwise false.
	 */
	bool loadROM (const std::string& filename);

	// Public Static Methods *************************************************
	/**
	 * Performs unit tests on MIPSSystem.
	 */
	static void runUnitTests (void);

private:
	// Private Ctors/Dtors ***************************************************
	/**
	 * Copy Ctor.
	 * @param mipsSystem to copy from.
	 */
	MIPSSystem (const MIPSSystem& mipsSystem);

	// Private Operators *****************************************************
	/**
	 * Copy Assignment operator.
	 * @param mipsSystem to copy from.
	 * @return reference to self.
	 */
	MIPSSystem& operator= (const MIPSSystem& mipsSystem);

	// Private Methods *******************************************************
	/**
	 * System call to print an integer to the console.
	 */
	void sysCallPrintInteger (void);

	/**
	 * System call to print a string to the console.
	 */
	void sysCallPrintString (void);

	/**
	 * System call to print a character to the console.
	 */
	void sysCallPrintChar (void);

	/**
	 * System call to read an integer into memory.
	 */
	void sysCallReadInteger (void);

	/**
	 * System call to read a string into memory.
	 */
	void sysCallReadString (void);

	/**
	 * System call to allocate heap memory.
	 */
	void sysCallAllocateMemory (void);

	/**
	 * Handles the current system call.
	 */
	void processSystemCall (void);

	/**
	 * Handles debug mode. Blocks until users presses enter.
	 */
	void handleDebugMode (void);

};

#endif