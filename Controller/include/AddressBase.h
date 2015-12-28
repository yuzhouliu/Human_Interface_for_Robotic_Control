//*****************************************************************************
//
// AddressBase.h
//
// Abstract base class for internet addresses.
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
#ifndef _ADDRESSBASE_H_
#define _ADDRESSBASE_H_

template <class T>
class AddressBase
{
protected:
    /* Fields */
    T _address;
    unsigned short _port;

public:
    /* Destructor */
    virtual ~AddressBase(){};

    /* Methods */
    virtual T getAddress() = 0;
    virtual void setAddress(T address) = 0;
    virtual unsigned short getPort() = 0;
    virtual void setPort(unsigned short port) = 0;
};

#endif /* _ADDRESSBASE_H_ */

