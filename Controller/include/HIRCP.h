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
// January 15, 2016
//
//*****************************************************************************
#ifndef _HIRCP_H_
#define _HIRCP_H_

namespace HIRCPConstants
{
    enum PACKET_TYPE
    {
        CRQ,
        CACK,
        DATA,
        DACK,
        TRQ,
        TACK,
        ERR,
    };

    char *HIRCP_CONSTANT = "HIRCP";
    const int OPCODE_LEN = 2;

    const int MAX_PAYLOAD_LEN = 10;
};

template <class Type>
Type createHIRCPPacketFromBytes(char *data);

class HIRCPPacket
{
protected:
    /* Fields */
    HIRCPConstants::PACKET_TYPE _type;
    char _opcode[HIRCPConstants::OPCODE_LEN];
    char _data[HIRCPConstants::MAX_PAYLOAD_LEN];

public:
    /* Constructor */
    HIRCPPacket();

    /* Destructor */
    ~HIRCPPacket();

    /* Methods */
    virtual bool isValid() = 0;
};

class HIRCPDataPacket : public HIRCPPacket
{
private:
    /* Fields */
    const int PAYLOAD_LEN = 6;

public:
    /* Constructor */
    HIRCPDataPacket(char *data);

    /* Destructor */
    ~HIRCPDataPacket();

    /* Methods */
    bool isValid();
};

#endif /* _HIRCP_H_ */
