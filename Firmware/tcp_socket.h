//*****************************************************************************
// tcp_socket.h
//
// TCP socket on TI CC3200 by providing those functions:
// 1. Connect to the WIFI network 
// 2. Create a server socket
// 3. Listen to client request
// 
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// This source file uses code snippets from Texas Instruments Incorporated's
// CC3200-LAUNCHXL sample projects. Copyright notice is moved to the end of
// this file.
//
// Author: Minh Mai
//
// Created: Dec 20 2015
//
// Modified: Dec 28 2015
//
//****************************************************************************
// simplelink includes
#include "simplelink.h"
#include "wlan.h"

/* Config for the WLAN */
#define SSID_NAME           "Minh's iPhone"    /* AP SSID */
#define SECURITY_TYPE       SL_SEC_TYPE_WPA /* Security type (OPEN or WEP or WPA)*/
#define SECURITY_KEY        "minh1234"              /* Password of the secured AP */
#define SSID_LEN_MAX        32
#define BSSID_LEN_MAX       6

/* Config for the TCP Socket Server */
#define IP_ADDR             0xc0a8006E /* Default IP: 192.168.0.110 */
#define PORT_NUM            5001 /*Default PORT_NUM */
#define BUF_SIZE            1400 /*Defaut BUFFER SIZE */
#define TCP_PACKET_COUNT    1000 /*Default TCP Packet Count */


/***************************END OF TCP CONFIG ************************************/

//****************************************************************************
//         	                    INTERFACE FUNCTIONS
//****************************************************************************
long WlanStart();//start the Wlan
long WlanConnect(char *cSSID, char *cSecurityType, \
                    char*cSecurityKey); //connect to the WIFI network
int BsdTcpServerSetup(unsigned short usPort); //Create a TCP server socket
int BsdTcpServerReceive(int *data);//Recieve data
int BsdTcpServerClose();//Close the TCP socket
int IpAddressParser(char *ucCMD);//convert string IP address to hex IP address

//*****************************************************************************
//                               CALLBACK FUNCTIONS
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent);
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent);
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent);
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock);
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse);
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

