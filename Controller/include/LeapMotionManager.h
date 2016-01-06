//*****************************************************************************
//
// LeapMotionManager.h
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
// January 4, 2016
//
//*****************************************************************************
#ifndef _LEAPMOTIONMANAGER_H_
#define _LEAPMOTIONMANAGER_H_

#include "Leap.h"

#include "Finger.h" /* NUM_FINGERS */

struct LeapDataStruct
{
    unsigned char totalAngle[NUM_FINGERS];
    unsigned char wristAngle;
};

class LeapMotionManager
{
private:
    /* Fields */
    Leap::Controller _controller;

    /* Methods */
    float _calculateTotalAngle(Leap::Vector *vectors, unsigned int size);
    float _radiansToDegrees(float angle);
    void _serialize(LeapDataStruct &leapData, unsigned char *buf,
        unsigned int buflen);

public:
    /* Constructor */
    LeapMotionManager();

    /* Destructor */
    ~LeapMotionManager();

    /* Methods */
    bool processFrame(unsigned char *buf, unsigned int buflen);
};

#endif /* _LEAPMOTIONMANAGER_H_ */
