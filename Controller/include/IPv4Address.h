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
// January 9, 2016
//
//*****************************************************************************
#ifndef _IPV4ADDRESS_H_
#define _IPV4ADDRESS_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "AddressBase.h"

class IPv4Address : public AddressBase<unsigned int>
{
public:
    /* Fields */
    static const int MAX_IP_ADDR_BUF_LEN = 16;
    static const int MIN_IP_ADDR_LEN = 7;
    static const int MAX_IP_ADDR_LEN = 15;

    /* Constructor */
    IPv4Address();
    IPv4Address(unsigned int address, unsigned short port);
    IPv4Address(char *address, unsigned short port);

    /* Destructor */
    ~IPv4Address();

    /* Methods */
    unsigned int getAddress();
    void setAddress(unsigned int address);
    unsigned short getPort();
    void setPort(unsigned short port);

    /* Static methods */
    static bool validateIPAddress(char *ipAddress);
};

#endif /* _IPV4ADDRESS_H_ */
