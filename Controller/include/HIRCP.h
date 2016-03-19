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
// March 18, 2016
//
//*****************************************************************************
#ifndef _HIRCP_H_
#define _HIRCP_H_

#define D_SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include <string>

class HIRCPPacket
{
public:
    /* Fields */
    static const unsigned char HIRCP_CONSTANT[];
    static const std::string HIRCP_CONSTANT_STRING;
    static const int HIRCP_CONSTANT_LEN = 4;
    static const int OPCODE_LEN = 1;
    static const int MAX_PAYLOAD_LEN = 10;
    static const int MAX_PACKET_SIZE = HIRCP_CONSTANT_LEN + OPCODE_LEN +
        MAX_PAYLOAD_LEN;

    enum TYPE
    {
        INVALID_TYPE = 0,
        CRQ,
        MODE,
        DATA,
        DACK,
        TRQ,
        ACK,
        ERR,
    };

    enum FEEDBACK_MODE
    {
        NORMAL_MODE = 0,
        CLOSED_LOOP_MODE,
    };

    /* Constructor */
    HIRCPPacket();

    /* Destructor */
    ~HIRCPPacket();

    /* Methods */
    bool isValid();
    void setType(TYPE type);
    TYPE getType();
    void setPayload(unsigned char *payload, int len);
    void getPayload(unsigned char *payload, int len);
    void getData(unsigned char *buf, int len);
    void populate(unsigned char *buf, int len);

    /* Static methods */
    static HIRCPPacket createCRQPacket(FEEDBACK_MODE mode);
    static HIRCPPacket createMODEPacket(FEEDBACK_MODE mode);
    static HIRCPPacket createDATAPacket(unsigned char *payload, int len);
    static HIRCPPacket createTRQPacket();
    static HIRCPPacket createEmptyPacket();

private:
    /* Fields */
    static const int CRQ_PAYLOAD_LEN = 1;
    static const int MODE_PAYLOAD_LEN = 1;
    static const int DATA_PAYLOAD_LEN = 6;
    static const int DACK_PAYLOAD_LEN = 10;
    static const int TRQ_PAYLOAD_LEN = 0;
    static const int ACK_PAYLOAD_LEN = 0;
    static const int ERR_PAYLOAD_LEN = 1;

    TYPE _type;
    unsigned char _payload[MAX_PAYLOAD_LEN];
};

#endif /* _HIRCP_H_ */
