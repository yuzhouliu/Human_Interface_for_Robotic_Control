//*****************************************************************************
//
// hircp.c
//
// HIRCP packet manipulation functions.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// March 4, 2016
//
// Modified:
// March 19, 2016
//
//*****************************************************************************
#include "hircp.h"

// Standard includes
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "tcp_socket.h"

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

const unsigned char HIRCP_CONSTANT[] = "HIRC";
HIRCP_Mode g_hircp_mode = HIRCP_NORMAL;

//
// Structure that stores HIRCP packet information
//
struct HIRCP_Packet
{
    HIRCP_Type _type;
    unsigned char _payload[HIRCP_MAX_PAYLOAD_LEN];
};

//*****************************************************************************
//
//! Allocates memory for an HIRCP_Packet structure.
//!
//! \param None.
//!
//! \return handle to a heap-allocated HIRCP_Packet structure.
//
//*****************************************************************************
HIRCP_Packet *HIRCP_CreatePacket(void)
{
    int i;
    HIRCP_Packet *handle;
    handle = (HIRCP_Packet*)malloc(sizeof(HIRCP_Packet));
    for (i=0; i<HIRCP_MAX_PAYLOAD_LEN; i++)
    {
        handle->_payload[i] = 0;
    }
    return handle;
}

//*****************************************************************************
//
//! Frees memory from an HIRCP_Packet structure.
//!
//! \param handle to an HIRCP_Packet structure.
//!
//! \return None.
//
//*****************************************************************************
void HIRCP_DestroyPacket(HIRCP_Packet *packet)
{
    if (packet != NULL)
    {
        free(packet);
    }
}

//*****************************************************************************
//
//! Clears packet data.
//!
//! \param handle to an HIRCP_Packet structure.
//!
//! \return None.
//
//*****************************************************************************
void HIRCP_ClearPacket(HIRCP_Packet *packet)
{
    int i;
    if (packet != NULL)
    {
        for (i=0; i<HIRCP_MAX_PAYLOAD_LEN; i++)
        {
            packet->_payload[i] = 0;
        }
        packet->_type = HIRCP_INVALID;
    }
}

//*****************************************************************************
//
//! Checks if the packet is well-formed.
//!
//! \param handle to an HIRCP_Packet structure.
//!
//! \return Returns \b true if the packet is well-formed and \b false
//! otherwise.
//
//*****************************************************************************
tBoolean HIRCP_IsValid(HIRCP_Packet *packet)
{
    //
    // TODO (Brandon): Implement
    //
    return 1;
}

//*****************************************************************************
//
//! Sets the HIRCP_Type of an HIRCP_Packet structure.
//!
//! \param handle to an HIRCP_Packet structure.
//! \param type the HIRCP_Type to set.
//!
//! \return None.
//
//*****************************************************************************
void HIRCP_SetType(HIRCP_Packet *packet, HIRCP_Type type)
{
    packet->_type = type;
}

//*****************************************************************************
//
//! Gets the HIRCP_Type of an HIRCP_Packet structure.
//!
//! \param handle to an HIRCP_Packet structure.
//!
//! \return Returns the HIRCP_Type of the packet structure.
//
//*****************************************************************************
HIRCP_Type HIRCP_GetType(HIRCP_Packet *packet)
{
    return packet->_type;
}

//*****************************************************************************
//
//! Sets the payload of an HIRCP_Packet structure.
//!
//! \param handle to an HIRCP_Packet structure.
//! \param payload buffer holding the payload.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
void HIRCP_SetPayload(HIRCP_Packet *packet, unsigned char *payload, int len)
{
    memcpy(packet->_payload, payload, len);
}

//*****************************************************************************
//
//! Gets the payload of an HIRCP_Packet structure.
//!
//! \param handle to an HIRCP_Packet structure.
//! \param payload buffer to store the payload.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
void HIRCP_GetPayload(HIRCP_Packet *packet, unsigned char *payload, int len)
{
    memcpy(payload, packet->_payload, HIRCP_MAX_PAYLOAD_LEN);
}

//*****************************************************************************
//
//! Gets the data of an HIRCP_Packet structure.
//!
//! \param handle to an HIRCP_Packet structure.
//! \param data buffer to store the data.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
void HIRCP_GetData(HIRCP_Packet *packet, unsigned char *data, int len)
{
    unsigned int offset = 0;
    memcpy(data, HIRCP_CONSTANT, HIRCP_CONSTANT_LEN);
    offset += HIRCP_CONSTANT_LEN;
    data[offset++] = packet->_type;
    memcpy(data+offset, packet->_payload, HIRCP_MAX_PAYLOAD_LEN);
    offset += HIRCP_MAX_PAYLOAD_LEN;
}

//*****************************************************************************
//
//! Populates the fields of an HIRCP_Packet using the input data.
//!
//! \param handle to an HIRCP_Packet structure.
//! \param data buffer holding the data.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
void HIRCP_Populate(HIRCP_Packet *packet, unsigned char *data, int len)
{
    //
    // Check for HIRCP constant
    //
    unsigned int offset = 0;
    unsigned char hircpConstant[HIRCP_CONSTANT_LEN];
    memcpy(hircpConstant, data, HIRCP_CONSTANT_LEN);
    offset += HIRCP_CONSTANT_LEN;
    if (strncmp((char*)hircpConstant, (char*)HIRCP_CONSTANT, HIRCP_CONSTANT_LEN) == 0)
    {
        //
        // If the HIRCP constant is present, then populate type and payload
        //
        packet->_type = (HIRCP_Type)(data[offset++]);
        memcpy(packet->_payload, data+offset, HIRCP_MAX_PAYLOAD_LEN);
    }
    else
    {
        //
        // If the HIRCP constant is not present, set type to invalid
        //
        packet->_type = HIRCP_INVALID;
    }
}

//*****************************************************************************
//
//! Initiates HIRCP connection sequence.
//!
//! \param None.
//!
//! \return Returns \b true if the connection was successful and \b false
//! otherwise.
//
//*****************************************************************************
tBoolean HIRCP_InitiateConnectionSequence(void)
{
    long lRetVal = 0;
    unsigned char recv_data[HIRCP_MAX_PACKET_LEN];
    unsigned char send_data[HIRCP_MAX_PACKET_LEN];
    unsigned char recv_payload[HIRCP_MAX_PAYLOAD_LEN];
    unsigned char send_payload[HIRCP_MAX_PAYLOAD_LEN];
    HIRCP_Packet *sendPacket = HIRCP_CreatePacket();
    HIRCP_Packet *recvPacket = HIRCP_CreatePacket();

    //
    // Receive CRQ packet
    //
    lRetVal = BsdTcpServerReceive(recv_data, HIRCP_MAX_PACKET_LEN);
    if (lRetVal < 0)
    {
        return false;
    }
    HIRCP_Populate(recvPacket, recv_data, HIRCP_MAX_PACKET_LEN);
    if (!HIRCP_IsValid(recvPacket) || !(HIRCP_GetType(recvPacket) == HIRCP_CRQ))
    {
        return false;
    }

    //
    // Check for mode
    //
    HIRCP_GetPayload(recvPacket, recv_payload, HIRCP_MAX_PAYLOAD_LEN);
    if (recv_payload[0] != HIRCP_NORMAL && recv_payload[0] != HIRCP_CLOSED_LOOP)
    {
        UART_PRINT("Invalid mode.\n\r");
        return false;
    }
    g_hircp_mode = recv_payload[0];

    UART_PRINT("Received CRQ packet.\n\r");

    //
    // Send ACK packet
    //
    HIRCP_ClearPacket(sendPacket);
    HIRCP_SetType(sendPacket, HIRCP_ACK);
    HIRCP_GetData(sendPacket, send_data, HIRCP_MAX_PACKET_LEN);
    lRetVal = BsdTcpServerSend(send_data, HIRCP_MAX_PACKET_LEN);
    if (lRetVal < 0)
    {
        return false;
    }
    UART_PRINT("Sent ACK packet.\n\r");

    HIRCP_DestroyPacket(sendPacket);
    HIRCP_DestroyPacket(recvPacket);
    return true;
}

//*****************************************************************************
//
//! Initiates HIRCP termination sequence.
//!
//! \param None.
//!
//! \return Returns \b true if the termination was successful and \b false
//! otherwise.
//
//*****************************************************************************
tBoolean HIRCP_InitiateTerminationSequence(void)
{
    long lRetVal = 0;
    unsigned char send_data[HIRCP_MAX_PACKET_LEN];
    HIRCP_Packet *sendPacket = HIRCP_CreatePacket();

    //
    // Send ACK packet
    //
    HIRCP_ClearPacket(sendPacket);
    HIRCP_SetType(sendPacket, HIRCP_ACK);
    HIRCP_GetData(sendPacket, send_data, HIRCP_MAX_PACKET_LEN);
    lRetVal = BsdTcpServerSend(send_data, HIRCP_MAX_PACKET_LEN);
    if (lRetVal < 0)
    {
        return false;
    }
    UART_PRINT("Sent ACK packet.\n\r");

    HIRCP_DestroyPacket(sendPacket);
    return true;
}

