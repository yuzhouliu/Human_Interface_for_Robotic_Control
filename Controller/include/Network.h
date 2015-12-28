//*****************************************************************************
//
// Network.h
//
// Common network functions.
//
// Created:
// December 27, 2015
//
// Modified:
// December 28, 2015
//
//*****************************************************************************
#ifndef _NETWORK_H_
#define _NETWORK_H_

typedef int socklen_t;
typedef int ssize_t;

namespace network
{
    bool initialize();
    void terminate();
}

#endif /* _NETWORK_H_ */
