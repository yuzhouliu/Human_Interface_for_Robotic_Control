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
// December 29, 2015
//
//*****************************************************************************
#ifndef _LEAPMOTIONMANAGER_H_
#define _LEAPMOTIONMANAGER_H_

#include "Leap.h"

#define NUM_FINGERS 5

struct LeapDataStruct
{
    unsigned char totalAngle[NUM_FINGERS];
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
