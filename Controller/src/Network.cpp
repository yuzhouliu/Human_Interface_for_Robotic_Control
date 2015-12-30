//*****************************************************************************
//
// Network.cpp
//
// Common network functions.
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
#include "Network.h"

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
