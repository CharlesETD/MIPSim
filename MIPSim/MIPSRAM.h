/**
 * Author:		Charles Duncan (CharlesETD@gmail.com)
 * Created:		Jan 31, 2016
 * Compiler:	MSVS Express 2012
 * OS:			Windows 7 Home Premium SP1 x64
 *
 * © Copyright 2016 Charles Duncan (CharlesETD@gmail.com)
 */

// Includes ******************************************************************
#include <string>
#include "Byte.h"
#include "MIPSWord.h"

#pragma once
#ifndef MIPS_RAM_H
#define MIPS_RAM_H

/**
 * Class for representing a MIPS RAM module.
 */
class MIPSRAM {

private:
	// Private Static Constants **********************************************
	static const std::string& MSG_INVALID_CAPACITY;						/**< Exception text when ctor is given bad capacity. >*/
	static const std::string& MSG_CAPACITY_MISMATCH;					/**< Exception text when copy ctor capacities do not match. >*/
	static const std::string& MSG_ACCESS_VIOLATION_HEAD;				/**< Header of access violation exception. >*/
	static const std::string& MSG_ACCESS_VIOLATION_WORD;				/**< Word specifier of access violation exception. >*/
	static const std::string& MSG_ACCESS_VIOLATION_BYTE;				/**< Byte specifier of access violation exception. >*/
	static const std::string& MSG_ACCESS_VIOLATION_ADDRESS;				/**< Address specifier of access violation exception. >*/
	static const std::string& MSG_ACCESS_VIOLATION_IS_OUT_OF_BOUNDS;	/**< Tail of access violation exception. >*/
	static const std::string& MSG_INSUFFICIENT_CAPACITY;				/**< Exception text when allocateRAM capacity is too small. >*/

	// Private Constants *****************************************************
	const MIPSUWord WORD_CAPACITY;	/**< The maximum number of words that can be addressed. >*/
	const MIPSUWord BYTE_CAPACITY;	/**< The maximum number of bytes that can be addressed. >*/

	// Private Variables *****************************************************
	MIPSUWord* m_dataWords;			/**< The words of data stored in RAM. >*/

public:
	// Public Ctors/Dtors ****************************************************
	/**
	 * Ctor.
	 * @param wordCapacity the maximum number of words to store in RAM.
	 * @throw invalid_argument if capacity is 0.
	 */
	MIPSRAM (const MIPSUWord wordCapacity);

	/**
	 * Copy Ctor.
	 * @param mipsRAM to copy from.
	 */
	MIPSRAM (const MIPSRAM& mipsRAM);

	/**
	 * Dtor.
	 */
	virtual ~MIPSRAM (void);

	// Public Operators ******************************************************
	/**
	 * Copy Assignment Operator
	 * @param mipsRAM to copy from.
	 * @return reference to this MIPSRAM.
	 * @throw invalid_argument if WORD_CAPACITY of each MIPSRAM does not match.
	 */
	MIPSRAM& operator= (const MIPSRAM& mipsRAM);

	// Accessors *************************************************************
	/**
	 * Gets the word capacity of the RAM.
	 * @return capacity in words.
	 */
	MIPSUWord getWordCapacity (void) const;

	/**
	 * Gets the byte capacity of the RAM.
	 * @return capacity in bytes.
	 */
	MIPSUWord getByteCapacity (void) const;

	/**
	 * Gets a word from RAM.
	 * @param address of word.
	 * @return value of word at address or 0 if address is invalid.
	 * @throw out_of_range if address exceeds capacity.
	 */
	MIPSUWord getWord (const MIPSUWord address) const;

	/**
	 * Gets a byte from RAM.
	 * @param address of byte.
	 * @return value of byte at address or 0 if address is invalid.
	 * @throw out_of_range if address exceeds capacity.
	 */
	UByte getByte (const MIPSUWord address) const;

	// Mutators **************************************************************
	/**
	 * Set the value of a word in RAM.
	 * @param address of word.
	 * @param value new value for word.
	 * @throw out_of_range if address exceeds capacity.
	 */
	void setWord (const MIPSUWord address, const MIPSUWord value);

	/**
	 * Set the value of a byte in RAM.
	 * @param address of byte.
	 * @param value new value for byte.
	 * @throw out_of_range if address exceeds capacity.
	 */
	void setByte (const MIPSUWord address, const UByte value);

	// Public Methods ********************************************************
	/**
	 * Determines if the given address is within memory.
	 * @param address to check.
	 * @return true if in memory, otherwise false.
	 */
	bool isValidWordAddress (const MIPSUWord address) const;

	/**
	 * Determines if the given address is within memory.
	 * @param address to check.
	 * @return true if in memory, otherwise false.
	 */
	bool isValidByteAddress (const MIPSUWord address) const;

	/**
	 * Sets all of the words in RAM to 0.
	 */
	void reset (void);

	// Public Static Methods *************************************************
	/**
	 * Performs unit tests on MIPSRAM.
	 */
	static void runUnitTests (void);

private:
	// Private Methods *******************************************************
	/**
	 * Copies the RAM contents to an array of the same size.
	 * @param toArray destination array preallocated before calling.
	 */
	void copyRAM (MIPSUWord* const toArray) const;

	/**
	 * Generates text for an access violation exception with the given address.
	 * @param address to report.
	 * @param isWord true if the address points to a word, false if it points to a byte.
	 * @return text of access violation.
	 */
	std::string generateAccessViolationText (const MIPSUWord address, const bool isWord) const;

	/**
	 * Allocates memory for the RAM. Throws an exception if WORD_CAPACITY is zero.
	 * @throw length_error if capacity is 0.
	 */
	void allocateRAM (void);

};

#endif