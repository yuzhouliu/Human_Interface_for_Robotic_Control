//*****************************************************************************
//
// Network.cpp
//
// Common network functions.
//
// Created:
// December 27, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#include "Network.h"

#include <winsock2.h>

//*****************************************************************************
//
//! Initializes socket API.
//!
//! \param None.
//!
//! \return Returns \b true if the socket API was initialized successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool network::initialize()
{
#ifdef _WIN32
    //
    // Windows requires initialization
    //
    WSADATA wsaData;
    return (WSAStartup(MAKEWORD(2,2), &wsaData) == 0);
#else
    return true;
#endif
}

//*****************************************************************************
//
//! Terminates socket API.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void network::terminate()
{
#ifdef _WIN32
    //
    // Windows requires termination
    //
    WSACleanup();
#endif
}
