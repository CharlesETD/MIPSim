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
#include <stdexcept>
#include <string>
#include "Byte.h"
#include "MIPSCPU.h"
#include "MIPSRAM.h"
#include "MIPSShort.h"
#include "MIPSWord.h"
#include "Registers.h"

// Constants *****************************************************************
const std::string& MIPSCPU::MSG_OUT_OF_BOUNDS				= "Access Violation ad MIPSCPU::getRgister/setRegister: Given registerNumber is not a valid register.";
const std::string& MIPSCPU::MSG_FAILED_SYSTEM_CALL			= "Runtime Error at MIPSCPU::tick/operationSYSCALL: A system call was not handled.";
const std::string& MIPSCPU::MSG_UNREQUESTED_SYSTEM_CALL		= "Runtime Error at MIPSCPU::handleSystemCall: A system call was handled but not requested.";
const std::string& MIPSCPU::MSG_UNSUPORTED_OP				= "Runtime Error at MIPSCPU::?: An unsuported op code was encountered.";

const MIPSUWord MIPSCPU::S_INDEX_BITMASK					= 0x03E00000;
const MIPSUWord MIPSCPU::T_INDEX_BITMASK					= 0x001F0000;
const MIPSUWord MIPSCPU::D_INDEX_BITMASK					= 0x0000F800;
const MIPSUWord MIPSCPU::SHIFT_AMOUNT_BITMASK				= 0x000007C0;
const MIPSUWord MIPSCPU::IMMEDIATE_BITMASK					= 0x0000FFFF;
const MIPSUWord MIPSCPU::TARGET_BITMASK						= 0x03FFFFFF;
const MIPSUWord MIPSCPU::S_SHIFT_AMOUNT						= 21;
const MIPSUWord MIPSCPU::T_SHIFT_AMOUNT						= 16;
const MIPSUWord MIPSCPU::D_SHIFT_AMOUNT						= 11;
const MIPSUWord MIPSCPU::SHIFT_AMOUNT_SHIFT_AMOUNT			= 6;
const MIPSUWord MIPSCPU::IMMEDIATE_SHIFT_AMOUNT				= 0;
const MIPSUWord MIPSCPU::TARGET_SHIFT_AMOUNT				= 0;

// Thank you Logitec macro keyboards, this only took a couple minutes to type!
// 0 = bit must be zero.
// 1 = bit must be one.
// s = s register index mask
// t = t register index mask
// d = d register index mask
// i = immediate/target value mask.
// - = bit can be any value.
const std::string & MIPSCPU::TEMPLATE_ADD		= "000000ssssstttttddddd00000100000";
const std::string & MIPSCPU::TEMPLATE_ADDI		= "001000ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_ADDIU		= "001001ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_ADDU		= "000000ssssstttttddddd00000100001";
const std::string & MIPSCPU::TEMPLATE_AND		= "000000ssssstttttddddd00000100100";
const std::string & MIPSCPU::TEMPLATE_ANDI		= "001100ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_BEQ		= "000100ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_BGEZ		= "000001sssss00001iiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_BGEZAL	= "000001sssss10001iiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_BGTZ		= "000111sssss00000iiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_BLEZ		= "000110sssss00000iiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_BLTZ		= "000001sssss00000iiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_BLTZAL	= "000001sssss10000iiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_BNE		= "000101ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_DIV		= "000000sssssttttt0000000000011010";
const std::string & MIPSCPU::TEMPLATE_J			= "000010iiiiiiiiiiiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_JAL		= "000011iiiiiiiiiiiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_JR		= "000000sssss000000000000000001000";
const std::string & MIPSCPU::TEMPLATE_LB		= "100000ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_LUI		= "001111-----tttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_LW		= "100011ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_MFHI		= "0000000000000000ddddd00000010000";
const std::string & MIPSCPU::TEMPLATE_MFLO		= "0000000000000000ddddd00000010010";
const std::string & MIPSCPU::TEMPLATE_MULT		= "000000sssssttttt0000000000011000";
const std::string & MIPSCPU::TEMPLATE_OR		= "000000ssssstttttddddd00000100101";
const std::string & MIPSCPU::TEMPLATE_ORI		= "001101ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_SLL		= "000000ssssstttttdddddhhhhh000000";
const std::string & MIPSCPU::TEMPLATE_SLLV		= "000000ssssstttttddddd-----000100";
const std::string & MIPSCPU::TEMPLATE_SLT		= "000000ssssstttttddddd00000101010";
const std::string & MIPSCPU::TEMPLATE_SLTI		= "001010ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_SLTIU		= "001011ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_SLTU		= "000000ssssstttttddddd00000101011";
const std::string & MIPSCPU::TEMPLATE_SRA		= "000000-----tttttdddddhhhhh000011";
const std::string & MIPSCPU::TEMPLATE_SRL		= "000000-----tttttdddddhhhhh000010";
const std::string & MIPSCPU::TEMPLATE_SRLV		= "000000ssssstttttddddd00000000110";
const std::string & MIPSCPU::TEMPLATE_SUB		= "000000ssssstttttddddd00000100010";
const std::string & MIPSCPU::TEMPLATE_SUBU		= "000000ssssstttttddddd00000100011";
const std::string & MIPSCPU::TEMPLATE_SW		= "101011ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_SB		= "101000ssssstttttiiiiiiiiiiiiiiii";
const std::string & MIPSCPU::TEMPLATE_SYSCALL	= "000000--------------------001100";
const std::string & MIPSCPU::TEMPLATE_XOR		= "000000ssssstttttddddd-----100110";
const std::string & MIPSCPU::TEMPLATE_XORI		= "001110ssssstttttiiiiiiiiiiiiiiii";

//****************************************************************************
MIPSCPU::MIPSCPU (MIPSRAM& ramModule) : m_ram (ramModule), m_instruction (0), m_nextProgramCounter (0), m_programCounter (0), m_requestedSystemCall (false), m_resultLow (), m_resultHigh (0) {

	std::fill (m_generalRegisters, m_generalRegisters + NUM_REGISTERS, 0);

	reset ();

}

//****************************************************************************
MIPSUWord MIPSCPU::getRegister (const UByte registerNumber) const {

	if (registerNumber >= NUM_REGISTERS) {

		throw std::out_of_range (MSG_OUT_OF_BOUNDS);
		return 0;

	}

	return m_generalRegisters[registerNumber];

}

//****************************************************************************
MIPSUWord MIPSCPU::getRegister (const Register registerID) const {

	return getRegister (static_cast<UByte> (registerID));

}

//****************************************************************************
MIPSUWord MIPSCPU::getPC (void) const {

	return m_programCounter;

}

//****************************************************************************
MIPSUWord MIPSCPU::getNPC (void) const {

	return m_nextProgramCounter;

}

//****************************************************************************
MIPSUWord MIPSCPU::getLO (void) const {

	return m_resultLow;

}

//****************************************************************************
MIPSUWord MIPSCPU::getHI (void) const {

	return m_resultHigh;

}

//****************************************************************************
void MIPSCPU::setRegister (const UByte registerNumber, const MIPSUWord value) {

	if (registerNumber >= NUM_REGISTERS) {

		throw std::out_of_range (MSG_OUT_OF_BOUNDS);
		return;

	} else if (registerNumber == Register::ZERO) {

		// The zero register cannot be modified.
		return;

	}

	m_generalRegisters[registerNumber] = value;

}

//****************************************************************************
void MIPSCPU::setPC (const MIPSUWord value) {

	m_programCounter = value;

	// We need to divide by 4 (left shift 2) to get the first byte at the address.
	m_instruction = m_ram.getWord (m_programCounter >> 2);

}

//****************************************************************************
void MIPSCPU::setNPC (const MIPSUWord value) {

	m_nextProgramCounter = value;

}

//****************************************************************************
void MIPSCPU::setLO (const MIPSUWord value) {

	m_resultLow = value;

}

//****************************************************************************
void MIPSCPU::setHI (const MIPSUWord value) {

	m_resultHigh = value;

}

//****************************************************************************
MIPSUWord MIPSCPU::getInstruction (void) const {

	return m_instruction;

}

//****************************************************************************
bool MIPSCPU::systemCallRequested (void) const {

	return m_requestedSystemCall;

}

//****************************************************************************
UByte MIPSCPU::getSystemCall (void) const {

	return getRegister (Register::V0);

}

//****************************************************************************
void MIPSCPU::handleSystemCall (void) {

	if (m_requestedSystemCall) {

		m_requestedSystemCall = false;

	} else {

		m_requestedSystemCall = false;
		throw std::runtime_error (MSG_UNREQUESTED_SYSTEM_CALL);

	}

}

//****************************************************************************
void MIPSCPU::tick (void) {

	// We need to divide by 4 (left shift 2) to get the first byte at the address.
	m_instruction = m_ram.getWord (m_programCounter >> 2);

	if (m_requestedSystemCall) {

		throw std::runtime_error (MSG_FAILED_SYSTEM_CALL);
		return;

	}

	processInstruction ();

}

//****************************************************************************
void MIPSCPU::reset (void) {

	m_requestedSystemCall = false;
	
	// Reset general registers.
	std::fill (m_generalRegisters, m_generalRegisters + NUM_REGISTERS, 0);

	// This sets both the pc and npc to zero. We do this so that the tick 
	// will still start at the first instruction even after advancing.
	// We want to ensure getting the instruction after a reset and before a
	// tick still yields a valid instruction.
	m_programCounter = 0;
	m_nextProgramCounter = 0;

	// We need to divide by 4 (left shift 2) to get the first byte at the address.
	m_instruction = m_ram.getWord (m_programCounter >> 2);

	m_resultLow = 0;
	m_resultHigh = 0;

}

//****************************************************************************
MIPSCPU::MIPSCPU (const MIPSCPU& mipsCPU) : m_ram (mipsCPU.m_ram), m_instruction (mipsCPU.m_instruction), m_nextProgramCounter (mipsCPU.m_nextProgramCounter), m_programCounter (mipsCPU.m_programCounter), m_requestedSystemCall (mipsCPU.m_requestedSystemCall), m_resultLow (mipsCPU.m_resultLow), m_resultHigh (mipsCPU.m_resultHigh) {

	for (unsigned int i = 0; i < NUM_REGISTERS; i++) {

		m_generalRegisters[i] = mipsCPU.m_generalRegisters[i];

	}

}

//****************************************************************************
MIPSCPU & MIPSCPU::operator= (const MIPSCPU& mipsCPU) {

	// Dummy!
	return *this;

}

//****************************************************************************
void MIPSCPU::advancePC (const MIPSWord offset) {

	m_programCounter = m_nextProgramCounter;
	m_nextProgramCounter += offset;

	// We need to divide by 4 (left shift 2) to get the first byte at the address.
	m_instruction = m_ram.getWord (m_programCounter >> 2);

}

//****************************************************************************
MIPSUWord MIPSCPU::maskAndShift (const MIPSUWord value, const MIPSUWord bitmask, const MIPSUWord shiftAmount) const {

	return (value & bitmask) >> shiftAmount;

}

//****************************************************************************
UByte MIPSCPU::getSIndex (void) const {

	return static_cast<UByte> (maskAndShift (m_instruction, S_INDEX_BITMASK, S_SHIFT_AMOUNT));

}

//****************************************************************************
UByte MIPSCPU::getTIndex (void) const {

	return static_cast<UByte> (maskAndShift (m_instruction, T_INDEX_BITMASK, T_SHIFT_AMOUNT));

}

//****************************************************************************
UByte MIPSCPU::getDIndex (void) const {

	return static_cast<UByte> (maskAndShift (m_instruction, D_INDEX_BITMASK, D_SHIFT_AMOUNT));

}

//****************************************************************************
MIPSUShort MIPSCPU::getImmediate (void) const {

	return static_cast<MIPSUShort> (maskAndShift (m_instruction, IMMEDIATE_BITMASK, IMMEDIATE_SHIFT_AMOUNT));

}

//****************************************************************************
MIPSWord MIPSCPU::getTarget (void) const {

	// The remaining bits after a target all set to ones.
	static const MIPSUWord SIGN_EXTENSION = 0xFC000000;

	MIPSUWord unsignedTarget = maskAndShift (m_instruction, TARGET_BITMASK, TARGET_SHIFT_AMOUNT);

	// Is the high bit of the target a one? If so then sign extend it as a negative number.
	if (((unsignedTarget >> 25) & 1) == 1) {

		// Or-ing will set all upper bits to ones as a sign extension.
		unsignedTarget = unsignedTarget | SIGN_EXTENSION;

	}

	return asSigned (unsignedTarget);

}

//****************************************************************************
UByte MIPSCPU::getShift (void) const {

	return static_cast<UByte> (maskAndShift (m_instruction, SHIFT_AMOUNT_BITMASK, SHIFT_AMOUNT_SHIFT_AMOUNT));

}

//****************************************************************************
void MIPSCPU::processInstruction (void) {

	// Opcode for NOP.
	static const MIPSUWord NOP = 0;

	// Once again macro keyboard saves typing, this only took 60 seconds or so to write the entire body.
	if (m_instruction == NOP) {

		// No OP
		advancePC (4);

	} else if (isInstruction (TEMPLATE_ADD)) {

		operationADD ();

	} else if (isInstruction (TEMPLATE_ADDI)) {

		operationADDI ();

	} else if (isInstruction (TEMPLATE_ADDIU)) {

		operationADDIU ();

	} else if (isInstruction (TEMPLATE_ADDU)) {

		operationADDU ();

	} else if (isInstruction (TEMPLATE_AND)) {

		operationAND ();

	} else if (isInstruction (TEMPLATE_ANDI)) {

		operationANDI ();

	} else if (isInstruction (TEMPLATE_BEQ)) {

		operationBEQ ();

	} else if (isInstruction (TEMPLATE_BGEZ)) {

		operationBGEZ ();

	} else if (isInstruction (TEMPLATE_BGEZAL)) {

		operationBGEZAL ();

	} else if (isInstruction (TEMPLATE_BGTZ)) {

		operationBGTZ ();

	} else if (isInstruction (TEMPLATE_BLEZ)) {

		operationBLEZ ();

	} else if (isInstruction (TEMPLATE_BLTZ)) {

		operationBLTZ ();

	} else if (isInstruction (TEMPLATE_BLTZAL)) {

		operationBLTZAL ();

	} else if (isInstruction (TEMPLATE_BNE)) {

		operationBNE ();

	} else if (isInstruction (TEMPLATE_DIV)) {

		operationDIV ();

	} else if (isInstruction (TEMPLATE_J)) {

		operationJ ();

	} else if (isInstruction (TEMPLATE_JAL)) {

		operationJAL ();

	} else if (isInstruction (TEMPLATE_JR)) {

		operationJR ();

	} else if (isInstruction (TEMPLATE_LB)) {

		operationLB ();

	} else if (isInstruction (TEMPLATE_LUI)) {

		operationLUI ();

	} else if (isInstruction (TEMPLATE_LW)) {

		operationLW ();

	} else if (isInstruction (TEMPLATE_MFHI)) {

		operationMFHI ();

	} else if (isInstruction (TEMPLATE_MFLO)) {

		operationMFLO ();

	} else if (isInstruction (TEMPLATE_MULT)) {

		operationMULT ();

	} else if (isInstruction (TEMPLATE_OR)) {

		operationOR ();

	} else if (isInstruction (TEMPLATE_ORI)) {

		operationORI ();

	} else if (isInstruction (TEMPLATE_SLL)) {

		operationSLL ();

	} else if (isInstruction (TEMPLATE_SLLV)) {

		operationSLLV ();

	} else if (isInstruction (TEMPLATE_SLT)) {

		operationSLT ();

	} else if (isInstruction (TEMPLATE_SLTI)) {

		operationSLTI ();

	} else if (isInstruction (TEMPLATE_SLTIU)) {

		operationSLTIU ();

	} else if (isInstruction (TEMPLATE_SLTU)) {

		operationSLTU ();

	} else if (isInstruction (TEMPLATE_SRA)) {

		operationSRA ();

	} else if (isInstruction (TEMPLATE_SRL)) {

		operationSRL ();

	} else if (isInstruction (TEMPLATE_SRLV)) {

		operationSRLV ();

	} else if (isInstruction (TEMPLATE_SUB)) {

		operationSUB ();

	} else if (isInstruction (TEMPLATE_SUBU)) {

		operationSUBU ();

	} else if (isInstruction (TEMPLATE_SW)) {

		operationSW ();

	} else if (isInstruction (TEMPLATE_SB)) {

		operationSB ();

	} else if (isInstruction (TEMPLATE_SYSCALL)) {

		operationSYSCALL ();

	} else if (isInstruction (TEMPLATE_XOR)) {

		operationXOR ();

	} else if (isInstruction (TEMPLATE_XORI)) {

		operationXORI ();

	} else {

		throw new std::runtime_error (MSG_UNSUPORTED_OP);

	}

}

//****************************************************************************
void MIPSCPU::operationADD (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord d = s + t;
	setRegister (getDIndex (), d);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationADDI (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord i = signedExtendShort (getImmediate ());
	MIPSUWord t = s + i;
	setRegister (getTIndex (), t);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationADDIU (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord i = signedExtendShort (getImmediate ());
	MIPSUWord t = s + i;

	setRegister (getTIndex (), t);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationADDU (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord d = s + t;
	setRegister (getDIndex (), d);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationAND (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord d = s & t;
	setRegister (getDIndex (), d);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationANDI (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord i = signedExtendShort (getImmediate ());
	MIPSUWord t = s & i;
	setRegister (getTIndex (), t);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationBEQ (void) {

	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());

	if (s == t) {

		advancePC (offset << 2);

	} else {

		advancePC (4);

	}

}

//****************************************************************************
void MIPSCPU::operationBGEZ (void) {

	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSWord s = asSigned (getRegister (getSIndex ()));

	if (s >= 0) {

		advancePC (offset << 2);

	} else {

		advancePC (4);

	}

}

//****************************************************************************
void MIPSCPU::operationBGEZAL (void) {

	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSWord s = asSigned (getRegister (getSIndex ()));

	if (s >= 0) {

		advancePC (offset << 2);
		setRegister (Register::RA, getPC () + 8);

	} else {

		advancePC (4);

	}

}

//****************************************************************************
void MIPSCPU::operationBGTZ (void) {

	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSWord s = asSigned (getRegister (getSIndex ()));

	if (s > 0) {

		advancePC (offset << 2);

	} else {

		advancePC (4);

	}

}

//****************************************************************************
void MIPSCPU::operationBLEZ (void) {

	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSWord s = asSigned (getRegister (getSIndex ()));

	if (s <= 0) {

		advancePC (offset << 2);

	} else {

		advancePC (4);

	}

}

//****************************************************************************
void MIPSCPU::operationBLTZ (void) {

	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSWord s = asSigned (getRegister (getSIndex ()));

	if (s < 0) {

		advancePC (offset << 2);

	} else {

		advancePC (4);

	}

}

//****************************************************************************
void MIPSCPU::operationBLTZAL (void) {

	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSWord s = asSigned (getRegister (getSIndex ()));

	if (s < 0) {

		advancePC (offset << 2);
		setRegister (Register::RA, getPC () + 8);

	} else {

		advancePC (4);

	}

}

//****************************************************************************
void MIPSCPU::operationBNE (void) {

	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());

	if (s != t) {

		advancePC (offset << 2);

	} else {

		advancePC (4);

	}

}

//****************************************************************************
void MIPSCPU::operationDIV (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord q = s / t;
	MIPSUWord r = s % t;
	setLO (q);
	setHI (r);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationJ (void) {

	m_programCounter = m_nextProgramCounter;
	m_nextProgramCounter = ((m_programCounter & 0xF0000000) | (getTarget () << 2));

	// We need to divide by 4 (left shift 2) to get the first byte at the address.
	m_instruction = m_ram.getWord (m_programCounter >> 2);

}

//****************************************************************************
void MIPSCPU::operationJAL (void) {

	setRegister (Register::RA, getPC () + 8);

	m_programCounter = m_nextProgramCounter;
	m_nextProgramCounter = ((m_programCounter & 0xF0000000) | (getTarget () << 2));

	// We need to divide by 4 (left shift 2) to get the first byte at the address.
	m_instruction = m_ram.getWord (m_programCounter >> 2);

}

//****************************************************************************
void MIPSCPU::operationJR (void) {

	MIPSUWord s = getRegister (getSIndex ());
	m_programCounter = m_nextProgramCounter;
	m_nextProgramCounter = s;

	// We need to divide by 4 (left shift 2) to get the first byte at the address.
	m_instruction = m_ram.getWord (m_programCounter >> 2);

}

//****************************************************************************
void MIPSCPU::operationLB (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSUWord address = s + offset;
	setRegister (getTIndex (), m_ram.getByte (address));

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationLUI (void) {

	MIPSUWord i = signedExtendShort (getImmediate ());
	setRegister (getTIndex (), i << 16);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationLW (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSUWord address = s + offset;
	setRegister (getTIndex (), m_ram.getWord (address));

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationMFHI (void) {

	setRegister (getDIndex (), getHI ());

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationMFLO (void) {

	setRegister (getDIndex (), getLO ());

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationMULT (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord result = s * t;
	setLO (result);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationOR (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord d = s | t;
	setRegister (getDIndex (), d);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationORI (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord i = signedExtendShort (getImmediate ());
	MIPSUWord t = s | i;
	setRegister (getTIndex (), t);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSLL (void) {

	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord h = static_cast<MIPSUWord> (getShift ());
	MIPSUWord shifted = t << h;
	setRegister (getDIndex (), shifted);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSLLV (void) {

	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord shifted = t << s;
	setRegister (getDIndex (), shifted);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSLT (void) {

	MIPSWord t = asSigned (getRegister (getTIndex ()));
	MIPSWord s = asSigned (getRegister (getSIndex ()));
	
	if (s < t) {

		setRegister (getDIndex (), 1);

	} else {

		setRegister (getDIndex (), 0);

	}

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSLTI (void) {

	MIPSWord i = asSigned (signedExtendShort (getImmediate ()));
	MIPSWord s = asSigned (getRegister (getSIndex ()));
	
	if (s < i) {

		setRegister (getDIndex (), 1);

	} else {

		setRegister (getDIndex (), 0);

	}

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSLTIU (void) {

	MIPSUWord i = signedExtendShort (getImmediate ());
	MIPSUWord s = getRegister (getSIndex ());
	
	if (s < i) {

		setRegister (getDIndex (), 1);

	} else {

		setRegister (getDIndex (), 0);

	}

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSLTU (void) {

	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord s = getRegister (getSIndex ());
	
	if (s < t) {

		setRegister (getDIndex (), 1);

	} else {

		setRegister (getDIndex (), 0);

	}

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSRA (void) {

	MIPSWord t = asSigned (getRegister (getTIndex ()));
	MIPSUWord h = static_cast<MIPSUWord> (getShift ());
	MIPSWord shifted = t >> h;
	setRegister (getDIndex (), asUnsigned (shifted));

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSRL (void) {

	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord h = static_cast<MIPSUWord> (getShift ());
	MIPSUWord shifted = t >> h;
	setRegister (getDIndex (), shifted);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSRLV (void) {

	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord shifted = t >> s;
	setRegister (getDIndex (), shifted);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSUB (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord d = s - t;
	setRegister (getDIndex (), d);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSUBU (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord d = s - t;
	setRegister (getDIndex (), d);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSW (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSUWord address = s + offset;
	m_ram.setWord (address, getRegister (getTIndex ()));

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSB (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSWord offset = asSigned (signedExtendShort (getImmediate ()));
	MIPSUWord address = s + offset;
	m_ram.setByte (address, static_cast<UByte> (getRegister (getTIndex ())));

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationSYSCALL (void) {

	if (m_requestedSystemCall) {

		throw std::runtime_error (MSG_FAILED_SYSTEM_CALL);
		return;

	}

	m_requestedSystemCall = true;

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationXOR (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord t = getRegister (getTIndex ());
	MIPSUWord d = s ^ t;
	setRegister (getDIndex (), d);

	advancePC (4);

}

//****************************************************************************
void MIPSCPU::operationXORI (void) {

	MIPSUWord s = getRegister (getSIndex ());
	MIPSUWord i = signedExtendShort (getImmediate ());
	MIPSUWord t = s ^ i;
	setRegister (getTIndex (), t);

	advancePC (4);

}

//****************************************************************************
MIPSUWord MIPSCPU::signedExtendShort (const MIPSUShort value) const {

	MIPSShort signedValue = *reinterpret_cast<const MIPSShort*> (&value);
	MIPSWord signedWord = static_cast<MIPSWord> (signedValue);
	MIPSUWord unsignedWord = *reinterpret_cast<MIPSUWord*> (&signedWord);

	return unsignedWord;

}

//****************************************************************************
MIPSUWord MIPSCPU::unsignedExtendShort (const MIPSUShort value) const {

	MIPSUWord signExtended = static_cast<MIPSUWord> (value);

	return signExtended;

}

//****************************************************************************
MIPSWord MIPSCPU::asSigned (const MIPSUWord value) const {

	MIPSWord signedValue = *reinterpret_cast<const MIPSWord*> (&value);

	return signedValue;

}

//****************************************************************************
MIPSUWord MIPSCPU::asUnsigned (const MIPSWord value) const {

	MIPSUWord unsignedValue = *reinterpret_cast<const MIPSUWord*> (&value);

	return unsignedValue;

}

//****************************************************************************
bool MIPSCPU::isInstruction (const std::string& instructionTemplate) const {

	// How long is an opcode?
	static const unsigned int INSTRUCTION_LENGTH = 32;

	char c = '\0';
	MIPSUWord mask = 0;
	MIPSUWord compare = 0;

	if (instructionTemplate.length () != INSTRUCTION_LENGTH) {

		return false;

	}

	for (unsigned int i = 0; i < instructionTemplate.length (); i++) {

		c = instructionTemplate[i];

		compare = compare << 1;
		mask = mask << 1;

		if (c == '1' || c == '0') {

			if (c == '1') {

				compare = compare | 1;

			}

			mask = mask | 1;

		}

	}

	if ((m_instruction & mask) == compare) {

		return true;

	}

	return false;

}

//****************************************************************************
void MIPSCPU::runUnitTests (void) {

	std::cout << "Begining MIPSCPU Unit Tests" << std::endl;
	// This class is too complex for standard unit testing.
	// This method is only included for ease of use if
	// unit tests are added in the future.
	std::cout << "MIPSCPU Unit Tests Complete" << std::endl;

}