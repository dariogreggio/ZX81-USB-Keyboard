/*
********************************************************************************
                                                                                
Software License Agreement                                                      
                                                                                
Copyright � 2007-2008 Microchip Technology Inc. and its licensors.  All         
rights reserved.                                                                
                                                                                
Microchip licenses to you the right to: (1) install Software on a single        
computer and use the Software with Microchip 16-bit microcontrollers and        
16-bit digital signal controllers ("Microchip Product"); and (2) at your        
own discretion and risk, use, modify, copy and distribute the device            
driver files of the Software that are provided to you in Source Code;           
provided that such Device Drivers are only used with Microchip Products         
and that no open source or free software is incorporated into the Device        
Drivers without Microchip's prior written consent in each instance.             
                                                                                
You should refer to the license agreement accompanying this Software for        
additional information regarding your rights and obligations.                   
                                                                                
                             
********************************************************************************
*/

// Created by the Microchip USBConfig Utility, Version 1.0.4.0, 4/25/2008, 17:05:22

#ifndef _usb_config_h_
#define _usb_config_h_

#if defined(__PIC24F__)
    #include <p24fxxxx.h>
#elif defined(__dsPIC33EP512MU810__)
    #include <p33Exxxx.h>
#elif defined(__PIC24EP512GU810__)
    #include <p24Exxxx.h>
#elif defined(__18CXX)
    #include <p18cxxx.h>
#elif defined(__PIC32MX__)
    #include <p32xxxx.h>
    #include "plib.h"
#elif defined(__PIC32MZ__)
    #include <p32xxxx.h>
#else
    #error No processor header file.
#endif

#define _USB_CONFIG_VERSION_MAJOR 1
#define _USB_CONFIG_VERSION_MINOR 0
#define _USB_CONFIG_VERSION_DOT   11
#define _USB_CONFIG_VERSION_BUILD 0

// Supported USB Configurations

#define USB_SUPPORT_HOST

// Hardware Configuration

#define USB_PING_PONG_MODE  USB_PING_PONG__FULL_PING_PONG

// Host Configuration

#define NUM_TPL_ENTRIES 2
#define USB_NUM_CONTROL_NAKS 200
#define USB_SUPPORT_INTERRUPT_TRANSFERS
#define USB_NUM_INTERRUPT_NAKS 20
#define USB_SUPPORT_BULK_TRANSFERS
#define USB_NUM_BULK_NAKS 20000
#define USB_INITIAL_VBUS_CURRENT (100/2)
#define USB_INSERT_TIME (250+1)
#define USB_HOST_APP_EVENT_HANDLER USB_ApplicationEventHandler

#define USB_MAX_PRINTER_DEVICES 1
#define USB_PRINTER_TRANSFER_QUEUE_SIZE 1
#define USB_PRINTER_LANGUAGE_POSTSCRIPT
#define USB_PRINTER_LANGUAGE_PCL_5
#define USB_PRINTER_LANGUAGE_ESCPOS
#define USB_PRINTER_ALLOW_DYNAMIC_LANGUAGE_DETERMINATION
#define USB_PRINTER_POS_IMAGE_LINE_SPACING 8        // per ESC/POS

#define USB_ENABLE_TRANSFER_EVENT
// #define USB_HID_ENABLE_TRANSFER_EVENT

#define TPL_KEYBOARD1 1
#define TPL_KEYBOARD2 2

// Host HID Client Driver Configuration

#define USB_MAX_HID_DEVICES 1
#define HID_MAX_DATA_FIELD_SIZE 8
//#define APPL_COLLECT_PARSED_DATA USB_HID_DataCollectionHandler

// Helpful Macros

#define USBTasks()                  \
    {                               \
        USBHostTasks();             \
       /* USBHostHIDTasks()*/;          \
    }
//#warning VERIFICARE USBHostHIDTasks!
// v. USB_ENABLE_TRANSFER_EVENT e USB_HID_ENABLE_TRANSFER_EVENT.. cos� va tutto cmq
#define USBInitialize(x)            \
    {                               \
        USBHostInit(x);             \
    }


#endif

