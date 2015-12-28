//*****************************************************************************
//
// IPv4Address.h
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
// December 28, 2015
//
//*****************************************************************************
#ifndef _IPV4ADDRESS_H_
#define _IPV4ADDRESS_H_

#include "AddressBase.h"

class IPv4Address : public AddressBase<unsigned int>
{
public:
    /* Constructor */
    IPv4Address();
    IPv4Address(unsigned int address, unsigned short port);

    /* Destructor */
    ~IPv4Address();

    /* Methods */
    unsigned int getAddress();
    void setAddress(unsigned int address);
    unsigned short getPort();
    void setPort(unsigned short port);
};

#endif /* _IPV4ADDRESS_H_ */
