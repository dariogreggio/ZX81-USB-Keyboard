/******************************************************************************
 * FileName:        MainDemo.c
 * Dependencies:    see included files below
 * Processor:       PIC24F, dsPIC
 * Compiler:        XC16 
 * Company:         Microchip Technology, Inc.
 * Software License Agreement
 *
 * Copyright (c) 2011 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 *******************************************************************************/

#ifdef __PIC24FJ256GB206__
// PIC24FJ256GB206 Configuration Bit Settings
// 'C' source line config statements

// CONFIG4

// CONFIG3
#pragma config WPFP = WPFP255           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config SOSCSEL = EC           // Secondary Oscillator Power Mode Select (Secondary oscillator is in I/O or ext)
#pragma config WUTSEL = LEG             // Voltage Regulator Wake-up Time Select (Default regulator start-up time is used)
#pragma config ALTPMP = ALPMPDIS        // Alternate PMP Pin Mapping (EPMP pins are in default location mode)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Segmented code protection is disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Last page (at the top of program memory) and Flash Configuration Words are not write-protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Protected code segment upper boundary is at the last page of program memory; the lower boundary is the code page specified by WPFP)

// CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator is disabled)
#pragma config IOL1WAY = ON             // IOLOCK One-Way Set Enable (The IOLOCK bit (OSCCON<6>) can be set once, provided the unlock sequence has been completed. Once set, the Peripheral Pin Select registers cannot be written to a second time.)
#pragma config OSCIOFNC = ON            // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as I/O)
#pragma config FCKSM = CSDCMD           // Clock Switching and Fail-Safe Clock Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRCPLL           // Initial Oscillator Select (Fast RC Oscillator with PLL ecc Postscaler (FRCPLL))
#pragma config PLL96MHZ = ON            // 96MHz PLL Startup Select (96 MHz PLL is enabled automatically on start-up)
#pragma config PLLDIV = DIV2           // 96 MHz PLL Prescaler Select (Oscillator input is divided by 12 (48 MHz input))
#pragma config IESO = ON                // Internal External Switchover (IESO mode (Two-Speed Start-up) is enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed WDT (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = ON              // Watchdog Timer (Watchdog Timer is enabled)
#pragma config ICS = PGx2               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC2/PGED2)
#pragma config GWRP = OFF               // General Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF              // JTAG Port Enable (JTAG port is disabled)

/* QUARZO
*/
//_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx3 & FWDTEN_ON & WINDIS_OFF & FWPSA_PR32 & WDTPS_PS1024)
//_CONFIG2(IESO_OFF & FNOSC_PRIPLL & PLL_96MHZ_ON & OSCIOFNC_OFF & POSCMOD_HS & PLLDIV_DIV2 & FCKSM_CSDCMD & IOL1WAY_ON & DISUVREG_OFF )
//_CONFIG3(WPFP_WPFP0 &  WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)

//USB Host
//        _CONFIG2(FNOSC_PRIPLL & POSCMOD_HS & PLL_96MHZ_ON & PLLDIV_DIV2 & IESO_OFF) // Primary HS OSC with PLL, USBPLL /2
//       _CONFIG1(JTAGEN_OFF & FWDTEN_OFF & ICS_PGx2)   // JTAG off, watchdog timer off

#endif

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <p24fxxxx.h>


/************************************************************
 * Includes
 ************************************************************/
#include <ctype.h>
#include <stdlib.h>
#include "ZX81KeybEmu.h"

#ifdef __PIC24FJ64GB002__
#include <pps.h>
#elif __PIC24FJ64GB202__
#include <pps.h>
#endif


#include "usb_config.h"
#include "usb.h"
#include "usb_host_hid_parser.h"
#include "usb_host_hid.h"
#include "usb_host_local.h"
#include "usb_host_printer.h"
#include "keyboard.h"

/************************************************************
 * Configuration Bits
 ************************************************************/

#ifdef __PIC24FJ64GB002__
// PIC24FJ64GB002 Configuration Bit Settings
// 'C' source line config statements

/* QUARZO
*/
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx3 & FWDTEN_ON & WINDIS_OFF & FWPSA_PR32 & WDTPS_PS1024)
//_CONFIG2(IESO_OFF & FNOSC_PRIPLL & PLL_96MHZ_ON & OSCIOFNC_OFF & POSCMOD_HS & PLLDIV_DIV2 & FCKSM_CSDCMD & IOL1WAY_ON & DISUVREG_OFF )
_CONFIG2(POSCMOD_HS & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_ON)		// osc int
_CONFIG3(WPFP_WPFP0 &  WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)

//USB Host
//        _CONFIG2(FNOSC_PRIPLL & POSCMOD_HS & PLL_96MHZ_ON & PLLDIV_DIV2 & IESO_OFF) // Primary HS OSC with PLL, USBPLL /2
//       _CONFIG1(JTAGEN_OFF & FWDTEN_OFF & ICS_PGx2)   // JTAG off, watchdog timer off

#endif


#ifdef USA_BOOTLOADER 
//#define BL_ENTRY_BUTTON PORTEbits.RE0 //button 1...

//If defined, the reset vector pointer and boot mode entry delay
// value will be stored in the device's vector space at addresses 0x100 and 0x102
#define USE_VECTOR_SPACE
//****************************************

//Bootloader Vectors *********************
#ifdef USE_VECTOR_SPACE
	/*
		Store delay timeout value & user reset vector at 0x100 
		(can't be used with bootloader's vector protect mode).
		
		Value of userReset should match reset vector as defined in linker script.
		BLreset space must be defined in linker script.
	*/
	unsigned char timeout  __attribute__ ((space(prog),section(".BLreset"))) = 0x0A;
	unsigned int userReset  __attribute__ ((space(prog),section(".BLreset"))) = 0xC00; 
//cambiato ordine o le metteva a cazzo... 
#else
	/*
		Store delay timeout value & user reset vector at start of user space
	
		Value of userReset should be the start of actual program code since 
		these variables will be stored in the same area.
	*/
	unsigned int userReset  __attribute__ ((space(prog),section(".init"))) = 0xC04 ;
	unsigned char timeout  __attribute__ ((space(prog),section(".init"))) = 5 ;
#endif
#endif



static const char _ZX81_KEYBOARD_EMULATOR_C[]= {"PIC24FJ256GB202 _ZX81_KEYBOARD_EMULATOR - 18/6/2023\r\n\r\n"};
#ifdef USA_BOOTLOADER 
	' ','B','L',
#endif
static const char Copyr1[]="(C) Dario's Automation 2023 - G.Dar\xd\xa\x0";
// MORTE AGLI UMANI FOREVER!!!

/************************************************************
 * Defines
 ************************************************************/

/************************************************************
 * Function Prototypes
 ************************************************************/



/************************************************************
 * Variables
 ************************************************************/

//Flags

extern volatile APP_STATE App_State_USB;
extern BOOL DisplayConnectOnce,DisplayDetachOnce;
extern BYTE ErrorDriver;
extern BYTE ErrorCounter;
void InitializeTimer(void);
#define MAX_ERROR_COUNTER 10

WORD CodePage=0;
extern HID_USER_DATA_SIZE Appl_ShadowBuffer1[6];
BYTE keyboardTypematic1=1,keyboardTypematic2=15,ledStatusLocal=FALSE;

unsigned char setCodePageArray(WORD);

BYTE keyNew=0;

extern BYTE TPLfound;




/************************************************************
 * int main (void)
 ************************************************************/
int main(void) {
	int i;
  

#ifdef __PIC24FJ64GB002__
 	AD1PCFGL = 0xFFFF;
#else
	ANSB=0x0000;
	ANSC=0x0000;
	ANSD=0x0000;
	ANSF=0x0000;
	ANSG=0x0000;
#endif

  
#ifdef __PIC24FJ64GB002__
  CLKDIVbits.PLLEN     = 1; //
  while(OSCCONbits.LOCK != 1);
  
	CLKDIVbits.RCDIV=0;		// Set 1:1 8MHz FRC postscaler pll
  CLKDIVbits.CPDIV=0;
#elif __PIC24FJ64GB202__
  CLKDIVbits.PLLEN     = 1; //
  while(OSCCONbits.LOCK != 1);
  
	CLKDIVbits.RCDIV=0;		// Set 1:1 8MHz FRC postscaler pll
  CLKDIVbits.CPDIV=0;
#endif


	LATA =  0b0000000000000000;			// 
	LATB =  0b0000000000000000;			// 

	TRISA = 0b0000000000001111;			// 4 colonne
	TRISB = 0b0000000000001111;			// 4 colonne

#ifdef __PIC24FJ64GB002__
  CNPU1bits.CN2PUE=CNPU1bits.CN3PUE=CNPU1bits.CN4PUE=CNPU1bits.CN5PUE=CNPU1bits.CN6PUE=CNPU1bits.CN7PUE=1;
  CNPU2bits.CN29PUE=CNPU2bits.CN30PUE=1;

#elif __PIC24FJ64GB202__
  CNPU2bits.CN26PUE=CNPU2bits.CN27PUE=CNPU2bits.CN28PUE=CNPU3bits.CN32PUE=CNPU2bits.CN30PUE=1;
  CNPU2bits.CN31PUE=1;
  CNPU1bits.CN13PUE=CNPU1bits.CN14PUE=CNPU4bits.CN53PUE=CNPU4bits.CN56PUE=1;
#endif


//	while(1) {	/*LED1_IO ^= 1;*/ ClrWdt(); 		// test clock 206
//		LATB ^= 0xffff; }		// 21/6/22: 2MHz = 250nS per 4 istruzioni ASM (1+1+2), ok 32MHz 


//#ifndef USA_BOOTLOADER mah e invece sì sì sì

// Unlock Registers
//	PPSUnLock;
//	PPSOutput(PPS_RP11, PPS_U1TX);      // TXD pin 46 RD0
//	PPSInput(PPS_U1RX, PPS_RP24);      // RXD pin 49 RD1
//  PPSOutput(PPS_RP23,PPS_U1RTS);      // RTS pin 50 RD2
//  PPSInput(PPS_U1CTS,PPS_RP22);      // CTS pin 51 RD3
//	PPSLock;
#ifdef __PIC24FJ64GB002__
  __builtin_write_OSCCONL(OSCCON & 0xbf);
  RPINR18bits.U1RXR = 24; // BUZZER FARE!
  __builtin_write_OSCCONL(OSCCON | 0x40);
#elif __PIC24FJ64GB202__
  __builtin_write_OSCCONL(OSCCON & 0xbf);
  RPOR5bits.RP11R = 3;
  RPINR18bits.U1RXR = 24;
  RPOR11bits.RP23R = 4;
  RPINR18bits.U1CTSR = 22;
  __builtin_write_OSCCONL(OSCCON | 0x40);
#endif 


#ifndef USING_SIMULATOR
  __delay_ms(200);
#endif 
  
  Timer_Init(TMR2BASE);
  
//	InitUART(BAUD_RATE,8,0,1,0);

//  putsUART("Booting...\r\n");
  
	ClrWdt();


//	LED0_IO ^= 1;


	//setCodePageArray(437);


#ifndef USING_SIMULATOR
	USBInitialize(0);
#endif 
  



  while(1) {
		ClrWdt();

//		LATB ^= 0xffff;   //9.5uS 21/6/22 su 206


    USBTasks();
    
    {static DWORD divider;
    divider++;
    if(!(divider & 0xffff)) {      // 10uS*65536 = 0.6sec x non disturbare PS/2 v. PC_PIC TOGLIERE QUA???
      if(!TPLfound) {
  //      DisplayDetachOnce = FALSE;
        App_Detect_Device();
        }
      if(TPLfound==TPL_KEYBOARD1 || TPLfound==TPL_KEYBOARD2)
        App_Detect_Device();
      // QUESTO OVVIAMENTE non va con la stampante! di là arriva evento...
      }
    }
    
    switch(App_State_USB) {
	    case DEVICE_NOT_CONNECTED:
        switch(TPLfound) {

          case TPL_KEYBOARD1:
          case TPL_KEYBOARD2:
            T3CONbits.TON = 0;                  // stop timer 

            break;

          }
        if(DisplayDetachOnce == FALSE) {
          #ifdef DEBUG_MODE
          putsUART( "Device Detached \r\n" );
          #endif
          DisplayDetachOnce = TRUE;
          Appl_raw_report_buffer.Report_ID = 0;
          Appl_raw_report_buffer.ReportSize = 0;
          Appl_raw_report_buffer.ReportPollRate = 0;
          
          TPLfound=0;
      		}
        if(USBHostHID_ApiDeviceDetect()) {		// True if report descriptor is parsed with no error
          App_State_USB = DEVICE_CONNECTED;
          DisplayConnectOnce = FALSE;
          }
        break;
        
      case DEVICE_CONNECTED:
        if(DisplayConnectOnce == FALSE) {
          #ifdef DEBUG_MODE
          putsUART( "Explorer16 Board \r\n" );
          putsUART( "USB HIDHost Demo \r\n" );
          #endif
          DisplayConnectOnce = TRUE;
          DisplayDetachOnce = FALSE;
          
		      }
        switch(TPLfound) {
          case TPL_KEYBOARD1:
          case TPL_KEYBOARD2:
            InitializeTimer(); // start 10ms timer to schedule input reports
            i=0b00000111;
            // lampetto come in PS/2...
            if(USBHostHID_ApiSendReport(Appl_LED_Indicator.reportID,Appl_LED_Indicator.interfaceNum, 
              Appl_LED_Indicator.reportLength, (BYTE*)&i) == USB_HID_COMMAND_PASSED) {
              App_State_USB=DEVICE_CONNECTED2;
              }
            break;

          }
        break;
        
      case DEVICE_CONNECTED2:
        switch(TPLfound) {

          case TPL_KEYBOARD1:
          case TPL_KEYBOARD2:
            __delay_ms(50);
            i=0;
            if(USBHostHID_ApiSendReport(Appl_LED_Indicator.reportID,Appl_LED_Indicator.interfaceNum, 
              Appl_LED_Indicator.reportLength, (BYTE*)&i) == USB_HID_COMMAND_PASSED) {
              App_State_USB = READY_TO_TX_RX_REPORT;
              }
            __delay_ms(100);
            // non va, ci vorrebbe un altro Stato.. opp. mandare direttamente lo stato led...
            USBHostHID_ApiSendReport(Appl_LED_Indicator.reportID,Appl_LED_Indicator.interfaceNum, 
              Appl_LED_Indicator.reportLength, (BYTE*)&Appl_led_report_buffer);
            break;

          }
        break;
        
      case READY_TO_TX_RX_REPORT:
        {static WORD divider2;
        divider2++;    // 10 uS iterazione, fanno 0.6 sec, per non disturbare PS/2
        if(!divider2 && !USBHostHID_ApiDeviceDetect()) {
          App_State_USB = DEVICE_NOT_CONNECTED;
//          deviceInfoHID[i].ID.deviceAddress=0;
          TPLfound=0;
          //                                DisplayOnce = FALSE;
          }
        else {

          }
        }
        break;
        
      case GET_INPUT_REPORT:
        switch(TPLfound) {

          case TPL_KEYBOARD1:
          case TPL_KEYBOARD2:
            if(USBHostHID_ApiGetReport(Appl_raw_report_buffer.Report_ID,Appl_ModifierKeysDetails.interfaceNum,
              Appl_raw_report_buffer.ReportSize, Appl_raw_report_buffer.ReportData) != USB_SUCCESS) {
              // Host may be busy/error -- keep trying 
              }
            else {
              App_State_USB = INPUT_REPORT_PENDING;
              }
            break;

          }
        break;
        
      case INPUT_REPORT_PENDING:
        if(USBHostHID_ApiTransferIsComplete(&ErrorDriver,&NumOfBytesRcvd)) {
          if(ErrorDriver || (NumOfBytesRcvd != Appl_raw_report_buffer.ReportSize)) {
	          ErrorCounter++;
	          if(MAX_ERROR_COUNTER <= ErrorDriver)
	            App_State_USB = ERROR_REPORTED;
	          else
	            App_State_USB = READY_TO_TX_RX_REPORT;
          	}
          else {
            ErrorCounter = 0; 
            ReportBufferUpdated = TRUE;
            App_State_USB = READY_TO_TX_RX_REPORT;

            if(DisplayConnectOnce == TRUE) {
							int i;
              for(i=0; i<Appl_raw_report_buffer.ReportSize; i++) {
                if(Appl_raw_report_buffer.ReportData[i] != 0) {
                  DisplayConnectOnce = FALSE;
                  }
              	}
          		}

            switch(TPLfound) {

              case TPL_KEYBOARD1:
              case TPL_KEYBOARD2:
                  App_ProcessInputReport();
    //              App_PrepareOutputReport();
                break;

              }
            }
          }
        break;

      case SEND_OUTPUT_REPORT: // Will be done while implementing Keyboard 
        switch(TPLfound) {

          case TPL_KEYBOARD1:
          case TPL_KEYBOARD2:
            if(USBHostHID_ApiSendReport(Appl_LED_Indicator.reportID,Appl_LED_Indicator.interfaceNum, 
              Appl_LED_Indicator.reportLength, (BYTE*)&Appl_led_report_buffer) != USB_HID_COMMAND_PASSED) {
              // Host may be busy/error -- keep trying 
              }
            else {
              App_State_USB = OUTPUT_REPORT_PENDING;
              }
            break;

          }
        break;

      case OUTPUT_REPORT_PENDING:
        switch(TPLfound) {

          case TPL_KEYBOARD1:
          case TPL_KEYBOARD2:
            if(USBHostHID_ApiTransferIsComplete(&ErrorDriver,&NumOfBytesRcvd)) {
              if(ErrorDriver) {
               ErrorCounter++ ; 
               if(MAX_ERROR_COUNTER <= ErrorDriver)
                   App_State_USB = ERROR_REPORTED;

               //                                App_State_USB = READY_TO_TX_RX_REPORT;
                }
              else {
                ErrorCounter = 0; 
                App_State_USB = READY_TO_TX_RX_REPORT;
                }
              }
            break;

          }
        // GESTIRE!
        
        break;

      case ERROR_REPORTED:
        break;
        
      default:
        break;

      }

//    m_PSelP ^=1;     // check timer 10uS, 11/7/22

    
    if(keyNew) {    // 
      
      keyNew=0;
      }


#define TO_HID(c) ((char)(c-0x3d))
#define TO_HID_N(c) ((char)(c+0x13))
		if(second_50) {
			BYTE o=0b00011111;

//      m_PReset ^= 1;    // test
			if(!PORTAbits.RA0) {			// A8
				if(Appl_BufferModifierKeys[1] || Appl_BufferModifierKeys[5])		// SHift
					o &= ~0b00000001;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('Z')))
					o &= ~0b00000010;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('X')))
					o &= ~0b00000100;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('C')))
					o &= ~0b00001000;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('V')))
					o &= ~0b00010000;
				}
			if(!PORTAbits.RA1) {			// A9
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('A')))
					o &= ~0b00000001;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('S')))
					o &= ~0b00000010;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('D')))
					o &= ~0b00000100;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('F')))
					o &= ~0b00001000;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('G')))
					o &= ~0b00010000;
				}
			if(!PORTAbits.RA2) {			// A10
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('Q')))
					o &= ~0b00000001;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('W')))
					o &= ~0b00000010;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('E')))
					o &= ~0b00000100;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('R')))
					o &= ~0b00001000;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('T')))
					o &= ~0b00010000;
				}
			if(!PORTAbits.RA3) {			// A11
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('1')))
					o &= ~0b00000001;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('2')))
					o &= ~0b00000010;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('3')))
					o &= ~0b00000100;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('4')))
					o &= ~0b00001000;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('5')))
					o &= ~0b00010000;
				}
			if(!PORTBbits.RB0) {			// A12
				if(strchr((char*)Appl_ShadowBuffer1,0x27 /*TO_HID_N('0')*/))
					o &= ~0b00000001;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('9')))
					o &= ~0b00000010;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('8')))
					o &= ~0b00000100;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('7')))
					o &= ~0b00001000;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID_N('6')))
					o &= ~0b00010000;
				}
			if(!PORTBbits.RB1) {			// A13
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('P')))
					o &= ~0b00000001;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('O')))
					o &= ~0b00000010;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('I')))
					o &= ~0b00000100;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('U')))
					o &= ~0b00001000;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('Y')))
					o &= ~0b00010000;
				}
			if(!PORTBbits.RB2) {			// A14
				if(strchr((char*)Appl_ShadowBuffer1,/*Symbol_Return*/ 0x28))
					o &= ~0b00000001;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('L')))
					o &= ~0b00000010;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('K')))
					o &= ~0b00000100;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('J')))
					o &= ~0b00001000;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('H')))
					o &= ~0b00010000;
				}
			if(!PORTBbits.RB3) {			// A15
				if(strchr((char*)Appl_ShadowBuffer1,/*Symbol_Space*/ 0x2C))
					o &= ~0b00000001;
				if(strchr((char*)Appl_ShadowBuffer1,/*Symbol_PeriodGreaterThan*/ 0x37))
					o &= ~0b00000010;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('M')))
					o &= ~0b00000100;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('N')))
					o &= ~0b00001000;
				if(strchr((char*)Appl_ShadowBuffer1,TO_HID('B')))
					o &= ~0b00010000;
				}

			LATBbits.LATB5 = o & 0b00000001 ? 1 : 0;
			LATBbits.LATB7 = o & 0b00000010 ? 1 : 0;
			LATBbits.LATB8 = o & 0b00000100 ? 1 : 0;
			LATBbits.LATB9 = o & 0b00001000 ? 1 : 0;
			LATBbits.LATB13 = o & 0b00010000 ? 1 : 0;

      second_50=0;
			}				// 0.02 sec

		}			//main loop


  return 1;
	}

void syncPS2Leds(void) {
  PS2_LED_REPORT_BUFFER psl;
  int i;
  
  psl.b=0;
  psl.CAPS_LOCK= Appl_led_report_buffer.CAPS_LOCK;// dio cheffroci!
  psl.NUM_LOCK = Appl_led_report_buffer.NUM_LOCK;
  psl.SCROLL_LOCK = Appl_led_report_buffer.SCROLL_LOCK;

  }



// -----------------------------------------------------------------------------
void Timer_Init(unsigned int tim) {
  
  T2CON=0;
  T2CONbits.TCS = 0;                  // clock from peripheral clock
  T2CONbits.TCKPS = 0b10;             // 1:64 prescale
  T2CONbits.T32=0;
  PR2=TMR2BASE;                       // 
  T2CONbits.TON = 1;                  // start timer 
  
  IFS0bits.T2IF=0;
  IPC1bits.T2IP=2;
  IEC0bits.T2IE=1;
  
  }




// ----------------------------------------------------------------------
void putsUART() {		// dummy, usata da USB per debug
	}


