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
#include <sstream>
#include <stdexcept>
#include <string>
#include "Byte.h"
#include "MIPSRAM.h"
#include "MIPSWord.h"

// Static Constant Definitions ***********************************************
const std::string& MIPSRAM::MSG_INVALID_CAPACITY					= "Invalid Argument at MIPSRAM::MIPSRAM: Word capacity must not be 0.";
const std::string& MIPSRAM::MSG_CAPACITY_MISMATCH					= "Invalid Argument at MIPSRAM::operator=: Word capacities of caller and callee must match.";
const std::string& MIPSRAM::MSG_ACCESS_VIOLATION_HEAD				= "Access violation at MIPSRAM::getByte/setByte/getWord/setWord: ";
const std::string& MIPSRAM::MSG_ACCESS_VIOLATION_WORD				= "Word";
const std::string& MIPSRAM::MSG_ACCESS_VIOLATION_BYTE				= "Byte";
const std::string& MIPSRAM::MSG_ACCESS_VIOLATION_ADDRESS			= " address 0x";
const std::string& MIPSRAM::MSG_ACCESS_VIOLATION_IS_OUT_OF_BOUNDS	= " is out of bounds.";
const std::string& MIPSRAM::MSG_INSUFFICIENT_CAPACITY				= "Invalid Argument at MIPSRAM::allocateRAM: MIPSRAM must be allocated with at least 1 word of capacity.";

// Method Definitions ********************************************************
MIPSRAM::MIPSRAM (const MIPSUWord wordCapacity) : WORD_CAPACITY (wordCapacity), BYTE_CAPACITY (WORD_CAPACITY * 4), m_dataWords (nullptr) {

	try {

		allocateRAM ();

	} catch (std::length_error) {

		throw std::invalid_argument (MSG_INVALID_CAPACITY);

	}

	reset ();

}

//****************************************************************************
MIPSRAM::MIPSRAM (const MIPSRAM& mipsRAM) : WORD_CAPACITY (mipsRAM.WORD_CAPACITY), BYTE_CAPACITY (mipsRAM.BYTE_CAPACITY), m_dataWords (nullptr) {

	allocateRAM ();

	mipsRAM.copyRAM (m_dataWords);

}

//****************************************************************************
MIPSRAM::~MIPSRAM (void) {

	// Memory leaks of an entire bank of RAM are potentially disasterous.
	delete[] m_dataWords;
	m_dataWords = nullptr;

}

//****************************************************************************
MIPSRAM& MIPSRAM::operator= (const MIPSRAM& mipsRAM) {

	// Make sure we are not trying to assign to ourself.
	if (this != &mipsRAM) {

		if (WORD_CAPACITY == mipsRAM.WORD_CAPACITY) {

			delete[] m_dataWords;
			m_dataWords = nullptr;

			allocateRAM ();

			mipsRAM.copyRAM (m_dataWords);

		} else {

			throw std::invalid_argument (MSG_CAPACITY_MISMATCH);

		}

	}

	return *this;

}

//****************************************************************************
MIPSUWord MIPSRAM::getWordCapacity (void) const {

	return WORD_CAPACITY;

}

//****************************************************************************
MIPSUWord MIPSRAM::getByteCapacity (void) const {

	return BYTE_CAPACITY;

}

//****************************************************************************
MIPSUWord MIPSRAM::getWord (const MIPSUWord address) const {

	if (!isValidWordAddress (address)) {

		throw std::out_of_range (generateAccessViolationText (address, true));
		return 0;

	}

	return m_dataWords[address];

}

//****************************************************************************
UByte MIPSRAM::getByte (const MIPSUWord address) const {

	if (!isValidByteAddress (address)) {

		throw std::out_of_range (generateAccessViolationText (address, false));
		return 0;

	}

	return (reinterpret_cast<UByte *> (m_dataWords))[address];

}

//****************************************************************************
void MIPSRAM::setWord (const MIPSUWord address, const MIPSUWord value) {

	if (!isValidWordAddress (address)) {

		throw std::out_of_range (generateAccessViolationText (address, true));

	}

	m_dataWords[address] = value;

}

//****************************************************************************
void MIPSRAM::setByte (const MIPSUWord address, const UByte value) {

	if (!isValidByteAddress (address)) {

		throw std::out_of_range (generateAccessViolationText (address, false));

	}

	(reinterpret_cast<Byte *> (m_dataWords))[address] = value;

}

//****************************************************************************
bool MIPSRAM::isValidWordAddress (const MIPSUWord address) const {

	if (address < WORD_CAPACITY) {

		return true;

	}

	return false;

}

//****************************************************************************
bool MIPSRAM::isValidByteAddress (const MIPSUWord address) const {

	if (address < BYTE_CAPACITY) {

		return true;

	}

	return false;

}

//****************************************************************************
void MIPSRAM::reset (void) {

	std::fill (m_dataWords, m_dataWords + WORD_CAPACITY, 0);

}

//****************************************************************************
void MIPSRAM::copyRAM (MIPSUWord* const toArray) const {

	// I would have used std::copy for this but I cannot cast my dynamic
	// array pointers to c arrays and by extension cannot give std::copy
	// iterators to prevent it from complaining about the use of pointers being
	// unsafe.
	for (MIPSUWord i = 0; i < WORD_CAPACITY; i++) {

		toArray[i] = m_dataWords[i];

	}

}

//****************************************************************************
std::string MIPSRAM::generateAccessViolationText (const MIPSUWord address, const bool isWord) const {

	// Throwing the exception could itself be a method call; however, 
	// the debugger would show the error as originating from the
	// exception print method instead of what called it.

	std::stringstream outputStream;
	outputStream << MSG_ACCESS_VIOLATION_HEAD;

	if (isWord) {

		outputStream << MSG_ACCESS_VIOLATION_WORD;

	} else {

		outputStream << MSG_ACCESS_VIOLATION_BYTE;

	}

	outputStream << MSG_ACCESS_VIOLATION_ADDRESS << std::hex << address << MSG_ACCESS_VIOLATION_IS_OUT_OF_BOUNDS;

	return outputStream.str ();

}

//****************************************************************************
void MIPSRAM::allocateRAM (void) {

	if (WORD_CAPACITY > 0) {

		m_dataWords = new MIPSUWord[WORD_CAPACITY];

	} else {

		m_dataWords = nullptr;
		throw std::length_error (MSG_INSUFFICIENT_CAPACITY);

	}

}

//****************************************************************************
void MIPSRAM::runUnitTests (void) {

	const MIPSWord WORD_CAPACITY = 1048576; // 2^20 words.
	const MIPSWord BYTE_CAPACITY = WORD_CAPACITY * 4;

	const MIPSWord TEST_ADDRESS = 0x00002000;
	const MIPSWord TEST_VALUE_WORD = 0x000000FF;
	const MIPSWord TEST_VALUE_BYTE = 0x00000003;

	std::cout << "Begining MIPSRAM Unit Tests" << std::endl;

	// Ctor ******************************************************************
	MIPSRAM ram (WORD_CAPACITY);
	
	// Copy Ctor *************************************************************
	MIPSRAM ram2 (ram);

	// getWordCapacity *******************************************************
	if (ram.getWordCapacity () != WORD_CAPACITY) { std::cout << "getWordCapacity failed." << std::endl; }
	if (ram2.getWordCapacity () != WORD_CAPACITY) { std::cout << "getWordCapacity failed." << std::endl; }

	// getByteCapacity *******************************************************
	if (ram.getByteCapacity () != BYTE_CAPACITY) { std::cout << "getByteCapacity failed." << std::endl; }
	if (ram2.getByteCapacity () != BYTE_CAPACITY) { std::cout << "getByteCapacity failed." << std::endl; }

	// setWord/getWord *******************************************************
	ram.setWord (TEST_ADDRESS, TEST_VALUE_WORD);
	if (ram.getWord (TEST_ADDRESS) != TEST_VALUE_WORD) { std::cout << "getWord/setWord failed." << std::endl; }
	if (ram2.getWord (TEST_ADDRESS) != 0x00000000) { std::cout << "copy ctor failed." << std::endl; }

	// setByte/getByte *******************************************************
	ram.setByte (TEST_ADDRESS, TEST_VALUE_BYTE);
	if (static_cast<MIPSWord> (ram.getByte (TEST_ADDRESS)) == ram.getWord (TEST_ADDRESS)) { std::cout << "getByte/setByte/getWord/setWord failed." << std::endl; }
	if (ram.getByte (TEST_ADDRESS) != TEST_VALUE_BYTE) { std::cout << "getByte/setByte failed." << std::endl; }
	if (ram2.getByte (TEST_ADDRESS) != 0x00000000) { std::cout << "copy ctor failed." << std::endl; }

	// isValidAddress ********************************************************
	if (!ram.isValidWordAddress (TEST_ADDRESS)) { std::cout << "isValidWordAddress failed." << std::endl; }
	if (!ram.isValidByteAddress (TEST_ADDRESS)) { std::cout << "isValidByteAddress failed." << std::endl; }
	if (ram.isValidWordAddress (0xFFFFFFFF)) { std::cout << "isValidWordAddress failed." << std::endl; }
	if (ram.isValidByteAddress (0xFFFFFFFF)) { std::cout << "isValidByteAddress failed." << std::endl; }

	// Assignment ************************************************************
	MIPSRAM ram3 (WORD_CAPACITY);
	ram3 = ram;
	if (ram3.getWord (TEST_ADDRESS) != ram.getWord (TEST_ADDRESS)) { std::cout << "assignment op failed." << std::endl; }
	ram.setWord (0x00000000, 0x00000001);
	if (ram3.getWord (0x00000000) == 0x00000001) { std::cout << "assignment op failed." << std::endl; }

	// Verbose ***************************************************************
	try { ram.getWord (0xFFFFFFFF); std::cout << "out_of_bound failed." << std::endl; } catch (...) {  }
	try { ram.setWord (0xFFFFFFFF, TEST_VALUE_WORD); std::cout << "out_of_bound failed." << std::endl; } catch (...) {  }
	try { ram3.getWord (0xFFFFFFFF); std::cout << "out_of_bound failed." << std::endl; } catch (...) {  }

	std::cout << "MIPSRAM Unit Tests Complete" << std::endl;

}