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
// February 26, 2016
//
//*****************************************************************************
#include "HIRCP.h"

#include <cstring>

HIRCPPacket::HIRCPPacket()
{

}

HIRCPPacket::~HIRCPPacket()
{

}

bool HIRCPPacket::isValid()
{
    return false;
}

unsigned char *HIRCPPacket::getData()
{
    return _data;
}

HIRCPPacket HIRCPPacket::createPacketFromBytes(unsigned char *data)
{
    HIRCPPacket packet;

    // TODO: Implement

    return packet;
}

HIRCPPacket HIRCPPacket::createDATAPacket(unsigned char *payload)
{
    HIRCPPacket packet;

    // TODO: Implement

    return packet;
}
