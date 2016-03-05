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
// March 5, 2016
//
//*****************************************************************************
#include "hircp.h"

// Standard includes
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "tcp_socket.h"

const unsigned char HIRCP_CONSTANT[] = "HIRC";

struct HIRCP_Packet
{
    HIRCP_Type _type;
    unsigned char _payload[HIRCP_MAX_PAYLOAD_LEN];
};

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

void HIRCP_DestroyPacket(HIRCP_Packet *packet)
{
    if (packet != NULL)
    {
        free(packet);
    }
}

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

tBoolean HIRCP_IsValid(HIRCP_Packet *packet)
{
    //
    // TODO (Brandon): Implement
    //
    return 1;
}

void HIRCP_SetType(HIRCP_Packet *packet, HIRCP_Type type)
{
    packet->_type = type;
}

HIRCP_Type HIRCP_GetType(HIRCP_Packet *packet)
{
    return packet->_type;
}

void HIRCP_SetPayload(HIRCP_Packet *packet, unsigned char *payload, int len)
{
    memcpy(packet->_payload, payload, len);
}

void HIRCP_GetPayload(HIRCP_Packet *packet, unsigned char *payload, int len)
{
    memcpy(payload, packet->_payload, HIRCP_MAX_PAYLOAD_LEN);
}

void HIRCP_GetData(HIRCP_Packet *packet, unsigned char *data, int len)
{
    unsigned int offset = 0;
    memcpy(data, HIRCP_CONSTANT, HIRCP_CONSTANT_LEN);
    offset += HIRCP_CONSTANT_LEN;
    data[offset++] = packet->_type;
    memcpy(data+offset, packet->_payload, HIRCP_MAX_PAYLOAD_LEN);
    offset += HIRCP_MAX_PAYLOAD_LEN;
}

void HIRCP_Populate(HIRCP_Packet *packet, unsigned char *data, int len)
{
    unsigned int offset = 0;
    unsigned char hircpConstant[HIRCP_CONSTANT_LEN];
    memcpy(hircpConstant, data, HIRCP_CONSTANT_LEN);
    offset += HIRCP_CONSTANT_LEN;
    if (strncmp((char*)hircpConstant, (char*)HIRCP_CONSTANT, HIRCP_CONSTANT_LEN) == 0)
    {
        packet->_type = (HIRCP_Type)(data[offset++]);
        memcpy(packet->_payload, data+offset, HIRCP_MAX_PAYLOAD_LEN);
    }
    else
    {
        packet->_type = HIRCP_INVALID;
    }
}

tBoolean HIRCP_InitiateConnectionSequence(void)
{
    long lRetVal = 0;
    char recv_data[HIRCP_MAX_PACKET_LEN];
    char send_data[HIRCP_MAX_PACKET_LEN];
    HIRCP_Packet *sendPacket = HIRCP_CreatePacket();
    HIRCP_Packet *recvPacket = HIRCP_CreatePacket();

    // Receive CRQ packet
    lRetVal = BsdTcpServerReceive(recv_data, HIRCP_MAX_PACKET_LEN);
    HIRCP_Populate(recvPacket, recv_data, HIRCP_MAX_PACKET_LEN);
    if (!HIRCP_IsValid(recvPacket) || !(HIRCP_GetType(recvPacket) == HIRCP_CRQ))
    {
        return false;
    }
    UART_PRINT("Received CRQ packet.\n\r");

    // Send CACK packet
    HIRCP_ClearPacket(sendPacket);
    HIRCP_SetType(sendPacket, HIRCP_CACK);
    HIRCP_GetData(sendPacket, send_data, HIRCP_MAX_PACKET_LEN);
    lRetVal = BsdTcpServerSend(send_data, HIRCP_MAX_PACKET_LEN);
    UART_PRINT("Sent CACK packet.\n\r");

    HIRCP_DestroyPacket(sendPacket);
    HIRCP_DestroyPacket(recvPacket);
    return true;
}

tBoolean HIRCP_InitiateTerminationSequence(void)
{
    long lRetVal = 0;
    char send_data[HIRCP_MAX_PACKET_LEN];
    HIRCP_Packet *sendPacket = HIRCP_CreatePacket();

    // Send TACK packet
    HIRCP_ClearPacket(sendPacket);
    HIRCP_SetType(sendPacket, HIRCP_TACK);
    HIRCP_GetData(sendPacket, send_data, HIRCP_MAX_PACKET_LEN);
    lRetVal = BsdTcpServerSend(send_data, HIRCP_MAX_PACKET_LEN);
    UART_PRINT("Sent TACK packet.\n\r");

    HIRCP_DestroyPacket(sendPacket);
    return true;
}
