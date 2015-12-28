//*****************************************************************************
//
// SocketBase.h
//
// Abstract base class for berkeley sockets.
//
// Created:
// December 27, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#ifndef _SOCKETBASE_H_
#define _SOCKETBASE_H_

#include "AddressBase.h"

class SocketBase
{
private:
    /* Fields */
    int _sockfd;
    AddressBase<unsigned int> *_address;

public:
    /* Destructor */
    virtual ~SocketBase(){};

    /* Methods */
    virtual bool open();
    virtual void close();
    virtual bool connect(AddressBase<unsigned int> *address);
    virtual bool send(char *message);
    virtual bool recv(char *message);
};

#endif /* _SOCKETBASE_H_ */
