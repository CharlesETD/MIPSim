/**
 * Author:		Charles Duncan (CharlesETD@gmail.com)
 * Created:		Jan 31, 2016
 * Compiler:	MSVS Express 2012
 * OS:			Windows 7 Home Premium SP1 x64
 *
 * © Copyright 2016 Charles Duncan (CharlesETD@gmail.com)
 */

// Includes ******************************************************************
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include "MIPSCPU.h"
#include "MIPSRAM.h"
#include "MIPSSystem.h"
#include "Registers.h"

// Static Constant Definitions ***********************************************
const std::string & MIPSSystem::MSG_CANNOT_TICK		= "Runtime Error at MIPSSystem::tick: Cannot tick exited program.";
const std::string & MIPSSystem::MSG_DEBUG_CONTROLS	= "<enter> to continue, <$0-31> to read register, <0x...> to read RAM.";

// Method Definitions ********************************************************
MIPSSystem::MIPSSystem (const MIPSUWord capacity, const bool debugMode) : MEMORY_CAPACITY (capacity), m_isDebugMode (debugMode), m_ram (MEMORY_CAPACITY), m_cpu (m_ram), m_isRunning (false) {

	

}

//****************************************************************************
bool MIPSSystem::isRunning (void) const {

	return m_isRunning;

}

//****************************************************************************
void MIPSSystem::tick (void) {

	if (!m_isRunning) {

		throw std::runtime_error (MSG_CANNOT_TICK);
		return;

	}

	if (m_isDebugMode) {

		handleDebugMode ();

	}

	m_cpu.tick ();

	if (m_cpu.systemCallRequested ()) {

		processSystemCall ();
		m_cpu.handleSystemCall ();

	}

}

//****************************************************************************
void MIPSSystem::reset (void) {

	m_ram.reset ();
	m_cpu.reset ();
	//m_isRunning = true;
	m_isRunning = false;

}

//****************************************************************************
bool MIPSSystem::loadROM (const std::string& filename) {

	std::ifstream file;
	std::string chunk;
	bool writeToPC = false;
	MIPSUWord newPC = 0;
	bool writeToRegister = false;
	MIPSUWord address = 0;
	MIPSUWord value = 0;
	UByte registerIndex = 0;

	reset ();

	try {

		file.open (filename);
		
		if (!file.good () || !file.is_open ()) {

			return false;

		}

		while (!file.eof ()) {

			file >> chunk;

			if (chunk.length () >= 4) {

				if (chunk[0] == '[') {

					if (chunk[1] == 'P') {

						writeToPC = true;

					} else if (chunk[1] == 'R') {

						chunk = chunk.substr (2, chunk.length () - 3);
						registerIndex = static_cast<UByte> (std::stoul (chunk));
						writeToRegister = true;

					} else {

						writeToPC = false;
						writeToRegister = false;
						chunk = chunk.substr (3, chunk.length () - 4);
						address = static_cast<MIPSUWord> (std::stoul (chunk, nullptr, 16));

					}

				} else if (chunk[0] == '0') {

					value = std::stoul (chunk, nullptr, 16);

					if (writeToPC) {

						newPC = value;
						writeToPC = false;

					} else if (writeToRegister) {

						m_cpu.setRegister (registerIndex, value);
						writeToRegister = false;

					} else {

						m_ram.setWord (address / 4, value);
						address += 4;

					}

				} else {

					if (!file.eof ()) {

						std::getline (file, chunk);

					}

				}

			} else {

				if (!file.eof ()) {

					std::getline (file, chunk);

				}

			}

		}

	} catch (...) {

		return false;

	}

	m_cpu.setPC (newPC);
	m_cpu.setNPC (newPC + 4);
	m_isRunning = true;
	return true;

}

//****************************************************************************
MIPSSystem::MIPSSystem (const MIPSSystem& mipsSystem) : MEMORY_CAPACITY (mipsSystem.MEMORY_CAPACITY), m_isDebugMode (mipsSystem.m_isDebugMode), m_ram (mipsSystem.m_ram), m_cpu (m_ram), m_isRunning (mipsSystem.m_isRunning) {

	// Dummy.

}

//****************************************************************************
MIPSSystem& MIPSSystem::operator= (const MIPSSystem& mipsSystem) {

	// Dummy.
	return *this;

}

//****************************************************************************
void MIPSSystem::sysCallPrintInteger (void) {

	MIPSUWord integer = m_cpu.getRegister (Register::A0);
	MIPSWord signedInteger = *reinterpret_cast<MIPSWord *> (&integer);

	std::cout << std::dec << signedInteger;

}

//****************************************************************************
void MIPSSystem::sysCallPrintString (void) {

	MIPSUWord startAddress = m_cpu.getRegister (Register::A0);
	char character = m_ram.getByte (startAddress);

	while (character != '\0') {

		std::cout << character;
		startAddress++;
		character = m_ram.getByte (startAddress);

	}

}

//****************************************************************************
void MIPSSystem::sysCallPrintChar (void) {

	char character = static_cast<char> (m_cpu.getRegister (Register::A0));

	std::cout << character;

}

//****************************************************************************
void MIPSSystem::sysCallReadInteger (void) {

	MIPSWord integer = 0;
	MIPSUWord unsignedIntger = 0;

	try {

		std::cin >> integer;
		std::cin.ignore ();

	} catch (...) {

		integer = 0;

	}

	unsignedIntger = *reinterpret_cast<MIPSUWord *> (&integer);
	m_cpu.setRegister (Register::V0, unsignedIntger);

}

//****************************************************************************
void MIPSSystem::sysCallReadString (void) {

	std::string input;
	MIPSUWord startIndex = m_cpu.getRegister (Register::A0);
	MIPSUWord endIndex = m_cpu.getRegister (Register::A1);
	MIPSUWord address = startIndex;
	MIPSUWord endAddress = address + endIndex;

	std::getline (std::cin, input);

	// Clear space.
	for (; address < endAddress; address++) {

		m_ram.setByte (address, static_cast<UByte> ('\0'));

	}

	// Read string.
	for (unsigned int i = 0; i < input.length (); i++) {

		address = startIndex + i;

		if (address < endAddress) {

			m_ram.setByte (address, static_cast<UByte> (input[i]));

		} else {

			m_ram.setByte (address, static_cast<UByte> ('\0'));
			break;

		}

	}

}

//****************************************************************************
void MIPSSystem::sysCallAllocateMemory (void) {

	MIPSUWord globalPointer = m_cpu.getRegister (Register::GP);
	MIPSUWord bytesToAllocate = m_cpu.getRegister (Register::A0);

	m_cpu.setRegister (Register::V0, globalPointer);
	globalPointer += bytesToAllocate;
	m_cpu.setRegister (Register::GP, globalPointer);

}

//****************************************************************************
void MIPSSystem::processSystemCall (void) {

	switch (m_cpu.getSystemCall ()) {

	case 1:
		sysCallPrintInteger ();
		break;

	case 4:
		sysCallPrintString ();
		break;

	case 5:
		sysCallReadInteger ();
		break;

	case 8:
		sysCallReadString ();
		break;

	case 9:
		sysCallAllocateMemory ();
		break;

	case 11:
		sysCallPrintChar ();
		break;

	case 10:
		m_isRunning = false;
		break;

	}

}

//****************************************************************************
void MIPSSystem::handleDebugMode (void) {

	std::string input;
	bool continueStep = false;
	MIPSUWord valueWord = 0;
	MIPSUWord valueByte = 0;
	MIPSUWord wordAddress = 0;
	MIPSUWord wordAddressEnd = 0;
	MIPSUWord byteAddress = 0;
	UByte registerIndex = 0;

	std::cout << std::hex << "PC[0x" << m_cpu.getPC () << "] IN[0x" << m_cpu.getInstruction () << "]" << std::endl;
	std::cout << MSG_DEBUG_CONTROLS << std::endl;

	while (!continueStep) {

		std::getline (std::cin, input);

		if (input.length () == 0) {

			continueStep = true;

		} else if (input[0] == '$') {

			try {

				registerIndex = static_cast<UByte> (std::stoul (input.substr (1, input.length ())));

				if (registerIndex < NUM_REGISTERS) {

					valueWord = m_cpu.getRegister (registerIndex);
					std::cout << input << "=0x" << std::hex << valueWord << " (" << std::dec << valueWord << ")" << std::endl;

				} else {

					std::cout << input << " is not a valid register." << std::endl;

				}

			} catch (...) {

				std::cout << input << " is not a valid register." << std::endl;

			}

		} else if (input[0] == '0') {

			// Divide by 4 so that a byte address references a word. Byte is stored as a word
			// to prevent autocasting to a character.
			try {

				byteAddress = static_cast<MIPSUWord> (std::stoul (input, nullptr, 16));

				if (m_ram.isValidByteAddress (byteAddress)) {

					// If the byte address is valid then the word address is as well.
					valueByte = m_ram.getByte (byteAddress);
					valueWord = m_ram.getWord (byteAddress / 4);

					// These are byte addresses to the word not word addresses to the word.
					wordAddress = (byteAddress / 4) * 4;
					wordAddressEnd = wordAddress + 3;

					std::cout << "0x" << std::hex << std::uppercase << byteAddress << "=0x" << valueByte << " (" << std::dec << valueByte << ")" << std::endl;
					std::cout << "0x" << std::hex << std::uppercase << wordAddress << "-0x" << wordAddressEnd << "=0x" << valueWord << " (" << std::dec << valueWord << ")" << std::endl;

				} else {

					std::cout << "0x" << std::hex << std::uppercase << byteAddress << " is out of memory." << std::endl;

				}

			} catch (...) {

				std::cout << input << " is not a valid memory address." << std::endl;

			}

		} else {

			std::cout << "Unrecognized command." << std::endl;
			std::cout << MSG_DEBUG_CONTROLS << std::endl;

		}

	}

}

//****************************************************************************
void MIPSSystem::runUnitTests (void) {

	std::cout << "Begining MIPSSystem Unit Tests" << std::endl;
	// This class is too complex for standard unit testing.
	// This method is only included for ease of use if
	// unit tests are added in the future.
	std::cout << "MIPSSystem Unit Tests Complete" << std::endl;

}