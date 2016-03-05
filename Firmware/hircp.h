//*****************************************************************************
//
// hircp.h
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
#ifndef _HIRCP_C_
#define _HIRCP_C_

#include "hw_types.h"   /* tBoolean */

#define HIRCP_CRQ_PAYLOAD_LEN 0
#define HIRCP_CACK_PAYLOAD_LEN 0
#define HIRCP_DATA_PAYLOAD_LEN 6
#define HIRCP_DACK_PAYLOAD_LEN 10
#define HIRCP_TRQ_PAYLOAD_LEN 0
#define HIRCP_TACK_PAYLOAD_LEN 0
#define HIRCP_ERR_PAYLOAD_LEN 1

#define HIRCP_MAX_PACKET_LEN 15
#define HIRCP_MAX_PAYLOAD_LEN 10
#define HIRCP_CONSTANT_LEN 4
#define HIRCP_OPCODE_LEN 1

extern const unsigned char HIRCP_CONSTANT[];

typedef enum HIRCP_Type
{
    HIRCP_INVALID = 0,
    HIRCP_CRQ,
    HIRCP_CACK,
    HIRCP_DATA,
    HIRCP_DACK,
    HIRCP_TRQ,
    HIRCP_TACK,
    HIRCP_ERR,
} HIRCP_Type;

typedef struct HIRCP_Packet HIRCP_Packet;

HIRCP_Packet *HIRCP_CreatePacket(void);
void HIRCP_DestroyPacket(HIRCP_Packet *packet);
void HIRCP_ClearPacket(HIRCP_Packet *packet);
tBoolean HIRCP_IsValid(HIRCP_Packet *packet);
void HIRCP_SetType(HIRCP_Packet *packet, HIRCP_Type type);
HIRCP_Type HIRCP_GetType(HIRCP_Packet *packet);
void HIRCP_SetPayload(HIRCP_Packet *packet, unsigned char *payload, int len);
void HIRCP_GetPayload(HIRCP_Packet *packet, unsigned char *payload, int len);
void HIRCP_GetData(HIRCP_Packet *packet, unsigned char *data, int len);
void HIRCP_Populate(HIRCP_Packet *packet, unsigned char *data, int len);
tBoolean HIRCP_InitiateConnectionSequence(void);
tBoolean HIRCP_InitiateTerminationSequence(void);

#endif /* _HIRCP_C_ */
