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
// January 15, 2016
//
//*****************************************************************************
#include "HIRCP.h"

#include <cstring>

HIRCPPacket::HIRCPPacket()
{

}

HIRCPDataPacket::HIRCPDataPacket(char *data)
{
    memcpy(_data, data, PAYLOAD_LEN);
}

bool HIRCPDataPacket::isValid()
{
    
}
