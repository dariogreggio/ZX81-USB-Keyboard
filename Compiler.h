/*********************************************************************
 *
 *                  Compiler and hardware specific definitions
 *
 *********************************************************************
 * FileName:        Compiler.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.00 or higher
 *					Microchip C30 v3.01 or higher
 *					Microchip C18 v3.13 or higher
 *					HI-TECH PICC-18 STD 9.50PL3 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2008 Microchip Technology Inc.  All rights 
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and 
 * distribute: 
 * (i)  the Software when embedded on a Microchip microcontroller or 
 *      digital signal controller product ("Device") which is 
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c and 
 *      ENC28J60.h ported to a non-Microchip device used in 
 *      conjunction with a Microchip ethernet controller for the 
 *      sole purpose of interfacing with the ethernet controller. 
 *
 * You should refer to the license agreement accompanying this 
 * Software for additional information regarding your rights and 
 * obligations.
 *
 *
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		10/03/2006	Original, copied from old Compiler.h
 * Howard Schlunder		11/07/2007	Reorganized and simplified
 ********************************************************************/
#ifndef __COMPILER_H
#define __COMPILER_H

// Include proper device header file
#if defined(__PIC24F__)	// XC16 compiler
	// PIC24F processor
	#include <p24fxxxx.h>
#elif defined(__PIC24H__)	// Microchip C30 compiler
	// PIC24H processor
	#include <p24Hxxxx.h>
#elif defined(__dsPIC33F__)	// Microchip C30 compiler
	// dsPIC33F processor
	#include <p33Fxxxx.h>
#elif defined(__dsPIC33E__)	// XC16 compiler
	// dsPIC33E processor
	#include <xc.h>
#elif defined(__dsPIC30F__)	// Microchip C30 compiler
	// dsPIC30F processor
	#include <p30fxxxx.h>
#elif defined(__PIC32MX__)	// Microchip C32 compiler
	#if !defined(__C32__)
		#define __C32__
	#endif
	#include <p32xxxx.h>
	#include <plib.h>
#elif defined(__PIC32MM__)	// Microchip XC32 compiler
	#if !defined(__C32__)
		#define __C32__
	#endif
	#include <xc.h>
//	#include <plib.h>
#elif defined(__PIC32MZ__)	// Microchip XC32 compiler
	#if !defined(__C32__)
		#define __C32__
	#endif
	#include <xc.h>
//	#include <plib.h>
#else
	#error Unknown processor or compiler.  See Compiler.h
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Base RAM and ROM pointer types for given architecture
#if defined(__PIC32MX__) || defined(__PIC32MZ__) || defined(__PIC32MM__)
	#define PTR_BASE		DWORD
	#define ROM_PTR_BASE	DWORD
#elif defined(__C30__)
	#define PTR_BASE		WORD
	#define ROM_PTR_BASE	WORD
#endif


// Definitions that apply to all except Microchip MPLAB C Compiler for PIC18 MCUs (formerly C18)



// Definitions that apply to all 8-bit products

	#define	ROM			const
	#define	rom			const

	// 16-bit specific defines (PIC24F, PIC24H, dsPIC30F, dsPIC33F)
	#if defined(__C30__)
		#define Reset()				asm("reset")
        #define FAR                 // __attribute__((far))
	#endif

	// 32-bit specific defines (PIC32)
	#if defined(__C32__)
		#define persistent
		#define far
        #define FAR
		#define Reset()				SoftReset()
#ifndef ClrWdt
		#define ClrWdt()			{WDTCONbits.WDTCLRKEY=0x5743; /*WDTCONSET = _WDTCON_WDTCLR_MASK*/}
#endif

		// MPLAB C Compiler for PIC32 MCUs version 1.04 and below don't have a 
		// Nop() function. However, version 1.05 has Nop() declared as _nop().
		#if !defined(Nop)	
			#define Nop()				asm("nop")
		#endif
	#endif


#endif
