/*
********************************************************************************
                                                                                
Software License Agreement                                                      
                                                                                
Copyright © 2007-2008 Microchip Technology Inc. and its licensors.  All         
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

 * supporto multiplo tastiera/mouse/stampante, GD 2021-2022
 
********************************************************************************
*/

// Created by the Microchip USBConfig Utility, Version 1.0.4.0, 4/25/2008, 17:05:22

#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "usb.h"
#include "usb_host_hid.h"
#include "usb_host_printer.h"
#include "usb_host_printer_pcl_5.h"
#include "usb_host_printer_esc_pos.h"
#include "usb_host_printer_postscript.h"

BYTE TPLfound=0;

// *****************************************************************************
// Application Interface Function Pointer Table for the HID client driver
// *****************************************************************************

//CLIENT_DRIVER_TABLE usbDeviceInterfaceTable =
//{                                           
//    USBHostHID_APIInitialize,
//    USBHostHID_APIEventHandler,
//    0
//};

// *****************************************************************************
// Client Driver Function Pointer Table for the USB Embedded Host foundation
// *****************************************************************************

CLIENT_DRIVER_TABLE usbClientDrvTable[NUM_TPL_ENTRIES] =
{                                        
    {
        USBHostHIDInitialize,
        USBHostHIDEventHandler,
        0
    }
    ,
    {
        USBHostHIDInitialize,
        USBHostHIDEventHandler,
        0
    }
    ,
    {
        USBHostHIDInitialize,
        USBHostHIDEventHandler,
        0
    }
};


// *****************************************************************************
// USB Embedded Host Targeted Peripheral List (TPL)
// *****************************************************************************

USB_TPL usbTPL[NUM_TPL_ENTRIES] =
{
    { INIT_CL_SC_P( 3ul, 1ul, 1ul ), 0, 0, {TPL_CLASS_DRV} } // (null)    // keyboard
    ,
    { INIT_CL_SC_P( 3ul, 0ul, 0ul ), 0, 0, {TPL_CLASS_DRV} } // (null)    // keyboard2, credo
    
};


