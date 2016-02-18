//*****************************************************************************
// main.c
//
// main function to control the robot arm
//
// This source file uses code snippets from Texas Instruments Incorporated's
// CC3200-LAUNCHXL sample projects. Copyright notice is moved to the end of
// this file.
//
// Author: Minh Mai
//
// Created: Dec 21 2015
//
// Modified: January 6, 2015
//
//*****************************************************************************

#include "tcp_socket.h"

// Standard includes
#include <stdlib.h>
#include <string.h>

// simplelink includes 
#include "simplelink.h"
#include "wlan.h"

// driverlib includes 
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "rom.h"
#include "timer.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "uart.h"
#include "utils.h"

// common interface includes 
#include "udma_if.h"
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif

#include "pin_mux_config.h"
#include "servo_driver_if.h"
#include "adc_driver_if.h"
#include "msg_util_if.h"

/* Config for the TCP */
#define APPLICATION_NAME        "Human Interface for Robotic Control"
#define APPLICATION_VERSION     "0.0.1"
#define DBG_PRINT               Report

/***************************END OF TCP CONFIG ************************************/

//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//****************************************************************************
static void DisplayBanner();
static void BoardInit();

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs) || defined (gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************



//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void DisplayBanner(char * AppName)
{

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t      CC3200 %s Application       \n\r", AppName);
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}
//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs) || defined (gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//****************************************************************************
//                            MAIN FUNCTION
//****************************************************************************
void main()
{
    long lRetVal = 0;
    char data[BUF_SIZE];
    char sent_data[BUF_SIZE];
    unsigned char highByte, lowByte;
    int i;

    memset(sent_data, 0, 10);
    // Board Initialization
    BoardInit();

    // Configure the pinmux settings for the peripherals exercised
    PinMuxConfig();

    // Initialize the PWM outputs on the board
    InitServos();

    // Initialize the sensor ADC
    InitSensorADC();

    // Configuring UART
    InitTerm();

    // Display banner
    DisplayBanner(APPLICATION_NAME);

    // Connect to WIFI using default info
    //WlanConnect(NULL, NULL, NULL);
    WlanConnect("Nagui's Network", "SL_SEC_TYPE_WPA", "19520605");

    // Setup the TCP Server Socket
    BsdTcpServerSetup(PORT_NUM);

    // Recieve Data
    while (lRetVal >= 0)
    {
    	lRetVal = BsdTcpServerReceive(data);

    	for (i = 0; i<NUM_SERVOS; i++)
    	{
    		MoveServo((unsigned char)data[i], (enum Servo_Joint_Type)i);
    	}

    	for (i = 0; i< NUM_SENSORS; i++)
    	{
        	UnsignedShort_to_UnsignedChar(GetSensorReading((enum Fingertip_Sensor_Type)i), &highByte, &lowByte);
        	sent_data[i*2] = (char)highByte;
        	sent_data[i*2+1] = (char)lowByte;
    	}
    	lRetVal = BsdTcpServerSend(sent_data, 10);
    	UART_PRINT("Sent 10 bytes to client.\n\r");

    }
    UART_PRINT("Exiting Application ...\n\r");

    // power of the Network processor
    lRetVal = sl_Stop(SL_STOP_TIMEOUT);

}
//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
