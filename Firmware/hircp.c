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
// March 4, 2016
//
//*****************************************************************************
#include "hircp.h"

// Standard includes
#include <stdlib.h>
#include <string.h>

const unsigned char HIRCP_CONSTANT[] = "HIRC";

struct HIRCP_Packet
{
    HIRCP_Type _type;
    unsigned char _payload[HIRCP_MAX_PAYLOAD_LEN];
};

HIRCP_Packet *HIRCP_CreatePacket()
{
    HIRCP_Packet *handle;
    handle = (HIRCP_Packet*)malloc(sizeof(HIRCP_Packet));
    return handle;
}

void HIRCP_DestroyPacket(HIRCP_Packet *packet)
{
    free(packet);
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
