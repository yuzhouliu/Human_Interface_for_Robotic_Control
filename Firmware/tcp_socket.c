//*****************************************************************************
// tcp_socket.c
//
// This implements TCP socket on TI CC3200 by providing those functions:
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

// HTTP Client lib
#include <http/client/httpcli.h>
#include <http/client/common.h>

#include "pin_mux_config.h"
//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
unsigned long  g_ulDestinationIp = IP_ADDR;
unsigned int   g_uiPortNum = PORT_NUM;
volatile unsigned long  g_ulPacketCount = TCP_PACKET_COUNT;
unsigned char  g_ucConnectionStatus = 0;
unsigned char  g_ucSimplelinkstarted = 0;
unsigned long  g_ulIpAddr = 0;
char g_cBsdBuf[BUF_SIZE];
int ServerSockID; //Hold Server Socket ID
int ServerNewSockID; // Hold New Server Socket ID for Client
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//                  HELPER FUNCTIONS -- Start
//*****************************************************************************
static void SetSocketVariables();
static long ConfigureSimpleLinkToDefaultState();
static inline _u8 SecurityTypeParser(char * cSecurityType);
static int ConnectToHTTPServer(HTTPCli_Handle httpClient);
static int HTTPGetMethod(HTTPCli_Handle httpClient, char * getRequest);
static int UpdateIPtoServer(char *ipAddr);//update the ipAddr to the Server
//*****************************************************************************
//                  HELPER FUNCTIONS -- End
//*****************************************************************************

//****************************************************************************
//
// Start the SimpleLink in Station Mode in order to connect to a WIFI Router
//!
//! \return     0 on success, -1 on error.
//!
//****************************************************************************
long WlanStart()
{
    long lRetVal;
    // Asumption is that the device is configured in station mode already
    // and it is in its default state
    //
    lRetVal = sl_Start(0, 0, 0);
    if (lRetVal < 0)
    {
        UART_PRINT("Failed to start the device \n\r");
        LOOP_FOREVER();
    }

    UART_PRINT("Device started as STATION \n\r");
    return SUCCESS;
}

//****************************************************************************
//
//!  \brief Connecting to a WLAN Accesspoint
//!
//!   This function connects to the WIFI network. User need to provides:
//!
//!   1. SSID - name of the WIFI network
//!   2. SecurityType - type of the WIFI network
//!   3. SecurityKey - Password of the WIFI network  
//!
//!   \param[in] SSID, Security Type, Security Key
//!
//!   \return     Status value
//!
//!   \warning    If the WLAN connection fails or we don't aquire an IP
//!            address, It will be stuck in this function forever.
//
//****************************************************************************
long WlanConnect(char *cSSID, char *cSecurityType, char*cSecurityKey)
{
    SlSecParams_t secParams = {0};// Hold WLAN informations
    long lRetVal = 0;
    char ipString[15];

    //Reset the simplelink before connecting the the network
    lRetVal = ConfigureSimpleLinkToDefaultState();

    // Start WLAN
    WlanStart();

    if (cSSID == NULL || cSecurityType == NULL || cSecurityKey == NULL)
    { // if user did not give the network info, use the default info
        secParams.Key = (signed char*)SECURITY_KEY;
        secParams.KeyLen = strlen(SECURITY_KEY);
        secParams.Type = SECURITY_TYPE;
        lRetVal = sl_WlanConnect((signed char*)SSID_NAME, \
                                    strlen(SSID_NAME), 0, &secParams, 0);
        UART_PRINT("Connecting to AP: %s ...\r\n",SSID_NAME);

    }
    else
    { // if user provide network info
        secParams.Key = (signed char*)cSecurityKey;
        secParams.KeyLen = strlen(cSecurityKey);
        secParams.Type = SecurityTypeParser(cSecurityType);
        lRetVal = sl_WlanConnect((signed char*)cSSID, strlen(cSSID), 0, &secParams, 0);
        UART_PRINT("Connecting to AP: %s ...\r\n",cSSID);
    }

    ASSERT_ON_ERROR(lRetVal);

    /* Wait */
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
    {
        // Wait for WLAN Event
#ifndef SL_PLATFORM_MULTI_THREADED
        _SlNonOsMainLoopTask();
#endif
    }
    sprintf(ipString,"%d.%d.%d.%d", 
                    SL_IPV4_BYTE(g_ulIpAddr,3),
                    SL_IPV4_BYTE(g_ulIpAddr,2),
                    SL_IPV4_BYTE(g_ulIpAddr,1),
                    SL_IPV4_BYTE(g_ulIpAddr,0));
    UART_PRINT("Connected to AP: %s \n\r",SSID_NAME);
    UART_PRINT("Device IP: %s\n\r\n\r", ipString);
    UpdateIPtoServer(ipString);
    return SUCCESS;
}
//****************************************************************************
//
//! \brief Opening a TCP server side socket
//!
//! This function opens a TCP socket in Listen mode
//! \param[in] port number on which the server will be listening on
//!
//! \return     0 on success, -1 on error.
//!
//! \note   This function will wait for an incoming connection till
//!                     one is established
//****************************************************************************
int BsdTcpServerSetup(unsigned short usPort)
{
    SlSockAddrIn_t  sAddr;
    SlSockAddrIn_t  sLocalAddr;
    int             iCounter;
    int             iAddrSize;
    int             iStatus;
    long            lNonBlocking = 1;

    //Set variables for the socket
    SetSocketVariables();

    // filling the buffer
    for (iCounter=0 ; iCounter<BUF_SIZE ; iCounter++)
    {
        g_cBsdBuf[iCounter] = (char)(iCounter % 10);
    }

    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)usPort);
    sLocalAddr.sin_addr.s_addr = 0;

    // creating a TCP socket
    ServerSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( ServerSockID < 0 )
    {
        // error
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
    }

    iAddrSize = sizeof(SlSockAddrIn_t);

    // binding the TCP socket to the TCP server address
    iStatus = sl_Bind(ServerSockID, (SlSockAddr_t *)&sLocalAddr, iAddrSize);
    if( iStatus < 0 )
    {
        // error
        sl_Close(ServerSockID);
        ASSERT_ON_ERROR(BIND_ERROR);
    }

    // putting the socket for listening to the incoming TCP connection
    iStatus = sl_Listen(ServerSockID, 0);
    if( iStatus < 0 )
    {
        sl_Close(ServerSockID);
        ASSERT_ON_ERROR(LISTEN_ERROR);
    }

    // setting socket option to make the socket as non blocking
    iStatus = sl_SetSockOpt(ServerSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING,
                            &lNonBlocking, sizeof(lNonBlocking));
    if( iStatus < 0 )
    {
        sl_Close(ServerSockID);
        ASSERT_ON_ERROR(SOCKET_OPT_ERROR);
    }
    ServerNewSockID = SL_EAGAIN;

    UART_PRINT("SERVER SOCKET CREATED AT PORT: %d, SOCKID: %d.\n\r", usPort, ServerSockID);
    UART_PRINT("WAIT FOR CLIENT TO CONNECT....\n\r");

    // waiting for an incoming TCP connection
    while( ServerNewSockID < 0 )
    {
        // accepts a connection form a TCP client, if there is any
        // otherwise returns SL_EAGAIN
    	ServerNewSockID = sl_Accept(ServerSockID, ( struct SlSockAddr_t *)&sAddr,
                                (SlSocklen_t*)&iAddrSize);

        if( ServerNewSockID == SL_EAGAIN )
        {
           MAP_UtilsDelay(10000);
        }
        else if( ServerNewSockID < 0 )
        {
            // error
            sl_Close(ServerNewSockID);
            sl_Close(ServerSockID);
            ASSERT_ON_ERROR(ACCEPT_ERROR);
        }
    }
    UART_PRINT("CLIENT CONNECTED. new SOCKID: %d.\n\r",ServerNewSockID);
    return SUCCESS;
}

//****************************************************************************
//
//! \brief Recieving data from the client
//!
//! \param [in]: pointer to data, the recieving packet will be store in data
//!
//! \return     0 on success, -1 on error.
//!
//****************************************************************************
int BsdTcpServerReceive(char *data)
{
    int iStatus;

    iStatus = sl_Recv(ServerNewSockID, g_cBsdBuf, BUF_SIZE, 0);
    if( iStatus <= 0 )
    {
       // error
       sl_Close(ServerNewSockID);
       sl_Close(ServerSockID);
       ASSERT_ON_ERROR(RECV_ERROR);
    }
    else
    {
        UART_PRINT("RECEIVED BUFF:%s\n\r", g_cBsdBuf);
        strcpy(data, g_cBsdBuf);
    }
    return SUCCESS;
}

//****************************************************************************
//
//! \brief Sending data to the client
//!
//! \param [in]: pointer to data that need to be sent, length of data to be sent
//!
//! \return     0 on success, -1 on error.
//!
//****************************************************************************
int BsdTcpServerSend(char *data, int length)
{
	return sl_Send(ServerNewSockID, data, length, 0);
}

//****************************************************************************
//
//! \brief Close the TCP socket
//!
//! \return     0 on success, -1 on error.
//!
//****************************************************************************
int BsdTcpServerClose()
{
	int iStatus;
    // close the connected socket after receiving from connected TCP client
    iStatus = sl_Close(ServerNewSockID);
    ASSERT_ON_ERROR(iStatus);
    // close the listening socket
    iStatus = sl_Close(ServerSockID);
    ASSERT_ON_ERROR(iStatus);
    return SUCCESS;
}
//****************************************************************************
//                          UTILITY FUNCTIONS
//****************************************************************************
//****************************************************************************
//
//! \brief Update the IP address of the board to the server
//!
//! \return     0 on success, -1 on error.
//!
//****************************************************************************
static int UpdateIPtoServer(char *ipAddr)
{
    int lRetVal, count;
    char getRequest[60];
    HTTPCli_Struct httpClient;
    UART_PRINT("Update IP Address to remote server.\n\r");
    count = 0; //reset count
    lRetVal = -1;
    while (lRetVal < 0 && count < MAX_TRIAL)
    {
        lRetVal = ConnectToHTTPServer(&httpClient);
        count++;
    }
    if (lRetVal < 0) 
    {
        UART_PRINT("[ERROR] tcp_socket.c cannot connect to remote server.\n\r");
        return lRetVal;
    }
    /* Construct the get request message */
    sprintf(getRequest,"%s&ip=%s:%d", GET_REQUEST_URI, ipAddr, PORT_NUM);
    count = 0; // reset count
    lRetVal = -1;
    UART_PRINT("GET_REQUEST= %s.\n\r",getRequest);
    while (lRetVal < 0 && count < MAX_TRIAL)
    {
        lRetVal = HTTPGetMethod(&httpClient,getRequest);
        count++;
    }
    if(lRetVal < 0)
    {
        UART_PRINT("[ERROR] tcp_socket.c cannot update IP Address on server.\n\r");
        return lRetVal;
    }
    return SUCCESS;
}
//*****************************************************************************
//
//! Function to connect to HTTP server
//!
//! \param  httpClient - Pointer to HTTP Client instance
//!
//! \return Error-code or SUCCESS
//!
//*****************************************************************************
static int ConnectToHTTPServer(HTTPCli_Handle httpClient)
{
    long lRetVal = -1;
    struct sockaddr_in addr;
    unsigned long ServerIP;
    /* Resolve HOST NAME/IP */
    lRetVal = sl_NetAppDnsGetHostByName((signed char *)HOST_NAME,
                                          strlen((const char *)HOST_NAME),
                                          &ServerIP, SL_AF_INET);
    if(lRetVal < 0)
    {
        return lRetVal;
    }

    /* Set up the input parameters for HTTP Connection */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(HOST_PORT);
    addr.sin_addr.s_addr = sl_Htonl(ServerIP);

    /* Testing HTTPCli open call: handle, address params only */
    HTTPCli_construct(httpClient);
    lRetVal = HTTPCli_connect(httpClient, (struct sockaddr *)&addr, 0, NULL);
    if (lRetVal < 0)
    {
        UART_PRINT("[ERROR]tcp_socket.c connection to server failed\n\r");
        return lRetVal;
    }    
    return SUCCESS;
}
//*****************************************************************************
//
//! \brief HTTP GET
//!
//! \param[in]  httpClient - Pointer to http client
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
static int HTTPGetMethod(HTTPCli_Handle httpClient, char* getRequest)
{
  
    long lRetVal = 0;
    HTTPCli_Field fields[4] = {
                                {HTTPCli_FIELD_NAME_HOST, HOST_NAME},
                                {HTTPCli_FIELD_NAME_ACCEPT, "*/*"},
                                {HTTPCli_FIELD_NAME_CONTENT_LENGTH, "0"},
                                {NULL, NULL}
                            };
    bool        moreFlags;
    
    
    /* Set request header fields to be send for HTTP request. */
    HTTPCli_setRequestFields(httpClient, fields);

    /* Send GET method request. */
    /* Here we are setting moreFlags = 0 as there are no more header fields need to send
       at later stage. Please refer HTTP Library API documentaion @ HTTPCli_sendRequest
       for more information.
    */
    moreFlags = 0;
    lRetVal = HTTPCli_sendRequest(httpClient, HTTPCli_METHOD_GET, getRequest, moreFlags);
    if(lRetVal < 0)
    {
        UART_PRINT("[ERROR] tcp_socket.c failed to send HTTP GET request.\n\r");
        return lRetVal;
    }
    return SUCCESS;
}

//*****************************************************************************
//
//! This function convert string formatted Security Type into Integer values
//!
//! \param[in]    None
//!
//! \return None
//!
//*****************************************************************************
static inline _u8 SecurityTypeParser(char * cSecurityType)
{
    if (strcmp(cSecurityType,"SL_SEC_TYPE_OPEN") == 0) return SL_SEC_TYPE_OPEN;
    if (strcmp(cSecurityType,"SL_SEC_TYPE_WEP") == 0) return SL_SEC_TYPE_WEP;
    if (strcmp(cSecurityType, "SL_SEC_TYPE_WPA") == 0) return SL_SEC_TYPE_WPA;
    if (strcmp(cSecurityType,"SL_SEC_TYPE_WPA_WPA2") == 0) return SL_SEC_TYPE_WPA_WPA2;
    if (strcmp(cSecurityType,"SL_SEC_TYPE_WPS_PBC") == 0) return SL_SEC_TYPE_WPS_PBC;
    if (strcmp(cSecurityType,"SL_SEC_TYPE_WPS_PIN") == 0) return SL_SEC_TYPE_WPS_PIN;
    if (strcmp(cSecurityType,"SL_SEC_TYPE_WPA_ENT") == 0) return SL_SEC_TYPE_WPA_ENT;
    if (strcmp(cSecurityType,"SL_SEC_TYPE_P2P_PBC") == 0) return SL_SEC_TYPE_P2P_PBC;
    if (strcmp(cSecurityType,"SL_SEC_TYPE_P2P_PIN_KEYPAD") == 0) return SL_SEC_TYPE_P2P_PIN_KEYPAD;
    if (strcmp(cSecurityType,"SL_SEC_TYPE_P2P_PIN_DISPLAY") == 0) return SL_SEC_TYPE_P2P_PIN_DISPLAY;
    return SL_SEC_TYPE_OPEN;
}
//*****************************************************************************
//
//! This function set the socket variables
//!
//! \param[in]    None
//!
//! \return None
//!
//*****************************************************************************
static void SetSocketVariables()
{
    g_ulStatus = 0;
    g_ulGatewayIP = 0;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
    g_ulDestinationIp = IP_ADDR;
    g_uiPortNum = PORT_NUM;
    g_ulPacketCount = TCP_PACKET_COUNT;
}

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static long ConfigureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig 
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);

    

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore 
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    SetSocketVariables();
    
    return lRetVal; // Success
}
//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************


//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(!pWlanEvent)
    {
        return;
    }

    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'-Applications
            // can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //

            // Copy new connection SSID and BSSID to global parameters
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s ,"
                        " BSSID: %x:%x:%x:%x:%x:%x\n\r",
                      g_ucConnectionSSID,g_ucConnectionBSSID[0],
                      g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                      g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                      g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_USER_INITIATED_DISCONNECTION
            if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
                            "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(!pNetAppEvent)
    {
        return;
    }

    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
            g_ulIpAddr = pEventData->ip;

            //Gateway IP address
            g_ulGatewayIP = pEventData->gateway;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                        "Gateway=%d.%d.%d.%d\n\r",
                            SL_IPV4_BYTE(g_ulIpAddr,3),
                            SL_IPV4_BYTE(g_ulIpAddr,2),
                            SL_IPV4_BYTE(g_ulIpAddr,1),
                            SL_IPV4_BYTE(g_ulIpAddr,0),
                            SL_IPV4_BYTE(g_ulGatewayIP,3),
                            SL_IPV4_BYTE(g_ulGatewayIP,2),
                            SL_IPV4_BYTE(g_ulGatewayIP,1),
                            SL_IPV4_BYTE(g_ulGatewayIP,0));
        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    if(!pDevEvent)
    {
        return;
    }

    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status,
               pDevEvent->EventData.deviceEvent.sender);
}


//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(!pSock)
    {
        return;
    }

    //
    // This application doesn't work w/ socket - Events are not expected
    //
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status)
            {
                case SL_ECLOSE: 
                    UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n", 
                                    pSock->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default: 
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->socketAsyncEvent.SockTxFailData.sd, \
                                pSock->socketAsyncEvent.SockTxFailData.status);
                  break;
            }
            break;

        default:
            UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
          break;
    }

}
//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    // Unused in this application
}
//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************

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
