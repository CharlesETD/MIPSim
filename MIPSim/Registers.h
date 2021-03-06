/**
 * Author:		Charles Duncan (CharlesETD@gmail.com)
 * Created:		Jan 31, 2016
 * Compiler:	MSVS Express 2012
 * OS:			Windows 7 Home Premium SP1 x64
 *
 * � Copyright 2016 Charles Duncan (CharlesETD@gmail.com)
 */

#pragma once
#ifndef REGISTERS_H
#define REGISTERS_H

// Global Constants **********************************************************
/**
 * The number of registers defined in the simulation. See the Register enum
 * for the exact mapping of each register.
 */
const unsigned int NUM_REGISTERS = 32;

// Enums *********************************************************************
/**
 * Defines the name of every accessible MIPS register. Note
 * that some registers may not be used in the simulation but
 * are listed for completion. Also note that the value of
 * each enum is in-fact the offset of the register it represents
 * in memory.
 */
enum Register {

	ZERO = 0,
	AT = 1,
	V0 = 2,
	V1 = 3,
	A0 = 4,
	A1 = 5,
	A2 = 6,
	A3 = 7,
	T0 = 8,
	T1 = 9,
	T2 = 10,
	T3 = 11,
	T4 = 12,
	T5 = 13,
	T6 = 14,
	T7 = 15,
	S0 = 16,
	S1 = 17,
	S2 = 18,
	S3 = 19,
	S4 = 20,
	S5 = 21,
	S6 = 22,
	S7 = 23,
	T8 = 24,
	T9 = 25,
	K0 = 26,
	K1 = 27,
	GP = 28,
	SP = 29,
	S8 = 30,
	FP = 30,
	RA = 31

};

#endif