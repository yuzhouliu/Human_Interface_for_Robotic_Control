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
// March 3, 2016
//
//*****************************************************************************
#include "HIRCP.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

const unsigned char HIRCPPacket::HIRCP_CONSTANT[] = "HIRC";
const std::string HIRCPPacket::HIRCP_CONSTANT_STRING = "HIRC";

HIRCPPacket::HIRCPPacket()
    : _type(INVALID_TYPE)
{

}

HIRCPPacket::~HIRCPPacket()
{

}

bool HIRCPPacket::isValid()
{
    return false;
}

void HIRCPPacket::setType(TYPE type)
{
    _type = type;
}

HIRCPPacket::TYPE HIRCPPacket::getType()
{
    return _type;
}

void HIRCPPacket::setPayload(unsigned char *payload, int len)
{
    assert(len >= MAX_PAYLOAD_LEN);
    std::copy(payload, payload+MAX_PAYLOAD_LEN, _payload);
}

void HIRCPPacket::getPayload(unsigned char *payload, int len)
{
    assert(len >= MAX_PAYLOAD_LEN);
    std::copy(_payload, _payload+MAX_PAYLOAD_LEN, payload);
}

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

void HIRCPPacket::populate(unsigned char *buf, int len)
{
    assert((len <= MAX_PACKET_SIZE) && (len >= HIRCP_CONSTANT_LEN+OPCODE_LEN));

    int offset = 0;
    unsigned char hircpConstant[HIRCP_CONSTANT_LEN+1];
    std::copy(buf, buf+HIRCP_CONSTANT_LEN, hircpConstant);
    offset += HIRCP_CONSTANT_LEN;
    hircpConstant[offset] = '\0';
    std::string hircpConstantString((char*)hircpConstant);
    if (hircpConstantString == HIRCP_CONSTANT_STRING)
    {
        TYPE type = (TYPE)(buf[offset++]);
        std::copy(buf+offset, buf+MAX_PACKET_SIZE, _payload);
    }
    else
    {
        TYPE type = INVALID_TYPE;
        return;
    }
}

HIRCPPacket HIRCPPacket::createDATAPacket(unsigned char *payload, int len)
{
    HIRCPPacket packet;

    packet.setType(TYPE::DATA);
    packet.setPayload(payload, len);

    return packet;
}

HIRCPPacket HIRCPPacket::createEmptyPacket()
{
    HIRCPPacket packet;

    packet.setType(TYPE::INVALID_TYPE);

    return packet;
}
