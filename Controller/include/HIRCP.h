//*****************************************************************************
//
// HIRCP.h
//
// Classes responsible for implementing HIRCP. See protocol_specifications.txt
// for details.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// January 15, 2016
//
// Modified:
// Feburary 26, 2016
//
//*****************************************************************************
#ifndef _HIRCP_H_
#define _HIRCP_H_

class HIRCPPacket
{
private:
    /* Fields */
    char *_HIRCP_CONSTANT = "HIRCP";
    static const int _HIRCP_CONSTANT_LEN = 5;
    static const int _OPCODE_LEN = 2;
    static const int _MAX_PAYLOAD_LEN = 10;

    enum TYPE
    {
        CRQ,
        CACK,
        DATA,
        DACK,
        TRQ,
        TACK,
        ERR,
    } _type;
    unsigned char _data[_HIRCP_CONSTANT_LEN + _OPCODE_LEN + _MAX_PAYLOAD_LEN];

    /* Constructor */
    HIRCPPacket();

public:
    /* Fields */
    static const int MAX_PACKET_SIZE = _HIRCP_CONSTANT_LEN + _OPCODE_LEN +
        _MAX_PAYLOAD_LEN;

    /* Destructor */
    ~HIRCPPacket();

    /* Methods */
    bool isValid();
    unsigned char *getData();

    /* Static methods */
    static HIRCPPacket createPacketFromBytes(unsigned char *data);
    static HIRCPPacket createDATAPacket(unsigned char *payload);
};

#endif /* _HIRCP_H_ */
