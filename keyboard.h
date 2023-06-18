/* 
 * File:   keyboard.h
 * Author: dario
 *
 * Created on 13 giugno 2022, 11.15
 */

#ifndef KEYBOARD_H
#define	KEYBOARD_H

#ifdef	__cplusplus
extern "C" {
#endif


extern HID_DATA_DETAILS Appl_ModifierKeysDetails;
extern HID_DATA_DETAILS Appl_NormalKeysDetails;
extern HID_USER_DATA_SIZE Appl_BufferModifierKeys[8];
extern BYTE PS2_BufferModifierKeys;
extern HID_REPORT_BUFFER     Appl_raw_report_buffer;
extern HID_LED_REPORT_BUFFER Appl_led_report_buffer;

extern HID_DATA_DETAILS Appl_LED_Indicator;
extern BOOL ReportBufferUpdated;
extern BYTE NumOfBytesRcvd;
void App_ProcessInputReport(void);
void App_PrepareOutputReport(void);
void App_Detect_Device(void);
void App_Clear_Data_Buffer(void);
BYTE App_HID2ASCII(BYTE); //convert USB HID code (buffer[2 to 7]) to ASCII code
BYTE App_PS22ASCII(BYTE); //convert PS/2 code to ASCII code

#define CAPS_Lock_Pressed() Appl_led_report_buffer.CAPS_LOCK
#define NUM_Lock_Pressed() Appl_led_report_buffer.NUM_LOCK

BYTE getModifierKeys(void);

/* Array index for modifier keys */
#define MODIFIER_LEFT_CONTROL           (0)
#define MODIFIER_LEFT_SHIFT             (1)
#define MODIFIER_LEFT_ALT               (2)
#define MODIFIER_LEFT_GUI               (3)
#define MODIFIER_RIGHT_CONTROL          (4)
#define MODIFIER_RIGHT_SHIFT            (5)
#define MODIFIER_RIGHT_ALT              (6)
#define MODIFIER_RIGHT_GUI              (7)



#ifdef	__cplusplus
}
#endif

#endif	/* KEYBOARD_H */

