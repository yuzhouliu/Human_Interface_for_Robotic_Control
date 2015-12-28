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
// December 28, 2015
//
//*****************************************************************************
#ifndef _LEAPMOTIONMANAGER_H_
#define _LEAPMOTIONMANAGER_H_

#include "Leap.h"

typedef struct
{
    
} LeapDataStruct;

class LeapMotionManager
{
private:
    /* Fields */
    Leap::Controller _controller;

    /* Methods */
    void _serialize(LeapDataStruct &leapData, char *buf, unsigned int buflen);

public:
    /* Constructor */
    LeapMotionManager();

    /* Destructor */
    ~LeapMotionManager();

    /* Methods */
    void processFrame(char *buf, unsigned int buflen);
};

#endif /* _LEAPMOTIONMANAGER_H_ */
