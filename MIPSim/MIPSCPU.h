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
#include "MIPSRAM.h"
#include "MIPSShort.h"
#include "Registers.h"

#pragma once
#ifndef MIPS_CPU_H
#define MIPS_CPU_H

/**
 * Defines the core processor of a MIPS simulator.
 */
class MIPSCPU {

private:
	// Private Static Constants **********************************************
	static const std::string& MSG_OUT_OF_BOUNDS;			/**< Exception message displayed when an out of range exception occurs. >*/
	static const std::string& MSG_FAILED_SYSTEM_CALL;		/**< Exception message displayed when a system call is not handled. >*/
	static const std::string& MSG_UNREQUESTED_SYSTEM_CALL;	/**< Exception message displayed when a system call is handled when not requested. >*/
	static const std::string& MSG_UNSUPORTED_OP;			/**< Exception message displayed when an unsuported op is parsed. >*/ 

	static const MIPSUWord S_INDEX_BITMASK;					/**< Bitmask for the S register ID in an opcode. >*/
	static const MIPSUWord T_INDEX_BITMASK;					/**< Bitmask for the T register ID in an opcode. >*/ /// 
	static const MIPSUWord D_INDEX_BITMASK;					/**< Bitmask for the D register ID in an opcode. >*/ /// 
	static const MIPSUWord SHIFT_AMOUNT_BITMASK;			/**< Bitmask for the shift ammount in an opcode. >*/ /// 
	static const MIPSUWord IMMEDIATE_BITMASK;				/**< Bitmask for the immediate value in an opcode. >*/ /// 
	static const MIPSUWord TARGET_BITMASK;					/**< Bitmask for the target address in an opcode. >*/ /// 
	static const MIPSUWord S_SHIFT_AMOUNT;					/**< How far the S_INDEX_BITMASK must be shifted for alignment. >*/ 
	static const MIPSUWord T_SHIFT_AMOUNT;					/**< How far the T_INDEX_BITMASK must be shifted for alignment. >*/ 
	static const MIPSUWord D_SHIFT_AMOUNT;					/**< How far the D_INDEX_BITMASK must be shifted for alignment. >*/ 
	static const MIPSUWord SHIFT_AMOUNT_SHIFT_AMOUNT;		/**< How far the SHIFT_AMOUNT_BITMASK must be shifted for alignment. >*/
	static const MIPSUWord IMMEDIATE_SHIFT_AMOUNT;			/**< How far the IMMEDIATE_BITMASK must be shifted for alignment. >*/ 
	static const MIPSUWord TARGET_SHIFT_AMOUNT;				/**< How far the TARGET_BITMASK must be shifted for alignment. >*/ 

	static const std::string& TEMPLATE_ADD;					/**< Template for the ADD opcode. >*/
	static const std::string& TEMPLATE_ADDI;				/**< Template for the ADDI opcode. >*/			
	static const std::string& TEMPLATE_ADDIU;				/**< Template for the ADDIU opcode. >*/
	static const std::string& TEMPLATE_ADDU;				/**< Template for the ADDU opcode. >*/
	static const std::string& TEMPLATE_AND;					/**< Template for the AND opcode. >*/
	static const std::string& TEMPLATE_ANDI;				/**< Template for the ANDI opcode. >*/
	static const std::string& TEMPLATE_BEQ;					/**< Template for the BEQ opcode. >*/
	static const std::string& TEMPLATE_BGEZ;				/**< Template for the BGEZ opcode. >*/
	static const std::string& TEMPLATE_BGEZAL;				/**< Template for the BGEZAL opcode. >*/
	static const std::string& TEMPLATE_BGTZ;				/**< Template for the BGTZ opcode. >*/
	static const std::string& TEMPLATE_BLEZ;				/**< Template for the BLEZ opcode. >*/
	static const std::string& TEMPLATE_BLTZ;				/**< Template for the BLTZ opcode. >*/
	static const std::string& TEMPLATE_BLTZAL;				/**< Template for the BLTZAL opcode. >*/
	static const std::string& TEMPLATE_BNE;					/**< Template for the BNE opcode. >*/
	static const std::string& TEMPLATE_DIV;					/**< Template for the DIV opcode. >*/
	static const std::string& TEMPLATE_J;					/**< Template for the J opcode. >*/
	static const std::string& TEMPLATE_JAL;					/**< Template for the JAL opcode. >*/
	static const std::string& TEMPLATE_JR;					/**< Template for the JR opcode. >*/
	static const std::string& TEMPLATE_LB;					/**< Template for the LB opcode. >*/
	static const std::string& TEMPLATE_LUI;					/**< Template for the LUI opcode. >*/
	static const std::string& TEMPLATE_LW;					/**< Template for the LW opcode. >*/
	static const std::string& TEMPLATE_MFHI;				/**< Template for the MFHI opcode. >*/
	static const std::string& TEMPLATE_MFLO;				/**< Template for the MFLO opcode. >*/
	static const std::string& TEMPLATE_MULT;				/**< Template for the MULT opcode. >*/
	static const std::string& TEMPLATE_OR;					/**< Template for the OR opcode. >*/
	static const std::string& TEMPLATE_ORI;					/**< Template for the ORI opcode. >*/
	static const std::string& TEMPLATE_SLL;					/**< Template for the SLL opcode. >*/
	static const std::string& TEMPLATE_SLLV;				/**< Template for the SLLV opcode. >*/
	static const std::string& TEMPLATE_SLT;					/**< Template for the SLT opcode. >*/
	static const std::string& TEMPLATE_SLTI;				/**< Template for the SLTI opcode. >*/
	static const std::string& TEMPLATE_SLTIU;				/**< Template for the SLTIU opcode. >*/
	static const std::string& TEMPLATE_SLTU;				/**< Template for the SLTU opcode. >*/
	static const std::string& TEMPLATE_SRA;					/**< Template for the SRA opcode. >*/
	static const std::string& TEMPLATE_SRL;					/**< Template for the SRL opcode. >*/
	static const std::string& TEMPLATE_SRLV;				/**< Template for the SRLV opcode. >*/
	static const std::string& TEMPLATE_SUB;					/**< Template for the SUB opcode. >*/
	static const std::string& TEMPLATE_SUBU;				/**< Template for the SUBU opcode. >*/
	static const std::string& TEMPLATE_SW;					/**< Template for the SW opcode. >*/
	static const std::string& TEMPLATE_SB;					/**< Template for the SB opcode. >*/
	static const std::string& TEMPLATE_SYSCALL;				/**< Template for the SYSCALL opcode. >*/
	static const std::string& TEMPLATE_XOR;					/**< Template for the XOR opcode. >*/
	static const std::string& TEMPLATE_XORI;				/**< Template for the XORI opcode. >*/

	// Private Variables *****************************************************
	MIPSRAM&	m_ram;										/**< Reference to the RAM module. >*/
	bool		m_requestedSystemCall;						/**< Was a SYSCALL operation made last tick? >*/
	MIPSUWord	m_generalRegisters[NUM_REGISTERS];			/**< The array of general purpose registers. >*/
	MIPSUWord	m_programCounter;							/**< The current program counter in bytes. >*/
	MIPSUWord	m_nextProgramCounter;						/**< The next program counter in bytes. >*/
	MIPSUWord	m_resultLow;								/**< The low result from a multiply. >*/
	MIPSUWord	m_resultHigh;								/**< The high result from a multiply. >*/
	MIPSUWord	m_instruction;								/**< The current instruction. >*/

public:
	// Public Ctor/Dtor ******************************************************
	/**
	 * Ctor.
	 * @param ramModule reference to the system RAM.
	 */
	MIPSCPU (MIPSRAM& ramModule);

	// Accessors *************************************************************
	/**
	 * Gets the unsigned value of the given register.
	 * @param registerNumber index of the register.
	 * @return value of the register unsigned.
	 * @throw out_of_range if registerNumber is invalid.
	 */
	MIPSUWord getRegister (const UByte registerNumber) const;

	/**
	 * Gets the unsigned value of the given register.
	 * @param registerID ID of the register.
	 * @return value of the register unsigned.
	 */
	MIPSUWord getRegister (const Register registerID) const;

	/**
	 * Gets the current program counter.
	 * @return value of the program counter.
	 */
	MIPSUWord getPC (void) const;

	/**
	 * Gets the next program counter.
	 * @return value of the next program counter.
	 */
	MIPSUWord getNPC (void) const;

	/**
	 * Gets the low result from a multiply.
	 * @return the low result from a multiply unsigned.
	 */
	MIPSUWord getLO (void) const;

	/**
	 * Gets the high result from a multiply.
	 * @return high result from a multiply unsigned.
	 */
	MIPSUWord getHI (void) const;

	/**
	 * Gets the current instruction.
	 * @return current instruction.
	 */
	MIPSUWord getInstruction (void) const;

	// Mutators **************************************************************
	/**
	 * Sets the value of the given register.
	 * @param registerNumber index of the register.
	 * @param value new value of the register.
	 * @throw out_of_range if registerNumber is invalid.
	 */
	void setRegister (const UByte registerNumber, const MIPSUWord value);

	/**
	 * Sets the value of the program counter.
	 * @param value new value of the program counter.
	 */
	void setPC (const MIPSUWord value);

	/**
	 * Sets the value of the next program counter.
	 * @param value new value of the next program counter.
	 */
	void setNPC (const MIPSUWord value);

	/**
	 * Sets the value of the multiply result low.
	 * @param value new value of the low multiply result.
	 */
	void setLO (const MIPSUWord value);

	/**
	 * Sets the value of the multiply result high.
	 * @param value new value of the high multiply result.
	 */
	void setHI (const MIPSUWord value);

	// Public Methods ********************************************************
	/**
	 * Returns if a system call was requested.
	 * @return true if a system call was requested, otherwise false.
	 */
	bool systemCallRequested (void) const;

	/**
	 * Gets the system call code.
	 * @return the system call code.
	 */
	UByte getSystemCall (void) const;

	/**
	 * Called to report that the last system call was handled.
	 * @throw runtime_error if a system call is handled when not requested.
	 */
	void handleSystemCall ();

	/**
	 * Updates the processor and performs one instruction.
	 * @throw runtime_error if a system call is not handled.
	 */
	void tick (void);

	/**
	 * Resets the processor to a known state.
	 */
	void reset (void);

	// Public Static Methods *************************************************
	/**
	 * Performs unit tests on MIPSCPU.
	 */
	static void runUnitTests (void);

private:
	// Private Ctor/Dtor *****************************************************
	/**
	 * Private copy ctor to prevent copying of RAM module.
	 * @param mipsCPU CPU to copy from.
	 */
	MIPSCPU (const MIPSCPU& mipsCPU);

	// Private Operators *****************************************************
	/**
	 * Private copy assignment operator to prevent copying of RAM module.
	 * @param mipsCPU CPU to copy from.
	 * @return reference to this CPU.
	 */
	MIPSCPU & operator= (const MIPSCPU& mipsCPU);

	// Private Methods *******************************************************
	/**
	 * Sets the next program counter to the program counter plus the offset.
	 * Also sets instruction to the next instruction.
	 * @param offset to advance program counter by in bytes.
	 */
	void advancePC (const MIPSWord offset);

	/**
	 * Returns the value bitmasked then shifted.
	 * @param value to mask and shift.
	 * @param bitmask to apply.
	 * @param shiftAmount.
	 * @return the value bitmasked then shifted.
	 */
	MIPSUWord maskAndShift (const MIPSUWord value, const MIPSUWord bitmask, const MIPSUWord shiftAmount) const;

	/**
	 * Gets the S register index from the current instruction.
	 * @return the S register index from the current instruction.
	 */
	UByte getSIndex (void) const;

	/**
	 * Gets the T register index from the current instruction.
	 * @return the T register index from the current instruction.
	 */
	UByte getTIndex (void) const;

	/**
	 * Gets the D register index from the current instruction.
	 * @return the D register index from the current instruction.
	 */
	UByte getDIndex (void) const;

	/**
	 * Gets the immediate value from the current instruction.
	 * @return the immediate value from the current instruction.
	 */
	MIPSUShort getImmediate (void) const;

	/**
	 * Gets the target value from the current instruction.
	 * @return the target value from the current instruction.
	 */
	MIPSWord getTarget (void) const;

	/**
	 * Gets the shift amount value from the current instruction.
	 * @return the shift amount value from the current instruction.
	 */
	UByte getShift (void) const;

	/**
	 * Performs the action required by the current instruction.
	 * @throw runtime_error if current instruction is not a valid opcode.
	 */
	void processInstruction (void);

	/**
	 * Adds two registers ignoring overflow and stores the result in a third.
	 */
	void operationADD (void);

	/**
	 * Adds a register and an immediate ignoring overflow and stores the result in a second register.
	 */
	void operationADDI (void);

	/**
	 * Adds a register and an immediate ignoring overflow and stores the result in a second register unsigned.
	 */
	void operationADDIU (void);

	/**
	 * Adds two registers ignoring overflow and stores the result in a third unsigned.
	 */
	void operationADDU (void);

	/**
	 * Performs bitwise & between two registers and stores the result in a third.
	 */
	void operationAND (void);

	/**
	 * Performs bitwise & between a register and an immediate and stores the result in a second.
	 */
	void operationANDI (void);

	/**
	 * Branches if two registers are equal.
	 */
	void operationBEQ (void);

	/**
	 * Branches if the register is greater or equal to zero.
	 */
	void operationBGEZ (void);

	/**
	 * Branches if the register is greater or equal to zero and links.
	 */
	void operationBGEZAL (void);

	/**
	 * Branches if the register is greater than zero.
	 */
	void operationBGTZ (void);

	/**
	 * Branches if the register is less than or equal to zero.
	 */
	void operationBLEZ (void);

	/**
	 * Branches if the register is less than zero.
	 */
	void operationBLTZ (void);

	/**
	 * Branches if the register is less than zero and links.
	 */
	void operationBLTZAL (void);

	/**
	 * Branches if two registers are not equal.
	 */
	void operationBNE (void);

	/**
	 * Divides one register by another and stores the quotient in resultLow and remainder in resultHigh.
	 */
	void operationDIV (void);

	/**
	 * Jumps to an immediate address.
	 */
	void operationJ (void);

	/**
	 * Jumps to an address and links.
	 */
	void operationJAL (void);

	/**
	 * Jumps to an address in the return address register.
	 */
	void operationJR (void);

	/**
	 * Loads a byte from an address to a register.
	 */
	void operationLB (void);

	/**
	 * Loads the upper 16 bits of a register from an immediate.
	 */
	void operationLUI (void);

	/**
	 * Loads a word from an address to a register.
	 */
	void operationLW (void);

	/**
	 * Moves the resultHigh to a given register.
	 */
	void operationMFHI (void);

	/**
	 * Moves the resultLow to a given register.
	 */
	void operationMFLO (void);

	/**
	 * Multiples two registers and stores the result in resultLow and resultHigh.
	 */
	void operationMULT (void);

	/**
	 * Performs bitwise | on two registers and stores the result in a third.
	 */
	void operationOR (void);

	/**
	 * Performs bitwise | on a register and an immediate and stores the result in a second register.
	 */
	void operationORI (void);

	/**
	 * Shifts the value of a register left by the shift ammount and fills with zeros and stores in a second register.
	 */
	void operationSLL (void);

	/**
	 * Shifts the value of a register left by the value of another register and fills with zeros and stores in a third.
	 */
	void operationSLLV (void);

	/**
	 * Sets a register to one if one register is less than the other, otherwise the register is set to zero.
	 */
	void operationSLT (void);

	/**
	 * Sets a register to one if one register is less than the immediate, otherwise the register is set to zero.
	 */
	void operationSLTI (void);

	/**
	 * Sets a register to one if one register is less than the unsigned immediate, otherwise the register is set to zero.
	 */
	void operationSLTIU (void);

	/**
	 * Sets a register to one if one unsigned register is less than the other, otherwise the register is set to zero.
	 */
	void operationSLTU (void);

	/**
	 * Shifts a register right by the shift amount and fills with the sign bit and stores in another register.
	 */
	void operationSRA (void);

	/**
	 * Shifts a register right by the shift amount and fills with zeros and stores in another register.
	 */
	void operationSRL (void);

	/**
	 * Shifts a register right by a second register and fills with zeros and stores in another register.
	 */
	void operationSRLV (void);

	/**
	 * Subtracts two registers and stores the result in a third.
	 */
	void operationSUB (void);

	/**
	 * Subtracts two registers and stores the unsigned result in a third.
	 */
	void operationSUBU (void);

	/**
	 * Stores a register to a specific address.
	 */
	void operationSW (void);

	/**
	 * Stores a byte of a register to a specific address.
	 */
	void operationSB (void);

	/**
	 * Requests a system call.
	 * @throw runtime_error if the previous system call is not handled.
	 */
	void operationSYSCALL (void);

	/**
	 * Performs bitwise XOR on two registers and stores the result in a third.
	 */
	void operationXOR (void);

	/**
	 * Performs bitwise XOR on a register and an immediate and stores the result in a second register.
	 */
	void operationXORI (void);

	/**
	 * Converts an unsigned short to signed, sign extends it to 32 bits and converts back to unsigned.
	 * @param value to convert.
	 * @return unsigned representation of a sign extended signed value.
	 */
	MIPSUWord signedExtendShort (const MIPSUShort value) const;

	/**
	 * Converts an unsigned short to a 32 bit unsigned.
	 * @param value to convert.
	 * @return unsigned representation of a sign extended unsinged value.
	 */
	MIPSUWord unsignedExtendShort (const MIPSUShort value) const;

	/**
	 * Interprets bits as signed word.
	 * @param value to convert.
	 * @return singed word.
	 */
	MIPSWord asSigned (const MIPSUWord value) const;

	/**
	 * Interprets bits as unsigned word.
	 * @param value to convert.
	 * @return unsinged word.
	 */
	MIPSUWord asUnsigned (const MIPSWord value) const;

	/**
	 * Determines if the current instruction matches the template.
	 * @param instructionTemplate to check instruction against.
	 * @return true if the instruction matches the template, otherwise false.
	 */
	bool isInstruction (const std::string& instructionTemplate) const;

};

#endif