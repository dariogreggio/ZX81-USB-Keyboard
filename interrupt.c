#include <p24fxxxx.h>


#include "ZX81KeybEmu.h"
#include "usb.h"
#include "usb_host_hid_parser.h"
#include "usb_host_hid.h"
#include "keyboard.h"

	

volatile WORD tick50=0;
volatile BYTE second_50=0;


extern HID_USER_DATA_SIZE Appl_ShadowBuffer1[6];


// ---------------------------------------------------------------------------------------

void __attribute__ (( interrupt, shadow,  no_auto_psv )) _T2Interrupt(void) {
	register BYTE x,y;
	BYTE o=0b00011111;


//	LATC ^= 0xffff; 

	second_50=1;					// flag
	tick50++;

/* metto in main...	for(x=0; x<8; x++) {
		for(y=0; y<5; y++) {
			o &= ~y;
			}
		}
*/


	IFS0bits.T2IF = 0; 			//Clear the Timer2 interrupt status flag 
	}




void _ISR __attribute__((__no_auto_psv__)) _AddressError(void) {
	Nop();
	Nop();
	}

void _ISR __attribute__((__no_auto_psv__)) _StackError(void) {
	Nop();
	Nop();
	}
	
