//*****************************************************************************
//
// TCPSocket.h
//
// Class representing a TCP socket data type.
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
#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include "Network.h"
#include "SocketBase.h"

class TCPSocket : public SocketBase
{
public:
    /* Constructor */
    TCPSocket();

    /* Destructor */
    ~TCPSocket();

    /* Methods */
    bool open();
    void close();
    bool connect(AddressBase<unsigned int> *address);
    bool send(char *message);
    bool recv(char *message);
};

#endif /* _TCPSOCKET_H_ */
