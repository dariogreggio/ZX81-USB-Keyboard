#ifndef _PC_PIC_SOUTHBRIDGE_H
#define _PC_PIC_SOUTHBRIDGE_H
    
    
#include "GenericTypeDefs.h"

#include "hardwareprofile.h"
#include <libpic30.h>


/* check if build is for a real debug tool */
#if defined(__DEBUG) && !defined(__MPLAB_ICD2_) && !defined(__MPLAB_ICD3_) && \
   !defined(__MPLAB_PICKIT2__) && !defined(__MPLAB_PICKIT3__) && \
   !defined(__MPLAB_REALICE__) && \
   !defined(__MPLAB_DEBUGGER_REAL_ICE) && \
   !defined(__MPLAB_DEBUGGER_ICD3) && \
   !defined(__MPLAB_DEBUGGER_PK3) && \
   !defined(__MPLAB_DEBUGGER_PICKIT2) && \
   !defined(__MPLAB_DEBUGGER_PIC32MXSK)
    #warning Debug with broken MPLAB simulator
    #define USING_SIMULATOR
#endif


#define VERNUMH     1
#define VERNUML     0


//void di(void);
#define di() { asm volatile ("push SR");   asm volatile ("mov.w #0x00E0,w0");   asm volatile ("ior.W SR");}
   //void ei(void);
#define ei() { asm volatile ("pop SR"); }


extern volatile BYTE second_50;
extern volatile WORD tick50;

void Timer_Init(unsigned int);


void syncPS2Leds(void);

// il Timer0 conta ogni 62.5nSec*prescaler=64... (@32MHz CPUCLK => 16MHz) su PIC24; fanno 4uS
#define TMR2BASE (((FCY/64)/5)-11)		//   50Hz per timer 
//#define TMR2BASE (20000-11)		//   10Hz per timer con ps=8


void bumpClock(void);
void notifyToCPU(BYTE,BYTE *,BYTE);


// *****************************************************************************
// *****************************************************************************
// Data Structures
// *****************************************************************************
// *****************************************************************************

typedef struct __attribute((packed)) _HID_REPORT_BUFFER {
  WORD  Report_ID;
  WORD  ReportSize;
  BYTE  ReportData[16];      // MESSO FISSO (come facemmo in KUSmouse) anziché malloc! (meglio per hot plug)
  WORD  ReportPollRate;
	}   HID_REPORT_BUFFER;

typedef union __attribute((packed)) _HID_LED_REPORT_BUFFER {
    BYTE b;
    struct {
        BYTE  NUM_LOCK      : 1;
        BYTE  CAPS_LOCK     : 1;
        BYTE  SCROLL_LOCK   : 1;
        BYTE  UNUSED        : 5;
        };
	} HID_LED_REPORT_BUFFER;
typedef union __attribute((packed)) _PS2_LED_REPORT_BUFFER {
    BYTE b;
    struct {
        BYTE  SCROLL_LOCK   : 1;
        BYTE  NUM_LOCK      : 1;
        BYTE  CAPS_LOCK     : 1;
        BYTE  UNUSED        : 5;
        };
	} PS2_LED_REPORT_BUFFER;
extern WORD CodePage;
    
typedef enum __attribute((packed)) _APP_STATE {
  DEVICE_NOT_CONNECTED,
  DEVICE_CONNECTED, // Device Enumerated  - Report Descriptor Parsed 
  DEVICE_CONNECTED2, // post-init
  READY_TO_TX_RX_REPORT,
  GET_INPUT_REPORT, // perform operation on received report 
  INPUT_REPORT_PENDING,
  SEND_OUTPUT_REPORT, // Not needed in case of mouse 
  OUTPUT_REPORT_PENDING,
  ERROR_REPORTED 
	} APP_STATE;




#endif
    
