//*****************************************************************************
//
// IPv4Address.cpp
//
// Class representing an Internet Protocol version 4 data type.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 27, 2015
//
// Modified:
// December 30, 2015
//
//*****************************************************************************
#include "IPv4Address.h"

#include "Network.h"

//*****************************************************************************
//
//! Empty constructor for IPv4Address.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
IPv4Address::IPv4Address() : IPv4Address((unsigned int)0, 0)
{

}

//*****************************************************************************
//
//! Constructor for IPv4Address. Sets the destination address.
//!
//! \param address of the recipient.
//!
//! \return None.
//
//*****************************************************************************
IPv4Address::IPv4Address(unsigned int address, unsigned short port)
{
    setAddress(address);
    setPort(port);
}

//*****************************************************************************
//
//! Constructor for IPv4Address. Converts ascii address to unsigned in and sets
//! the destination address.
//!
//! \param address of the recipient.
//!
//! \return None.
//
//*****************************************************************************
IPv4Address::IPv4Address(char *address, unsigned short port)
{
    setAddress(ntohl(inet_addr(address)));
    setPort(port);
}

//*****************************************************************************
//
//! Empty destructor for IPv4Address.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
IPv4Address::~IPv4Address()
{

}

//*****************************************************************************
//
//! Gets the destination address.
//!
//! \param None.
//!
//! \return Returns the address of the recipient.
//
//*****************************************************************************
unsigned int IPv4Address::getAddress()
{
    return _address;
}

//*****************************************************************************
//
//! Sets the destination address.
//!
//! \param address of the recipient.
//!
//! \return None.
//
//*****************************************************************************
void IPv4Address::setAddress(unsigned int address)
{
    _address = address;
}

//*****************************************************************************
//
//! Gets the destination port.
//!
//! \param None.
//!
//! \return Returns the port of the recipient.
//
//*****************************************************************************
unsigned short IPv4Address::getPort()
{
    return _port;
}

//*****************************************************************************
//
//! Sets the destination port.
//!
//! \param port of the recipient.
//!
//! \return None.
//
//*****************************************************************************
void IPv4Address::setPort(unsigned short port)
{
    _port = port;
}
