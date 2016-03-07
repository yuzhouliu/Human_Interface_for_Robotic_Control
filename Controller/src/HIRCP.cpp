//*****************************************************************************
//
// HIRCP.cpp
//
// Classes responsible for implementing HIRCP. See protocol_specifications.txt
// for details.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// January 15, 2016
//
// Modified:
// March 4, 2016
//
//*****************************************************************************
#include "HIRCP.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

const unsigned char HIRCPPacket::HIRCP_CONSTANT[] = "HIRC";
const std::string HIRCPPacket::HIRCP_CONSTANT_STRING = "HIRC";

//*****************************************************************************
//
//! Constructor for HIRCPPacket.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
HIRCPPacket::HIRCPPacket()
    : _type(INVALID_TYPE)
{
    for (int i=0; i<MAX_PAYLOAD_LEN; i++)
    {
        _payload[i] = 0;
    }
}

//*****************************************************************************
//
//! Destructor for HIRCPPacket.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
HIRCPPacket::~HIRCPPacket()
{

}

//*****************************************************************************
//
//! Checks if the packet is well-formed.
//!
//! \param None.
//!
//! \return Returns \b true if the packet is well-formed and \b false
//! otherwise.
//
//*****************************************************************************
bool HIRCPPacket::isValid()
{
    return true;
}

//*****************************************************************************
//
//! Sets the HIRCP packet type.
//!
//! \param type the HIRCP packet type to set.
//!
//! \return None.
//
//*****************************************************************************
void HIRCPPacket::setType(TYPE type)
{
    _type = type;
}

//*****************************************************************************
//
//! Gets the HIRCP packet type.
//!
//! \param None.
//!
//! \return Returns the HIRCP packet type.
//
//*****************************************************************************
HIRCPPacket::TYPE HIRCPPacket::getType()
{
    return _type;
}

//*****************************************************************************
//
//! Sets the HIRCP packet payload.
//!
//! \param payload buffer holding the payload.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
void HIRCPPacket::setPayload(unsigned char *payload, int len)
{
    assert(len >= MAX_PAYLOAD_LEN);
    std::copy(payload, payload+MAX_PAYLOAD_LEN, _payload);
}

//*****************************************************************************
//
//! Gets the HIRCP packet payload.
//!
//! \param payload buffer to store the payload.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
void HIRCPPacket::getPayload(unsigned char *payload, int len)
{
    assert(len >= MAX_PAYLOAD_LEN);
    std::copy(_payload, _payload+MAX_PAYLOAD_LEN, payload);
}

//*****************************************************************************
//
//! Gets the HIRCP packet data.
//!
//! \param buf buffer to store the data.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
void HIRCPPacket::getData(unsigned char *buf, int len)
{
    assert(len >= MAX_PACKET_SIZE);

    int offset = 0;
    std::copy(HIRCP_CONSTANT, HIRCP_CONSTANT+HIRCP_CONSTANT_LEN, buf+offset);
    offset += HIRCP_CONSTANT_LEN;
    buf[offset++] = _type;
    std::copy(_payload, _payload+MAX_PAYLOAD_LEN, buf+offset);
    offset += MAX_PAYLOAD_LEN;
}

//*****************************************************************************
//
//! Populates the fields of the HIRCP packet using the input data.
//!
//! \param buf buffer holding the data.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
void HIRCPPacket::populate(unsigned char *buf, int len)
{
    assert((len <= MAX_PACKET_SIZE) && (len >= HIRCP_CONSTANT_LEN+OPCODE_LEN));

    //
    // Check for HIRCP constant
    //
    int offset = 0;
    unsigned char hircpConstant[HIRCP_CONSTANT_LEN+1];
    std::copy(buf, buf+HIRCP_CONSTANT_LEN, hircpConstant);
    offset += HIRCP_CONSTANT_LEN;
    hircpConstant[offset] = '\0';
    std::string hircpConstantString((char*)hircpConstant);
    if (hircpConstantString == HIRCP_CONSTANT_STRING)
    {
        //
        // If the HIRCP constant is present, then populate type and payload
        //
        _type = (TYPE)(buf[offset++]);
        std::copy(buf+offset, buf+MAX_PACKET_SIZE, _payload);
    }
    else
    {
        //
        // If the HIRCP constant is not present, set type to invalid
        //
        _type = INVALID_TYPE;
    }
}

//*****************************************************************************
//
//! Static method for creating a CRQ packet.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
HIRCPPacket HIRCPPacket::createCRQPacket()
{
    HIRCPPacket packet;

    packet.setType(TYPE::CRQ);

    return packet;
}

//*****************************************************************************
//
//! Static method for creating a DATA packet.
//!
//! \param payload buffer holding the payload.
//! \param len size of the buffer.
//!
//! \return None.
//
//*****************************************************************************
HIRCPPacket HIRCPPacket::createDATAPacket(unsigned char *payload, int len)
{
    HIRCPPacket packet;

    packet.setType(TYPE::DATA);
    packet.setPayload(payload, len);

    return packet;
}

//*****************************************************************************
//
//! Static method for creating a TRQ packet.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
HIRCPPacket HIRCPPacket::createTRQPacket()
{
    HIRCPPacket packet;

    packet.setType(TYPE::TRQ);

    return packet;
}

//*****************************************************************************
//
//! Static method for creating an empty packet.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
HIRCPPacket HIRCPPacket::createEmptyPacket()
{
    HIRCPPacket packet;

    packet.setType(TYPE::INVALID_TYPE);

    return packet;
}
