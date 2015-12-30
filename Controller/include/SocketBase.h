//*****************************************************************************
//
// SocketBase.h
//
// Abstract base class for berkeley sockets.
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
#ifndef _SOCKETBASE_H_
#define _SOCKETBASE_H_

#include "AddressBase.h"

class SocketBase
{
protected:
    /* Fields */
    int _sockfd;
    AddressBase<unsigned int> *_address;

public:
    /* Destructor */
    virtual ~SocketBase(){};

    /* Methods */
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool connect(AddressBase<unsigned int> *address) = 0;
    virtual bool send(char *message) = 0;
    virtual bool recv(char *message) = 0;
};

#endif /* _SOCKETBASE_H_ */
