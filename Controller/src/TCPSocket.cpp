//*****************************************************************************
//
// TCPSocket.cpp
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
// December 30, 2015
//
//*****************************************************************************
#include "TCPSocket.h"

#include <iostream>
#include <winsock2.h>

//*****************************************************************************
//
//! Constructor for TCPSocket. Initializes variables.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
TCPSocket::TCPSocket()
{
    _sockfd = 0;
}

//*****************************************************************************
//
//! Destructor for TCPSocket. Releases resources used by socket.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
TCPSocket::~TCPSocket()
{
    close();
}

//*****************************************************************************
//
//! Creates and opens a TCP streaming socket.
//!
//! \param None.
//!
//! \return Returns \b true if the socket was opened successfully and \b
//! false otherwise.
//
//*****************************************************************************
bool TCPSocket::open()
{
    //
    // Checks if the socket is already open
    //
    if (_sockfd != 0)
    {
        std::cerr << "[ERROR] TCPSocket::open(): Socket already open." <<
            std::endl;
        return false;
    }

    //
    // Creates a socket
    //
    if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "[ERROR] TCPSocket::open(): Socket could not be opened."
            << std::endl;
        return false;
    }

    return true;
}

//*****************************************************************************
//
//! Closes the TCP streaming socket. Automatically called by destructor.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void TCPSocket::close()
{
#ifdef _WIN32
    //
    // Windows uses closesocket() instead of close()
    //
    closesocket(_sockfd);
#elif __unix__
    //
    // Call close() from the global namespace
    //
    ::close(_sockfd);
#endif
}

//*****************************************************************************
//
//! Establishes a TCP connection to remote host.
//!
//! \param address in which to attempt TCP connection.
//!
//! \return Returns \b true if the connection was established and \b
//! false otherwise.
//
//*****************************************************************************
bool TCPSocket::connect(AddressBase<unsigned int> *address)
{
    //
    // Constructs sockaddr_in structure to use for connecting
    //
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len = sizeof(serv_addr);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(address->getPort());
    serv_addr.sin_addr.s_addr = htonl((int)address->getAddress());

    //
    // Connects to remote host designated by address
    //
    if (::connect(_sockfd, (struct sockaddr*)&serv_addr, serv_addr_len))
    {
        std::cerr << "[ERROR] TCPSocket::connect(): Failed to connect to "\
            "remote host." << std::endl;
        close();
        return false;
    }

    return true;
}

//*****************************************************************************
//
//! Sends the entire message to remote host.
//!
//! \param message buffer containing message to send.
//!
//! \return Returns \b true if the message was sent successfully and \b
//! false otherwise.
//
//*****************************************************************************
bool TCPSocket::send(unsigned char *message, unsigned short len)
{
    // TODO (Brandon): Implement
    if (::send(_sockfd, reinterpret_cast<char*>(message), len, 0) == -1)
    {
        close();
        std::cerr << "[ERROR] TCPSocket::send(): send failed." << std::endl;
        return false;
    }

    return true;
}

//*****************************************************************************
//
//! Receives the entire message from remote host.
//!
//! \param message buffer to store message to be received.
//!
//! \return Returns \b true if the message was received successfully and \b
//! false otherwise.
//
//*****************************************************************************
bool TCPSocket::recv(unsigned char *message, unsigned short len)
{
    // TODO (Brandon): Implement
    return false;
}
