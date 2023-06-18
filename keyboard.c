/******************************************************************************

    USB Keyboard Host Application Demo

Description:
    This file contains the basic USB keyboard application. Purpose of the demo
    is to demonstrate the capability of HID host . Any Low speed/Full Speed
    USB keyboard can be connected to the PICtail USB adapter along with 
    Explorer 16 demo board. This file schedules the HID transfers, and interprets
    the report received from the keyboard. Key strokes are decoded to ascii 
    values and the same can be displayed either on hyperterminal or on the LCD
    display mounted on the Explorer 16 board. Since the purpose is to 
    demonstrate HID host all the keys have not been decoded. However demo gives
    a fair idea and user should be able to incorporate necessary changes for
    the required application. All the alphabets, numeric characters, special
    characters, ESC , Shift, CapsLK and space bar keys have been implemented.

Summary:
 This file contains the basic USB keyboard application.

Remarks:
    This demo requires Explorer 16 board and the USB PICtail plus connector.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the “Company”) for its PICmicro® Microcontroller is intended and
supplied to you, the Company’s customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.


********************************************************************************

 Change History:
  Rev    Description
  ----   -----------
  2.6a   fixed bug in LCDDisplayString() that could cause the string print to
         terminate early.

*******************************************************************************/
//DOM-IGNORE-END
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "usb_config.h"
#include "usb.h"
#include "usb_host_hid_parser.h"
#include "usb_host_hid.h"
#include "usb_host_printer.h"
#include "ZX81KeybEmu.h"
#include "keyboard.h"

//#define DEBUG_MODE

// *****************************************************************************
// *****************************************************************************
// Constants
// *****************************************************************************
// *****************************************************************************

// We are taking Timer 3  to schedule input report transfers

// da 0x91 le frecce, ecc.; pgdn/pgup; 9f=Pause; 9e=Menu
// da 0xa1 a 0xac i tasti Fn
// da 0xb1 a 0xb3 i lock; 0xb8=PrtSc, b9-bb=power/sleep/wake
//  su PS/2 CTRL BREAK arriva come B3??? e non ctrl-9f? su usb va..
// da 0xc1 tasti multimedia: Scan Previous Track, Stop, Play/ Pause, Mute, Bass Boost, Loudness 
//  Volume Up, Volume Down, Bass Up, Bass Down, Treble Up, Treble Down 
// da 0xd1 tasti apps: Media Select, Mail, Calculator, My Computer, WWW Search, WWW Home, WWW Back,
//  WWW Forward, WWW Stop, WWW Refresh, WWW Favorites 


#define MILLISECONDS_PER_TICK       10
#define TIMER_PERIOD                20000                // 10ms=20000, 1ms=2000



extern BYTE keyNew,mouseNew;
extern WORD CodePage;
BYTE codePageArr=0;
extern BYTE keyboardTypematic1,keyboardTypematic2;

// *****************************************************************************
// *****************************************************************************
// Internal Function Prototypes
// *****************************************************************************
// *****************************************************************************
void AppInitialize(void);
void App_Detect_Device(void);
void App_ProcessInputReport(void);
void App_PrepareOutputReport(void);
void InitializeTimer(void);
void App_Clear_Data_Buffer(void);
signed char App_CompareKeyPressedPrevBuf(BYTE);
signed char App_CompareKeyReleasedPrevBuf(BYTE);
void App_CopyToShadowBuffer(void);
BOOL USB_HID_DataCollectionHandlerKB(void);
BOOL USB_HID_DataCollectionHandlerMouse(void);


// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************
#define MAX_ALLOWED_CURRENT             (200)         // Maximum power we can supply in mA
#define MINIMUM_POLL_INTERVAL           (0x0A)        // Minimum Polling rate for HID reports is 10ms
// FORSE x il mouse andava diminuito?? 2021
#define USAGE_PAGE_LEDS                 (0x08)

#define USAGE_PAGE_BUTTONS              (0x09)
#define USAGE_PAGE_GEN_DESKTOP          (0x01)
#define USAGE_PAGE_CONSUMER             (0x0C)

#define USAGE_PAGE_KEY_CODES            (0x07)
#define USAGE_MIN_MODIFIER_KEY          (0xE0)
#define USAGE_MAX_MODIFIER_KEY          (0xE7)

#define USAGE_MIN_NORMAL_KEY            (0x00)
#define USAGE_MAX_NORMAL_KEY            (0xFF)

#define HID_SCROLL_LOCK_VAL               (0x47)
#define HID_CAPS_LOCK_VAL               (0x39)
#define HID_NUM_LOCK_VAL                (0x53)

#define MAX_ERROR_COUNTER               (10)


//******************************************************************************
//  macros to identify special charaters(other than Digits and Alphabets) for HID/USB
//******************************************************************************
#define Symbol_Exclamation              (0x1E)
#define Symbol_AT                       (0x1F)
#define Symbol_Pound                    (0x20)
#define Symbol_Dollar                   (0x21)
#define Symbol_Percentage               (0x22)
#define Symbol_Cap                      (0x23)
#define Symbol_AND                      (0x24)
#define Symbol_Star                     (0x25)
#define Symbol_NormalBracketOpen        (0x26)
#define Symbol_NormalBracketClose       (0x27)

#define Symbol_Return                   (0x28)
#define Symbol_Escape                   (0x29)
#define Symbol_Backspace                (0x2A)
#define Symbol_Tab                      (0x2B)
#define Symbol_Space                    (0x2C)
#define Symbol_HyphenUnderscore         (0x2D)
#define Symbol_EqualAdd                 (0x2E)
#define Symbol_BracketOpen              (0x2F)
#define Symbol_BracketClose             (0x30)
#define Symbol_BackslashOR              (0x31)
#define Symbol_SemiColon                (0x33)
#define Symbol_InvertedComma            (0x34)
#define Symbol_Tilde                    (0x35)
#define Symbol_CommaLessThan            (0x36)
#define Symbol_PeriodGreaterThan        (0x37)
#define Symbol_FrontSlashQuestion       (0x38)

// *****************************************************************************
// *****************************************************************************
// Global Variables
// *****************************************************************************
// *****************************************************************************

volatile APP_STATE App_State_USB=DEVICE_NOT_CONNECTED;
BOOL DisplayConnectOnce=FALSE, DisplayDetachOnce=FALSE;

HID_DATA_DETAILS Appl_LED_Indicator;

HID_DATA_DETAILS Appl_ModifierKeysDetails;
HID_DATA_DETAILS Appl_NormalKeysDetails;

HID_USER_DATA_SIZE Appl_BufferModifierKeys[8];
BYTE PS2_BufferModifierKeys=0;
//#warning servono MODIFIER PRIVATI PER PS2... l usb li sovrascrive a ogni giro (se c'è)
  
HID_USER_DATA_SIZE Appl_BufferNormalKeys[6];
HID_USER_DATA_SIZE Appl_ShadowBuffer1[6];

BYTE getModifierKeys(void) {
  BYTE i,j,n=0;
// no, non credo :) #warning ***SI POTREBBERO mettere nei 3 bit alti lo stato dei led/lock! così passano al pc con i tasti
  
  for(i=0,j=1; i<8; i++,j <<= 1) {
    if(Appl_BufferModifierKeys[i])
      n |= j;
    }
  n |= PS2_BufferModifierKeys;
  return n;
  }

HID_REPORT_BUFFER     Appl_raw_report_buffer;
HID_LED_REPORT_BUFFER Appl_led_report_buffer;

BYTE ErrorDriver;
BYTE ErrorCounter;
BYTE NumOfBytesRcvd;

BOOL ReportBufferUpdated;
//BOOL LED_Key_Pressed=FALSE;
BYTE HeldKeyCount = 0;
BYTE HeldKey;


HID_DATA_DETAILS Appl_Mouse_Buttons_Details;
HID_DATA_DETAILS Appl_XY_Axis_Details;

HID_USER_DATA_SIZE Appl_Button_report_buffer[8];
HID_USER_DATA_SIZE Appl_XY_report_buffer[3];


BYTE Mode=0,DeviceMouseID=0xff /* not_known al boot !*/ ;
BYTE FLAGS,RESN,SRATE;
BYTE READ[5];

extern BYTE TPLfound;

//******************************************************************************
//******************************************************************************
// USB Support Functions
//******************************************************************************
//******************************************************************************

BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {

  switch((INT)event) {
    case EVENT_VBUS_REQUEST_POWER:
      // The data pointer points to a byte that represents the amount of power
      // requested in mA, divided by two.  If the device wants too much power,
      // we reject it.
      if(((USB_VBUS_POWER_EVENT_DATA*)data)->current <= (MAX_ALLOWED_CURRENT / 2)) {
        return TRUE;
        }
      else {
//      putsUART( "\r\n***** USB Error - device requires too much current *****\r\n" );
        }
      break;

    case EVENT_VBUS_RELEASE_POWER:
      // Turn off Vbus power.
      // This board cannot turn off Vbus through software.
      return TRUE;
      break;

    case EVENT_HUB_ATTACH:
//        putsUART( "\r\n***** USB Error - hubs are not supported *****\r\n" );
      return TRUE;
      break;

    case EVENT_UNSUPPORTED_DEVICE:
//        putsUART( "\r\n***** USB Error - device is not supported *****\r\n" );
      return TRUE;
      break;

    case EVENT_CANNOT_ENUMERATE:
//            putsUART( "\r\n***** USB Error - cannot enumerate device *****\r\n" );

      return TRUE;
      break;

    case EVENT_CLIENT_INIT_ERROR:
//            putsUART( "\r\n***** USB Error - client driver initialization error *****\r\n" );
      return TRUE;
      break;

    case EVENT_OUT_OF_MEMORY:
//      putsUART( "\r\n***** USB Error - out of heap memory *****\r\n" );
      return TRUE;
      break;

    case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
//      putsUART( "\r\n***** USB Error - unspecified *****\r\n" );
      return TRUE;
      break;

		case EVENT_HID_RPT_DESC_PARSED:
      switch(TPLfound) {
        case TPL_KEYBOARD1:
        case TPL_KEYBOARD2:
          return USB_HID_DataCollectionHandlerKB();
          break;
        }
			break;

    case EVENT_PRINTER_ATTACH:
//      notifyToCPU(EVENT_USB_IN,&TPLfound,1);
      App_State_USB = DEVICE_CONNECTED;
			break;
    case EVENT_PRINTER_DETACH:
//      notifyToCPU(EVENT_USB_OUT,&TPLfound,1);
	    App_State_USB = DEVICE_NOT_CONNECTED;
			break;
    case EVENT_PRINTER_TX_DONE:
			break;
    case EVENT_PRINTER_RX_DONE:
			break;
    case EVENT_PRINTER_REQUEST_DONE:
			break;
    case EVENT_PRINTER_TX_ERROR:
			break;
    case EVENT_PRINTER_RX_ERROR:
			break;
      
    default:
      break;
    }
  
  return FALSE;
	}



/****************************************************************************
  Function:
    void App_PrepareOutputReport(void)

  Description:
    This function schedules output report if any LED indicator key is pressed.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_PrepareOutputReport(void) {

#if 0
//    if((App_State_Keyboard == READY_TO_TX_RX_REPORT) && (ReportBufferUpdated == TRUE))
  if(ReportBufferUpdated == TRUE) {
    ReportBufferUpdated = FALSE;
    
    if(LED_Key_Pressed) {
      
      syncPS2Leds();
      
      App_State_USB = SEND_OUTPUT_REPORT;
      LED_Key_Pressed = FALSE;
      }
    }
#endif
	}

/****************************************************************************
  Function:
    void App_ProcessInputReport(void)

  Description:
    This function processes input report received from HID device.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_ProcessInputReport(void) {
  BYTE i;
  BYTE data;


 /* process input report received from device */
  USBHostHID_ApiImportData(Appl_raw_report_buffer.ReportData, Appl_raw_report_buffer.ReportSize,
          Appl_BufferModifierKeys, &Appl_ModifierKeysDetails);
  USBHostHID_ApiImportData(Appl_raw_report_buffer.ReportData, Appl_raw_report_buffer.ReportSize,
          Appl_BufferNormalKeys, &Appl_NormalKeysDetails);

#ifdef DEBUG_MODE
9  putsUART( "\n\rHID: Raw Report  " );
  for(i=0;i<(Appl_raw_report_buffer.ReportSize);i++) {
    UART2PutHex( Appl_raw_report_buffer.ReportData[i]);
    putsUART( "-" );
    }
#endif

  
//  m_PReset =1;   // con 256 prescaler siamo a 700/300mS ... asimmetrico?? con timer giusto fa tipoogni 100mS, dura 10uS
  
  
  for(i=0; i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0])); i++) {
    switch(Appl_BufferNormalKeys[i]) {
      case 0:
        if(i==0) {
          HeldKeyCount=0;
          }
        else {
          if(Appl_BufferNormalKeys[i-1] == HeldKey) {
            if(HeldKeyCount < 3) {
              HeldKeyCount++;
              }
            else {
              data = App_HID2ASCII(HeldKey);  // convert data to ascii
              #ifdef DEBUG_MODE
                  writeUART(data);
              #endif
  //                        LCD_Display_Routine(data,HeldKey );
              }
            }
          else {
            HeldKeyCount=0;
            HeldKey = Appl_BufferNormalKeys[i-1];
            }
          }
        goto fine;
        break;
    	case HID_SCROLL_LOCK_VAL:
        Appl_led_report_buffer.SCROLL_LOCK ^= 1;
//          keyToSend[0]=0b00000100; // COME FARE i RILASCI QUA?? confrontare ecc
//        keyToSend[1]=0xb3;
//        keyToSend[2]=getModifierKeys();   // 
        goto std_key;
        break;
    	case HID_CAPS_LOCK_VAL:
//        LED_Key_Pressed = TRUE;
//        Appl_led_report_buffer.CAPS_LOCK ^= 1;
        goto std_key;
        break;
			case HID_NUM_LOCK_VAL:
//        LED_Key_Pressed = TRUE;
//        Appl_led_report_buffer.NUM_LOCK ^= 1;
        goto std_key;
        break;
			default:
        /* check if key press was present in previous report */
        // if key press was pressed in previous report neglect it ???? 
        if(!App_CompareKeyPressedPrevBuf(Appl_BufferNormalKeys[i])) {
std_key:
          data = App_HID2ASCII(Appl_BufferNormalKeys[i]);  // convert data to ascii
          #ifdef DEBUG_MODE
              writeUART(data);
          #endif
          keyNew=1;
#warning per motivi ignoti esce R-WIN come tasto 00, qua
          }
        else
          if(!App_CompareKeyReleasedPrevBuf(Appl_BufferNormalKeys[i])) {
            data = App_HID2ASCII(Appl_BufferNormalKeys[i]);  // convert data to ascii
            keyNew=1;
            }

// FAR PASSARE ANCHE I MODIFIER da soli!! qua? :)
        
        break;
      case 1:   // rollover error https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2
      case 2:   // POST fail
      case 3:   // undefined error
        keyNew |= 0b01000000;
        break;
      }
    }
  
fine:
  App_CopyToShadowBuffer();
  App_Clear_Data_Buffer();
//  m_PReset =0;   // con 256 prescaler siamo a 700/300mS ... asimmetrico?? v. sopra
	}

/****************************************************************************
  Function:
    void App_CopyToShadowBuffer(void)

  Description:
    This function updates the shadow buffers with previous reports.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_CopyToShadowBuffer(void) {
  BYTE i;

  for(i=0; i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0])); i++)
    Appl_ShadowBuffer1[i] = Appl_BufferNormalKeys[i];
	}

/****************************************************************************
  Function:
    signed char App_CompareKeyPressedPrevBuf(BYTE data)

  Description:
    This function compares if the data byte received in report was sent in 
    previous report. This is to avoid duplication incase user key in strokes
    at fast rate.

  Precondition:
    None

  Parameters:
    BYTE data         -   data byte that needs to be compared with previous
                          report

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
signed char App_CompareKeyPressedPrevBuf(BYTE data) {
  BYTE i;

  for(i=0; i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0])); i++) {
    if(data == Appl_ShadowBuffer1[i]) {
      return TRUE;
      }
  	}
  return FALSE;
	}

signed char App_CompareKeyReleasedPrevBuf(BYTE data) {
  BYTE i;

  for(i=0; i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0])); i++) {
    if(data == Appl_ShadowBuffer1[i]) {
      return FALSE;
      }
  	}
  return TRUE;
	}


/****************************************************************************
  Function:
    void App_Detect_Device(void)

  Description:
    This function monitors the status of device connected/disconnected

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_Detect_Device(void) {

  if(!USBHostHID_ApiDeviceDetect()) {
    App_State_USB = DEVICE_NOT_CONNECTED;
//    TPLfound=0;
	  }
	}

/****************************************************************************
  Function:
    void App_Clear_Data_Buffer(void)

  Description:
    This function clears the content of report buffer after reading

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_Clear_Data_Buffer(void) {
  BYTE i;

  for(i=0; i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0])); i++)
    Appl_BufferNormalKeys[i] = 0;

  for(i=0; i<Appl_raw_report_buffer.ReportSize; i++)
    Appl_raw_report_buffer.ReportData[i] = 0;
	}

/****************************************************************************
  Function:
    BYTE App_HID2ASCII(BYTE a)
  Description:
    This function converts the HID code of the key pressed to coressponding
    ASCII value. For Key strokes like Esc, Enter, Tab etc it returns 0.
	
  Precondition:
    None

  Parameters:
    BYTE a          -   HID code for the key pressed

  Return Values:
    BYTE            -   ASCII code for the key pressed

  Remarks:
    https://deskthority.net/wiki/Scancode
***************************************************************************/
BYTE App_HID2ASCII(BYTE a) {	//convert USB HID code (buffer[2 to 7]) to ASCII code
  BYTE AsciiVal;
  BYTE ShiftkeyStatus;

  ShiftkeyStatus = getModifierKeys() & 0b00100010;
#warning finire di gestire codePageArr
    
  if(a>=0x1E && a<=0x27) {
    if(ShiftkeyStatus) {
      switch(a) {
        case Symbol_Exclamation: 
          AsciiVal = 0x21;
          break;    
        case Symbol_AT: 
          AsciiVal = 0x40; 
          break;    
        case Symbol_Pound: 
          AsciiVal = codePageArr ? '£' : '#'; 
          break;                                  
        case Symbol_Dollar: 
          AsciiVal = 0x24; 
          break;                                  
        case Symbol_Percentage:
          AsciiVal = 0x25;    
          break;                                  
        case Symbol_Cap:
          AsciiVal = codePageArr ? '^' : '^';
          break;                                  
        case Symbol_AND: 
          AsciiVal = codePageArr ? '^' : '&';
          break;                                  
        case Symbol_Star:
          AsciiVal = codePageArr ? '*' : '(';
          break;                                  
        case Symbol_NormalBracketOpen: 
          AsciiVal = codePageArr ? ')' : '(';
          break;    
        case Symbol_NormalBracketClose: 
          AsciiVal = codePageArr ? '=' : ')';
          break;
        default:
          break;
      	}
      return AsciiVal;
      }
    else {			// no shift
      if(a==0x27) {
        return '0';
      	}
      else {
        return (a+0x13);
      	}
      } 
    }

  if(a>=0x04 && a<=0x1D) {
    if(((CAPS_Lock_Pressed()) && !(getModifierKeys() & 0b00100010))
      || ((!CAPS_Lock_Pressed()) && (getModifierKeys() & 0b00100010)))
      return a+0x3d;  // return capital
    else
      return a+0x5d;  // return small case
   	}

  if(a>=0x2D && a<=0x38) {
  	switch(a) {
      case Symbol_HyphenUnderscore:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '\'' : '-';
        else
          AsciiVal = codePageArr ? '?' : '_';
        break;                      
      case Symbol_EqualAdd:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'ì' : '=';
        else
          AsciiVal = codePageArr ? '^' : '+';
        break;
      case Symbol_BracketOpen:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'è' : '[';
        else
          AsciiVal = codePageArr ? 'é' : '{';        
        break;
      case Symbol_BracketClose:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '+' : ']';
        else
          AsciiVal = codePageArr ? '*' : '}';
        break;
      case Symbol_BackslashOR:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '\\' : '<';
        else
          AsciiVal = codePageArr ? '|'  : '>';
        break;
      case Symbol_SemiColon:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'ò' : ';';
        else
          AsciiVal = codePageArr ? 'ç' : ':';
        break;
      case Symbol_InvertedComma:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'à' : '\'';
        else
					AsciiVal = codePageArr ? '"' : '°';
        break;
      case Symbol_Tilde:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'ù' : '`';
        else
          AsciiVal = codePageArr ? '§' : '~'; 
        break;
      case Symbol_CommaLessThan:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? ',' : ',';
        else
          AsciiVal = codePageArr ? ';' : '<';
        break;
      case Symbol_PeriodGreaterThan:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '.' : '.';
        else
          AsciiVal = codePageArr ? ':' : '>';
        break;
      case Symbol_FrontSlashQuestion:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '-' : '/';
        else
          AsciiVal = codePageArr ? '_' : '?';
        break;
      default:
        break;
      }
    return AsciiVal;
  	}   
   
  if(a>=0x3A && a<=0x45) {      // Fn tasti funzione
    return a-0x3a+0xa1;
    }
  
  if(a>=0x4F && a<=0x52) {      // frecce
    return a-0x4f+0x91;
    }
  
  switch(a) {
    case Symbol_Space /*0x2C*/: 		// spazio...
      return ' ';
      break;
    case Symbol_Return:
    case 0x58:    // keypad Enter
      return '\n';
      break;
    case 0x29:    // ESC
      return '\x1b';
      break;
    case 0x2a:    // backspace
      return '\x8';
      break;
    case 0x2b:    // TAB
      return '\x9';
      break;
    case 0x54:
      return '/';
      break;
    case 0x55:
      return '*';
      break;
    case 0x56:
      return '-';
      break;
    case 0x57:
      return '+';
      break;
    case 0x46: 		// PrtSc
      return 0xb8;
      break;
    case 0x53: 		// num lock
      return 0xb1;
      break;
    case 0x39: 		// caps lock
      return 0xb2;
      break;
    case 0x47: 		// scroll lock
      return 0xb3;
      break;
    case 0x4c: 		// DEL
      return 0x9a;
      break;
    case 0x49: 		// INS
      return 0x99;
      break;
    case 0x59:
      return NUM_Lock_Pressed() ? '1' : 0x98;
      break;
    case 0x5a:
      return NUM_Lock_Pressed() ? '2' : 0x93;
      break;
    case 0x5b:
      return NUM_Lock_Pressed() ? '3' : 0x95;
      break;
    case 0x5c:
      return NUM_Lock_Pressed() ? '4' : 0x92;
      break;
    case 0x5d:
      return '5';
      break;
    case 0x5e:
      return NUM_Lock_Pressed() ? '6' : 0x91;
      break;
    case 0x5f:
      return NUM_Lock_Pressed() ? '7' : 0x97;
      break;
    case 0x60:
      return NUM_Lock_Pressed() ? '8' : 0x94;
      break;
    case 0x61:
      return NUM_Lock_Pressed() ? '9' : 0x96;
      break;
    case 0x62:
      return NUM_Lock_Pressed() ? '0' : 0x99;
      break;
    case 0x66:    // Power su Page 7
    case 0x81:		// Power su Page 1 come il resto
      return 0xb9;
      break;
    case 0x82:    // Sleep
      return 0xba;
      break;
    case 0x83:    // Wake
      return 0xbb;
      break;
    case 0x48:    // Pause
      return 0x9f;
      break;
    // questi sono in usage 0x0c... a 16 bit, quindi non so...
    case 0xb5:    // scan next track
      return 0xc1;
      break;
    case 0xb6:    // scan prev track
      return 0xc2;
      break;
    case 0xb7:    // stop
      return 0xc3;
      break;
    case 0xcd:    // play pause
      return 0xc4;
      break;
    case 0xe2:    // mute
      return 0xc5;
      break;
    case 0xe5:    // bass boost
      return 0xc6;
      break;
    case 0xe7:    // loudness
      return 0xc7;
      break;
    case 0xe9:    // volume up
      return 0xc8;
      break;
    case 0xea:    // volume down
      return 0xc9;
      break;
    case 0x52:    // bass up 152
      return 0xca;
      break;
    case 0x153:    // bass down 153
      return 0xcb;
      break;
    case 0x154:    // treble up 154
      return 0xcc;
      break;
    case 0x155:    // treble down 155
      return 0xcd;
      break;
    case 0x183:    // media select 183
      return 0xd1;
      break;
    case 0x18a:    // mail 18a
      return 0xd2;
      break;
    case 0x192:    // calculator 192
      return 0xd3;
      break;
    case 0x194:    // my computer 194
      return 0xd4;
      break;
    case 0x221:    // www search 221
      return 0xd5;
      break;
    case 0x223:    // www home 223
      return 0xd6;
      break;
    case 0x224:    // www back 224
      return 0xd7;
      break;
    case 0x225:    // www forward 225
      return 0xd8;
      break;
    case 0x226:    // www stop 226
      return 0xd9;
      break;
    case 0x227:    // www refresh 227
      return 0xda;
      break;
    case 0x22a:    // www favourites 22a
      return 0xdb;
      break;
    }

	return 0;
	}

const BYTE ps2_2_ascii[4][160] = {
  {			// 1°
//  https://wiki.osdev.org/PS/2_Keyboard
	0,
  0xa9,    //F9
  0,
  0xa5,		 //F5
  0xa3,    //F3
  0xa1,    //F1  = 0x05
  0xa2,    //F2
  0xac,    //F12
  0,
  0xaa,    //F10
  0xa8,    //F8
  0xa6,    //F6
  0xa4,    //F4
  9,      //TAB=0xD
  '\'',     // | 
  0,
  
  0,    //16
  0,    //LALT=0x11
  0,    //LSHIFT=0x12 
  0,
  0,    //LCTRL=0x14 
	'q',
	'1',
	0,
	0,
	0,
	'z',
	's',
	'a',
	'w',
	'2',
	0,      // left WIN (con E0)
  
  
	0,    //32
	'c',
	'x',
	'd',
	'e',
	'4',
	'3',
	0,      // right WIN (con E0)
	0,
	' ',    //0x29
	'v',
	'f',
	't',
	'r',
	'5',
	0,
  
	0,    //48
	'n',
	'b',
	'h',
	'g',
	'y',
	'6',
	0,   // Power (con E0)
	0,
	0,
	'm',
	'j',
	'u',
	'7',
	'8',
	0,   // Sleep (con E0)
  
	0,    //64
	',',
	'k',
	'i',
	'o',
	'0',
	'9',
	0,
	0,
	'.',
	'/',	
	'l',
	';',		// sarebbe ò ossia @
	'p',
	'-',
	'\'',		// sarebbe à ossia #
  
	0,    //80
	0,
	0,
	0,
	'[',
	'=',
	0,
	0,
	0xb2,    //caps lock
	0,    //RSHIFT
	'\n',   // CR (0x5A)
	']',
	0,   // 
	'\\',   // (0x5D)
  0,      // Wake (con E0)
  0,
  
  0,    // 0x60
  0,
  0,
  0,
  0,
  0,    
  0x8,    // 0x66 = BKSP
  0,
  0,
  '1',		// end= 0x69
  0,
  '4',		// e cursor left = 0x6b
  '7',		// home= 0x6c
  0,
  0,
  0,
  
  '0',    //0x70, INS se E0
  '.',    // keypad ovvero DEL se E0 71
  '2',		// e cursor down = 0x72
  '5',
  '6',		// e cursor right = 0x74
  '8',		// e cursor up = 0x75
  0x1b,   //0x76=ESC   0xe0 prefix! ...=RCTRL
  0xb1,    // NumLock
  0xab,    // F11
  '+',
  '3',
  '-',
  '*',
  '9',
  0xb3,  //0x7e scroll lock
  0,
  
	0,
	0,
	0,
	0xA7,   //0x83 è F7 !!

	},
  
  
  {			// 2°
	0,
  0xa9,    //F9
  0,
  0xa5,		 //F5
  0xa3,    //F3
  0xa1,    //F1  = 0x05
  0xa2,    //F2
  0xac,    //F12
  0,
  0xaa,    //F10
  0xa8,    //F8
  0xa6,    //F6
  0xa4,    //F4
  9,      //TAB=0xD
  '\\',     // | 
  0,
  
  0,    //16
  0,    //LALT=0x11
  0,    //LSHIFT=0x12 
  0,
  0,    //LCTRL=0x14 
	'q',
	'1',
	0,
	0,
	0,
	'z',
	's',
	'a',
	'w',
	'2',
	0,      // left WIN (con E0)
	0,    //32
	'c',
	'x',
	'd',
	'e',
	'4',
	'3',
	0,      // right WIN (con E0)
	0,
	' ',    //0x29
	'v',
	'f',
	't',
	'r',
	'5',
	0,
  
	0,    //48
	'n',
	'b',
	'h',
	'g',
	'y',
	'6',
	0,   // Power (con E0)
	0,
	0,
	'm',
	'j',
	'u',
	'7',
	'8',
	0,   // Sleep (con E0)
  
	0,    //64
	',',
	'k',
	'i',
	'o',
	'0',
	'9',
	0,
	0,
	'.',
	'/',	
	'l',
	'@',		// sarebbe ò ossia @
	'p',
	'-',
	'#',		// sarebbe à ossia #
  
	0,    //80
	0,
	0,
	0,
	'è',
	'=',
	0,
	0,
	0xb2,    //caps lock
	0,    //RSHIFT
	'\n',   // CR (0x5A)
	'+',
	0,   // 
	'\\',   // (0x5D)
  0,      // Wake (con E0)
  0,
  
  0,    // 0x60
  0,
  0,
  0,
  0,
  0,    
  0x8,    // 0x66 = BKSP
  0,
  0,
  '1',		// end= 0x69
  0,
  '4',		// e cursor left = 0x6b
  '7',		// home= 0x6c
  0,
  0,
  0,
  
  '0',    //0x70, INS se E0
  '.',    // keypad ovvero DEL se E0 71
  '2',		// e cursor down = 0x72
  '5',
  '6',		// e cursor right = 0x74
  '8',		// e cursor up = 0x75
  0x1b,   //0x76=ESC   0xe0 prefix! ...=RCTRL
  0xb1,    // NumLock
  0xab,    // F11
  '+',
  '3',
  '-',
  '*',
  '9',
  0xb3,  //0x7e scroll lock
  0,
  
	0,
	0,
	0,
	0xA7,   //0x83 è F7 !!

	},
  
  {			// 3°
	0,
  0xa9,    //F9
  0,
  0xa5,		 //F5
  0xa3,    //F3
  0xa1,    //F1  = 0x05
  0xa2,    //F2
  0xac,    //F12
  0,
  0xaa,    //F10
  0xa8,    //F8
  0xa6,    //F6
  0xa4,    //F4
  9,      //TAB=0xD
  '\'',     // | 
  0,
  
  0,    //16
  0,    //LALT=0x11
  0,    //LSHIFT=0x12 
  0,
  0,    //LCTRL=0x14 
	'a',    // prove :) AZERTY
	'1',
	0,
	0,
	0,
	'w',
	's',
	'q',
	'z',
	'2',
	0,      // left WIN (con E0)
	0,    //32
	'c',
	'x',
	'd',
	'e',
	'4',
	'3',
	0,      // right WIN (con E0)
	0,
	' ',    //0x29
	'v',
	'f',
	't',
	'r',
	'5',
	0,
  
	0,    //48
	'n',
	'b',
	'h',
	'g',
	'y',
	'6',
	0,   // Power (con E0)
	0,
	0,
	'm',
	'j',
	'u',
	'7',
	'8',
	0,   // Sleep (con E0)
  
	0,    //64
	',',
	'k',
	'i',
	'o',
	'0',
	'9',
	0,
	0,
	'.',
	'/',	
	'l',
	'@',		// sarebbe ò ossia @
	'p',
	'-',
	'#',		// sarebbe à ossia #
  
	0,    //80
	0,
	0,
	0,
	'[',
	'=',
	0,
	0,
	0xb2,    //caps lock
	0,    //RSHIFT
	'\n',   // CR (0x5A)
	']',
	0,   // 
	'\\',   // (0x5D)
  0,      // Wake (con E0)
  0,
  
  0,    // 0x60
  0,
  0,
  0,
  0,
  0,    
  0x8,    // 0x66 = BKSP
  0,
  0,
  '1',		// end= 0x69
  0,
  '4',		// e cursor left = 0x6b
  '7',		// home= 0x6c
  0,
  0,
  0,
  
  '0',    //0x70, INS se E0
  '.',    // keypad ovvero DEL se E0 71
  '2',		// e cursor down = 0x72
  '5',
  '6',		// e cursor right = 0x74
  '8',		// e cursor up = 0x75
  0x1b,   //0x76=ESC   0xe0 prefix! ...=RCTRL
  0xb1,    // NumLock
  0xab,    // F11
  '+',
  '3',
  '-',
  '*',
  '9',
  0xb3,  //0x7e scroll lock
  0,
  
	0,
	0,
	0,
	0xA7,   //0x83 è F7 !!

	},
  
  {			// 4°
	0,
  0xa9,    //F9
  0,
  0xa5,		 //F5
  0xa3,    //F3
  0xa1,    //F1  = 0x05
  0xa2,    //F2
  0xac,    //F12
  0,
  0xaa,    //F10
  0xa8,    //F8
  0xa6,    //F6
  0xa4,    //F4
  9,      //TAB=0xD
  '\'',     // | 
  0,
  
  0,    //16
  0,    //LALT=0x11
  0,    //LSHIFT=0x12 
  0,
  0,    //LCTRL=0x14 
	'q',
	'1',
	0,
	0,
	0,
	'z',
	's',
	'a',
	'w',
	'2',
	0,      // left WIN (con E0)
	0,    //32
	'c',
	'x',
	'd',
	'e',
	'4',
	'3',
	0,      // right WIN (con E0)
	0,
	' ',    //0x29
	'v',
	'f',
	't',
	'r',
	'5',
	0,
  
	0,    //48
	'n',
	'b',
	'h',
	'g',
	'y',
	'6',
	0,   // Power (con E0)
	0,
	0,
	'm',
	'j',
	'u',
	'7',
	'8',
	0,   // Sleep (con E0)
  
	0,    //64
	',',
	'k',
	'i',
	'o',
	'0',
	'9',
	0,
	0,
	'.',
	'/',	
	'l',
	'@',		// sarebbe ò ossia @
	'p',
	'-',
	'#',		// sarebbe à ossia #
  
	0,    //80
	0,
	0,
	0,
	'[',
	'=',
	0,
	0,
	0xb2,    //caps lock
	0,    //RSHIFT
	'\n',   // CR (0x5A)
	']',
	0,   // 
	'\\',   // (0x5D)
  0,      // Wake (con E0)
  0,
  
  0,    // 0x60
  0,
  0,
  0,
  0,
  0,    
  0x8,    // 0x66 = BKSP
  0,
  0,
  '1',		// end= 0x69
  0,
  '4',		// e cursor left = 0x6b
  '7',		// home= 0x6c
  0,
  0,
  0,
  
  '0',    //0x70, INS se E0
  '.',    // keypad ovvero DEL se E0 71
  '2',		// e cursor down = 0x72
  '5',
  '6',		// e cursor right = 0x74
  '8',		// e cursor up = 0x75
  0x1b,   //0x76=ESC   0xe0 prefix! ...=RCTRL
  0xb1,    // NumLock
  0xab,    // F11
  '+',
  '3',
  '-',
  '*',
  '9',
  0xb3,  //0x7e scroll lock
  0,
  
	0,
	0,
	0,
	0xA7,   //0x83 è F7 !!

	}
  };


BYTE App_PS22ASCII(BYTE a) {	//convert PS/2 code to ASCII code
  BYTE AsciiVal;
  BYTE ShiftkeyStatus,AltkeyStatus;

  //ovvio layout italiano :) KBIT se 850 ossia arr=1 altrimenti KBUS=437
  ShiftkeyStatus = getModifierKeys() & 0b00100010;
  AltkeyStatus = getModifierKeys() & 0b01000100;
  AsciiVal=ps2_2_ascii[codePageArr][a];
  
#warning finire di gestire codePageArr
  
  if(AsciiVal>='a' && AsciiVal<='z') {
    if(((CAPS_Lock_Pressed()) && !(getModifierKeys() & 0b00100010))
      || ((!CAPS_Lock_Pressed()) && (getModifierKeys() & 0b00100010)))
      AsciiVal &= ~0x20;   // capital
    else
      AsciiVal |= 0x20;   // lower case
    }
  else {
  	switch(a) {
      case 0x4a:
        if(!ShiftkeyStatus)
          AsciiVal =  codePageArr ? '-' : '-';
        else
          AsciiVal =  codePageArr ? '_' : '_';
        break;                      
      case 0x46:
        if(!ShiftkeyStatus)
          AsciiVal = '9';
        else
          AsciiVal =  codePageArr ? ')' : ')';
        break;
      case 0x45:
        if(!ShiftkeyStatus)
          AsciiVal = '0';
        else
          AsciiVal =  codePageArr ? '=' : '=';
        break;
      case 0x16:
        if(!ShiftkeyStatus)
          AsciiVal = '1';
        else
          AsciiVal = '!';
        break;
      case 0x1e:
        if(!ShiftkeyStatus)
          AsciiVal = '2';
        else
          AsciiVal = '"';
        break;
      case 0x26:
        if(!ShiftkeyStatus)
          AsciiVal = '3';
        else
          AsciiVal = codePageArr ? '£' : '#';
        break;
      case 0x25:
        if(!ShiftkeyStatus)
          AsciiVal = '4';
        else
          AsciiVal = '$';
        break;
      case 0x2e:
        if(!ShiftkeyStatus)
          AsciiVal = '5';
        else
          AsciiVal = '%';
        break;
      case 0x36:
        if(!ShiftkeyStatus)
          AsciiVal = '6';
        else
          AsciiVal = codePageArr ? '&' :  '&';
        break;
      case 0x3d:
        if(!ShiftkeyStatus)
          AsciiVal = '7';
        else
          AsciiVal = codePageArr ? '/' : '/';
        break;
      case 0x3e:
        if(!ShiftkeyStatus)
          AsciiVal = '8';
        else
          AsciiVal = codePageArr ? '(' : '(';
        break;
        
      case 0x61:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '<' : '<';
        else
          AsciiVal = codePageArr ? '>' : '>';
        break;
      case 0xe:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '\\' : '\\';
        else
          AsciiVal = codePageArr ? '|' : '|'; 
        break;
      case 0x41:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? ',' : ',';
        else
          AsciiVal = codePageArr ? ';' : ';';
        break;
      case 0x49:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '.' : '.';
        else
          AsciiVal = codePageArr ? ':' : ':';
        break;
/*      case 0x8b: cosa sono??
        if(!ShiftkeyStatus)
          AsciiVal = '+';
        else
          AsciiVal = '*';            
        break;*/
      case 0x8d:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'ù' : '#';
        else
          AsciiVal = codePageArr ? '§' : '~';
        break;
      case 0x55:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'ì' : 'ì';
        else
          AsciiVal = codePageArr ? '^' : '^'; 
        break;
      case 0x4e:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '\'' : '\'';
        else
          AsciiVal = codePageArr ? '?' : '?';
        break;
      case 0x4c:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'ò' : ';';
        else
          AsciiVal = codePageArr ? (AltkeyStatus ? '@' : 'ç') : ':';
        break;
      case 0x52:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'à' : '\'';
        else
          AsciiVal = codePageArr ? (AltkeyStatus ? '#' : '°') : '@';
        break;
      case 0x54:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? 'è' : '[';
        else
          AsciiVal = codePageArr ? '[' : '{';
        break;
      case 0x5b:
        if(!ShiftkeyStatus)
          AsciiVal = codePageArr ? '+' : ']';
        else
          AsciiVal = codePageArr ? '*' : '}';
        break;
//      case 0xb3:			// CTRL-BREAK arriva direttamente così...
//        AsciiVal = 0x9f;
//        break;
      default:
        break;
      }
    }

  return AsciiVal;
	}


/****************************************************************************
  Function:
    BOOL USB_HID_DataCollectionHandler(void)
  Description:
    This function is invoked by HID client , purpose is to collect the 
    details extracted from the report descriptor. HID client will store
    information extracted from the report descriptor in data structures.
    Application needs to create object for each report type it needs to 
    extract.
    For ex: HID_DATA_DETAILS Appl_ModifierKeysDetails;
    HID_DATA_DETAILS is defined in file usb_host_hid_appl_interface.h
    Each member of the structure must be initialized inside this function.
    Application interface layer provides functions :
    USBHostHID_ApiFindBit()
    USBHostHID_ApiFindValue()
    These functions can be used to fill in the details as shown in the demo
    code.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TRUE    - If the report details are collected successfully.
    FALSE   - If the application does not find the the supported format.

  Remarks:
    This Function name should be entered in the USB configuration tool
    in the field "Parsed Data Collection handler".
    If the application does not define this function , then HID client 
    assumes that Application is aware of report format of the attached
    device.
***************************************************************************/
BOOL USB_HID_DataCollectionHandlerKB(void) {
  BYTE NumOfReportItem = 0;
  BYTE i;
  USB_HID_ITEM_LIST* pitemListPtrs;
  USB_HID_DEVICE_RPT_INFO* pDeviceRptinfo;
  HID_REPORTITEM *reportItem;
  HID_USAGEITEM *hidUsageItem;
  BYTE usageIndex;
  BYTE reportIndex;
  BOOL foundLEDIndicator = FALSE;
  BOOL foundModifierKey = FALSE;
  BOOL foundNormalKey = FALSE;


  pDeviceRptinfo = USBHostHID_GetCurrentReportInfo(); // Get current Report Info pointer
  pitemListPtrs = USBHostHID_GetItemListPointers();   // Get pointer to list of item pointers

  BOOL status = FALSE;
  /* Find Report Item Index for Modifier Keys */
  /* Once report Item is located , extract information from data structures provided by the parser */
  NumOfReportItem = pDeviceRptinfo->reportItems;
  for(i=0; i<NumOfReportItem; i++) {
    reportItem = &pitemListPtrs->reportItemList[i];
    if(reportItem->reportType==hidReportInput && reportItem->dataModes == HIDData_Variable &&
      reportItem->globals.usagePage==USAGE_PAGE_KEY_CODES) {
      /* We now know report item points to modifier keys */
      /* Now make sure usage Min & Max are as per application */
      usageIndex = reportItem->firstUsageItem;
      hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];
      if(hidUsageItem->usageMinimum == USAGE_MIN_MODIFIER_KEY
        && hidUsageItem->usageMaximum == USAGE_MAX_MODIFIER_KEY) //else application cannot support
        {
        reportIndex = reportItem->globals.reportIndex;
        Appl_ModifierKeysDetails.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
        Appl_ModifierKeysDetails.reportID = (BYTE)reportItem->globals.reportID;
        Appl_ModifierKeysDetails.bitOffset = (BYTE)reportItem->startBit;
        Appl_ModifierKeysDetails.bitLength = (BYTE)reportItem->globals.reportsize;
        Appl_ModifierKeysDetails.count=(BYTE)reportItem->globals.reportCount;
        Appl_ModifierKeysDetails.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
        foundModifierKey = TRUE;
     		}

      }
    else if((reportItem->reportType==hidReportInput) && (reportItem->dataModes == HIDData_Array) &&
    	(reportItem->globals.usagePage==USAGE_PAGE_KEY_CODES)) {
        /* We now know report item points to modifier keys */
        /* Now make sure usage Min & Max are as per application */
      usageIndex = reportItem->firstUsageItem;
      hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];
      if(hidUsageItem->usageMinimum == USAGE_MIN_NORMAL_KEY
        && hidUsageItem->usageMaximum <= USAGE_MAX_NORMAL_KEY) //else application cannot support
        {
        reportIndex = reportItem->globals.reportIndex;
        Appl_NormalKeysDetails.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
        Appl_NormalKeysDetails.reportID = (BYTE)reportItem->globals.reportID;
        Appl_NormalKeysDetails.bitOffset = (BYTE)reportItem->startBit;
        Appl_NormalKeysDetails.bitLength = (BYTE)reportItem->globals.reportsize;
        Appl_NormalKeysDetails.count=(BYTE)reportItem->globals.reportCount;
        Appl_NormalKeysDetails.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
        foundNormalKey = TRUE;
        }

      }
    else if(reportItem->reportType==hidReportOutput &&
    	reportItem->globals.usagePage==USAGE_PAGE_LEDS) {
      usageIndex = reportItem->firstUsageItem;
      hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];

      reportIndex = reportItem->globals.reportIndex;
      Appl_LED_Indicator.reportLength = (pitemListPtrs->reportList[reportIndex].outputBits + 7)/8;
      Appl_LED_Indicator.reportID = (BYTE)reportItem->globals.reportID;
      Appl_LED_Indicator.bitOffset = (BYTE)reportItem->startBit;
      Appl_LED_Indicator.bitLength = (BYTE)reportItem->globals.reportsize;
      Appl_LED_Indicator.count=(BYTE)reportItem->globals.reportCount;
      Appl_LED_Indicator.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
      foundLEDIndicator = TRUE;
      }

    }		// for


   if(pDeviceRptinfo->reports == 1) {
    Appl_raw_report_buffer.Report_ID = 0;
    Appl_raw_report_buffer.ReportSize = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
    Appl_raw_report_buffer.ReportPollRate = pDeviceRptinfo->reportPollingRate;
    if(foundNormalKey == TRUE && foundModifierKey == TRUE)
    	status = TRUE;
    }

  return status;
	}

/****************************************************************************
  Function:
     void InitializeTimer( void )

  Description:
    This function initializes the tick timer.  It configures and starts the
    timer, and enables the timer interrupt.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void InitializeTimer(void) {
  WORD timerPeriod;


  TMR3 = 0;
  timerPeriod = TIMER_PERIOD*(Appl_raw_report_buffer.ReportPollRate/MINIMUM_POLL_INTERVAL);
      // adjust the timer prescaler if poll rate is too high
      // 20000 counts correspond to 10ms , so current prescaler setting will 
      // allow maximum 30ms poll interval


  T3CONbits.TCS = 0;                  // clock from peripheral clock
  T3CONbits.TCKPS = 0b01 /*0b01*/;             // 1:8 prescale PROVO 256, USB disturba la PS/2... 6/2/23 fa un cazzo :(
  PR3=timerPeriod;                    // 
  
  IFS0bits.T3IF=0;
  IEC0bits.T3IE=1;
  IPC2bits.T3IP=3;
//  IPC2bits.T3IP = TIMER_INTERRUPT_PRIORITY;
//  IFS0bits.T3IF = 0;
  
  T3CONbits.TON = 1;                  // start timer 

  }

/****************************************************************************
  Function:
    void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void)

  Description:
    Timer ISR, used to update application state. If no transfers are pending
    new input request is scheduled.
  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt( void ) {
#warning si potrebbe usare il timer 2 (clock) anche per questo...
  
  IFS0bits.T3IF   = 0;
  if(App_State_USB==READY_TO_TX_RX_REPORT) {
    App_State_USB = GET_INPUT_REPORT; // If no report is pending schedule new request
    }
  
//  m_PSelP^=1;      // 10mS, 9/7/22 con prescaler 8; prova prescaler 256 => 312mS (corretto) 26/2/23
	}



/****************************************************************************
  Function:
    BOOL USB_HID_DataCollectionHandler(void)
  Description:
    This function is invoked by HID client , purpose is to collect the 
    details extracted from the report descriptor. HID client will store
    information extracted from the report descriptor in data structures.
    Application needs to create object for each report type it needs to 
    extract.
    For ex: HID_DATA_DETAILS Appl_ModifierKeysDetails;
    HID_DATA_DETAILS is defined in file usb_host_hid_appl_interface.h
    Each member of the structure must be initialized inside this function.
    Application interface layer provides functions :
    USBHostHID_ApiFindBit()
    USBHostHID_ApiFindValue()
    These functions can be used to fill in the details as shown in the demo
    code.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TRUE    - If the report details are collected successfully.
    FALSE   - If the application does not find the the supported format.

  Remarks:
    This Function name should be entered in the USB configuration tool
    in the field "Parsed Data Collection handler".
    If the application does not define this function , then HID client 
    assumes that Application is aware of report format of the attached
    device.
***************************************************************************/
BOOL USB_HID_DataCollectionHandlerMouse(void) {
  BYTE NumOfReportItem = 0;
  BYTE i;
  USB_HID_ITEM_LIST* pitemListPtrs;
  USB_HID_DEVICE_RPT_INFO* pDeviceRptinfo;
  HID_REPORTITEM *reportItem;
  HID_USAGEITEM *hidUsageItem;
  BYTE usageIndex;
  BYTE reportIndex;

  pDeviceRptinfo = USBHostHID_GetCurrentReportInfo(); // Get current Report Info pointer
  pitemListPtrs = USBHostHID_GetItemListPointers();   // Get pointer to list of item pointers

  BOOL status = FALSE;
   /* Find Report Item Index for Modifier Keys */
   /* Once report Item is located , extract information from data structures provided by the parser */
  NumOfReportItem = pDeviceRptinfo->reportItems;
  for(i=0; i<NumOfReportItem; i++) {
    reportItem = &pitemListPtrs->reportItemList[i];
    if(reportItem->reportType==hidReportInput && reportItem->dataModes == (HIDData_Variable|HIDData_Relative) &&
      reportItem->globals.usagePage==USAGE_PAGE_GEN_DESKTOP) {
       /* Now make sure usage Min & Max are as per application */
        usageIndex = reportItem->firstUsageItem;
        hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];

        reportIndex = reportItem->globals.reportIndex;
        Appl_XY_Axis_Details.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
        Appl_XY_Axis_Details.reportID = (BYTE)reportItem->globals.reportID;
        Appl_XY_Axis_Details.bitOffset = (BYTE)reportItem->startBit;
        Appl_XY_Axis_Details.bitLength = (BYTE)reportItem->globals.reportsize;
        Appl_XY_Axis_Details.count=(BYTE)reportItem->globals.reportCount;
        Appl_XY_Axis_Details.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
        }
    else if(reportItem->reportType==hidReportInput && reportItem->dataModes == HIDData_Variable &&
      reportItem->globals.usagePage==USAGE_PAGE_BUTTONS) {
       /* We now know report item points to modifier keys */
       /* Now make sure usage Min & Max are as per application */
        usageIndex = reportItem->firstUsageItem;
        hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];

        reportIndex = reportItem->globals.reportIndex;
        Appl_Mouse_Buttons_Details.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
        Appl_Mouse_Buttons_Details.reportID = (BYTE)reportItem->globals.reportID;
        Appl_Mouse_Buttons_Details.bitOffset = (BYTE)reportItem->startBit;
        Appl_Mouse_Buttons_Details.bitLength = (BYTE)reportItem->globals.reportsize;
        Appl_Mouse_Buttons_Details.count=(BYTE)reportItem->globals.reportCount;
        Appl_Mouse_Buttons_Details.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
        }
      else if(reportItem->reportType==hidReportInput && 
        reportItem->globals.usagePage==USAGE_PAGE_CONSUMER) {
        Nop();
        }
    	}

  if(pDeviceRptinfo->reports == 1 || pDeviceRptinfo->reports == 3 /* mouse usb.. 31/10/21*/
          || pDeviceRptinfo->reports == 2 /* mouse gaming colorato 31/10/21*/) {
    Appl_raw_report_buffer.Report_ID = 0;
    Appl_raw_report_buffer.ReportSize = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
	  Appl_raw_report_buffer.ReportPollRate = pDeviceRptinfo->reportPollingRate;
	  status = TRUE;
    }

  
//    m_PReset ^=1;   // con 256 prescaler siamo a 700/300mS ... asimmetrico??

  return status;
	}

//******************************************************************************
// USB Support Functions
//******************************************************************************
//******************************************************************************

  