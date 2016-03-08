//*****************************************************************************
// main.c
//
// Application that controls the InMoov 3D printed robotic arm.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// This source file uses code snippets from Texas Instruments Incorporated's
// CC3200-LAUNCHXL sample projects. Copyright notice is moved to the end of
// this file.
//
// Created: December 21, 2015
//
// Modified: March 5, 2016
//
//*****************************************************************************
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
#include "tcp_socket.h"
#include "servo_driver_if.h"
#include "servo_driver_search_pressure_if.h"
#include "adc_driver_if.h"
//#include "adc_break_out_if.h"
#include "i2c_if.h"

#include "pwm_break_out_if.h"
#include "adc_break_out_if.h"

// HTTP Client lib
#include <http/client/httpcli.h>
#include <http/client/common.h>
#include "hircp.h"

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
    Report("\t **************************************************************\n\r");
    Report("\t      HUMAN INTERFACE FOR ROBOTIC CONTROL       \n\r", AppName);
    Report("\t      \n\r");
    Report("\t      Copyright (c) 2015-2016 Brandon To, Minh Mai, Yuzhou Liu \n\r");
    Report("\t **************************************************************\n\r");
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
    unsigned char recv_data[HIRCP_MAX_PACKET_LEN];
    unsigned char send_data[HIRCP_MAX_PACKET_LEN];
    unsigned char recv_payload[HIRCP_MAX_PAYLOAD_LEN];
    unsigned char send_payload[HIRCP_MAX_PAYLOAD_LEN];
    unsigned char highByte, lowByte;
    unsigned short adc_reading;
    int i;
    int freeMovementMode = 1;		// TODO Testing, remove later
    tBoolean connected;

    HIRCP_Packet *sendPacket = HIRCP_CreatePacket();
    HIRCP_Packet *recvPacket = HIRCP_CreatePacket();

    // Zeroes out memory buffers
    memset(recv_data, 0, HIRCP_MAX_PACKET_LEN);
    memset(send_data, 0, HIRCP_MAX_PACKET_LEN);

    // Board Initialization
    BoardInit();

    // Configure the pinmux settings for the peripherals exercised
    PinMuxConfig();

    // Configuring UART
    InitTerm();

    // Display banner
    DisplayBanner(APPLICATION_NAME);

    // Configure I2C
    I2C_IF_Open(I2C_MASTER_MODE_STD);

    // Initialize the PWM outputs on the board
    InitServos_PWM_Breakout();

    // Initialize the sensor ADC
    InitSensorADC();

    // Connect to WIFI using default info

    WlanConnect("Nagui's Network", "SL_SEC_TYPE_WPA", "19520605");
    //WlanConnect("NETGEAR31", "SL_SEC_TYPE_WPA", "happystar329");
    //WlanConnect("Minh's iPhone", "SL_SEC_TYPE_WPA", "minh1234");

    // Setup the TCP Server Socket for listening
    BsdTcpServerSetup(PORT_NUM);

    while (TRUE)
    {
        connected = false;
        while (!connected)
        {
            // Accept incoming connections
            if (BsdTcpServerAccept() != SUCCESS)
            {
                UART_PRINT("Accept() failed.n\r");
                continue;
            }

            if (!HIRCP_InitiateConnectionSequence())
            {
                UART_PRINT("Received invalid packet... aborting connection\n\r");
                continue;
            }

            connected = true;
        }

        while (lRetVal >= 0)
        {
            // Receive packet data
            lRetVal = BsdTcpServerReceive(recv_data, HIRCP_MAX_PACKET_LEN);
            if (lRetVal < 0)
            {
            	break;
            }

            // Populates packet structure and checks validity
            HIRCP_Populate(recvPacket, recv_data, HIRCP_MAX_PACKET_LEN);
            if (HIRCP_IsValid(recvPacket))
            {
                if (HIRCP_GetType(recvPacket) == HIRCP_DATA)
                {
                    // Get packet payload
                    HIRCP_GetPayload(recvPacket, recv_payload, HIRCP_MAX_PAYLOAD_LEN);
                    UART_PRINT("Received DATA packet.\n\r");
                }
                else if (HIRCP_GetType(recvPacket) == HIRCP_TRQ)
                {
                    UART_PRINT("Received TRQ packet.\n\r");
                    HIRCP_InitiateTerminationSequence();
                    break;
                }
                else
                {
                    // TODO (Brandon): Handle invalid types
                    continue;
                }
            }
            else
            {
                // TODO (Brandon): Handle invalid packets
                continue;
            }

            if (freeMovementMode)
            {
                // Moves servo motors using data from packet
                for (i = 0; i<NUM_SERVOS; i++)
                {
                    //MoveServo_PWM_Breakout((unsigned char)recv_payload[i], (enum Servo_Joint_Type)i);
                }

                // Gets pressure readings from sensors and populates buffer to be used as payload for sending
                for (i = 0; i<NUM_SENSORS; i++)
                {
                    //adc_reading = GetSensorReading((enum Fingertip_Sensor_Type)i);
                    adc_reading = 2048;
                    UART_PRINT("Finger: %d, reading: %d,\n\r", i, adc_reading);

                    UnsignedShort_to_UnsignedChar(adc_reading, &highByte, &lowByte);
                    send_payload[i*2] = (char)highByte;
                    send_payload[i*2+1] = (char)lowByte;
                }
            }
            else
            {
                MoveServo_SearchPressure(CMD_CLOSE, send_payload);
            }

            // Configure packet fields and gets packet data to send
            HIRCP_SetType(sendPacket, HIRCP_DACK);
            HIRCP_SetPayload(sendPacket, send_payload, HIRCP_MAX_PAYLOAD_LEN);
            HIRCP_GetData(sendPacket, send_data, HIRCP_MAX_PACKET_LEN);

            // Sends data
            lRetVal = BsdTcpServerSend(send_data, HIRCP_MAX_PACKET_LEN);
            if (lRetVal < 0)
            {
            	break;
            }
            UART_PRINT("Sent DACK packet.\n\r");
        }
    }

    // Frees resources held by HIRCP_Packet
    HIRCP_DestroyPacket(sendPacket);
    HIRCP_DestroyPacket(recvPacket);

    // Closes listening socket
    BsdTcpServerClose();

    // Power off the network processor
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
