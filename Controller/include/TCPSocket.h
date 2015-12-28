//*****************************************************************************
//
// TCPSocket.h
//
// Class representing a TCP socket data type.
//
// Created:
// December 27, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include "SocketBase.h"

class TCPSocket : public SocketBase
{
public:
    /* Constructor */
    TCPSocket();

    /* Methods */
    bool open();
    void close();
    bool connect(AddressBase<unsigned int> *address);
    bool send(char *message);
    bool recv(char *message);
};

#endif /* _TCPSOCKET_H_ */
