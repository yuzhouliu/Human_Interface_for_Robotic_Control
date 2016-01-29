//*****************************************************************************
//
// LeapMotionManager.cpp
//
// Class responsible for interfacing with the Leap Motion Controller.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 28, 2015
//
// Modified:
// January 5, 2016
//
//*****************************************************************************
#include "LeapMotionManager.h"

#include <cassert>

//*****************************************************************************
//
//! Empty constructor for LeapMotionManager.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
LeapMotionManager::LeapMotionManager()
{

}

//*****************************************************************************
//
//! Empty destructor for LeapMotionManager.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
LeapMotionManager::~LeapMotionManager()
{

}

//*****************************************************************************
//
//! Processes the Leap frame. Called once every application frame.
//!
//! \param buf buffer to store processed data.
//! \param buflen the size of the buffer
//!
//! \return Returns \b true if the frame was processed successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool LeapMotionManager::processFrame(unsigned char *buf, unsigned int buflen)
{
    LeapDataStruct leapData;
    Leap::Frame frame = _controller.frame();
    Leap::HandList hands = frame.hands();
    Leap::Hand hand;

    //
    // Iterates through list of hands provided by Leap Motion Controller and
    // identifies first right hand it detects.
    //
    for (auto it = hands.begin(); it != hands.end(); it++)
    {
        if ((*it).isRight())
        {
            hand = (*it);
        }
    }

    //
    // Checks if right hand was detected by Leap Motion Controller
    //
    if (!hand.isValid() || !hand.isRight())
    {
        std::cout << "No hand detected." << std::endl;
        unsigned int bufIndex = 0;
        for (int i = 0; i < NUM_FINGERS; i++)
        {
            //
            // Stores angle sequentially in buf
            //
            buf[bufIndex++] = 0;
        }
        buf[bufIndex++] = 0;
        return false;
    }

    //
    // Populates LeapDataStruct structure with finger info
    //
    Leap::FingerList fingers = hand.fingers();
    for (auto it = fingers.begin(); it != fingers.end(); it++)
    {
        Leap::Finger finger = (*it);
        Leap::Vector direction[4];
        for (auto b=0; b<4; b++)
        {
            Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
            Leap::Bone bone = finger.bone(boneType);
            direction[b] = bone.direction();
        }
        leapData.totalAngle[finger.type()] = (unsigned char)
            (_radiansToDegrees(_calculateTotalAngle(direction, 4)));
    }

    //
    // Populates LeapDataStruct structure with wrist info
    //
    Leap::Vector palmNormal = hand.palmNormal();
    float rollInRadians = palmNormal.roll();
    float rollInDegrees = _radiansToDegrees(rollInRadians);
    float wristAngle = rollInDegrees + 90;
    if (wristAngle > 180)
    {
        wristAngle = 180;
    }
    else if (wristAngle < 0)
    {
        wristAngle = 0;
    }
    leapData.wristAngle = (unsigned char)(wristAngle);

    //
    // Serialize data
    //
    _serialize(leapData, buf, buflen);

    return true;
}

//*****************************************************************************
//
//! Calculates the total angle (in radians) between the vectors
//!
//! \param vectors the array of vectors.
//! \param size the length of the array.
//!
//! \return Returns the total angle (in radians) between the vectors.
//
//*****************************************************************************
float LeapMotionManager::_calculateTotalAngle(Leap::Vector *vectors,
    unsigned int size)
{
    float angle = 0;

    for (unsigned int i=0; i<size-1; i++)
    {
        float temp = vectors[i].angleTo(vectors[i+1]);
        angle += temp;
    }

    return angle;
}

//*****************************************************************************
//
//! Converts an angle in radians to an angle in degrees.
//!
//! \param angle the angle in radians to convert.
//!
//! \return Returns the converted angle in degrees.
//
//*****************************************************************************
float LeapMotionManager::_radiansToDegrees(float angle)
{
    const float _RADIANS_TO_DEGREES = (float)57.2958;
    return _RADIANS_TO_DEGREES * angle;
}

//*****************************************************************************
//
//! Serializes structure of data and stores into buffer.
//!
//! \param leapData the structure with data to serialize.
//! \param buf the buffer to store serialized data.
//! \param buflen the size of the buffer
//!
//! \return None.
//
//*****************************************************************************
void LeapMotionManager::_serialize(LeapDataStruct &leapData,
    unsigned char *buf, unsigned int buflen)
{
    //
    // Buffer needs to be at least this size
    //
    const int BITS_PER_BYTE = 8;
    int angleSize = sizeof(leapData.totalAngle[0]);
    int wristSize = sizeof(leapData.wristAngle);
    assert(buflen >= (angleSize*NUM_FINGERS + wristSize));

    //
    // Serialize data
    //
    unsigned int bufIndex = 0;
    for (int i=0; i<NUM_FINGERS; i++)
    {
        auto angle = leapData.totalAngle[i];

        //
        // Stores angle sequentially in buf
        //
        buf[bufIndex++] = angle;
        std::cout << static_cast<unsigned int>(buf[bufIndex - 1]) << " ";
    }
    buf[bufIndex++] = leapData.wristAngle;
    std::cout << static_cast<unsigned int>(buf[bufIndex - 1]) << std::endl;
    std::cout << std::endl;
}
