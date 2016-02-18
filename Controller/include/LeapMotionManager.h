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
// Feburary 18, 2016
//
//*****************************************************************************
#ifndef _LEAPMOTIONMANAGER_H_
#define _LEAPMOTIONMANAGER_H_

#include "Leap.h"

#include <SDL.h>

#include "Finger.h" /* NUM_FINGERS */

struct LeapDataStruct
{
    static const unsigned short _MAX_PAYLOAD = 256;
    static const unsigned short _IMAGE_SIZE = 153600;
    static const unsigned short _IMAGE_DATA_SIZE = 614400;
    unsigned char data[LeapDataStruct::_MAX_PAYLOAD];
    bool imageAvailable;
    unsigned char imageDataRGBA[LeapDataStruct::_IMAGE_DATA_SIZE];
    SDL_Rect fingerRects[NUM_FINGERS];
};

struct _LeapAngleStruct
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
    void _serialize(_LeapAngleStruct &leapData, unsigned char *buf,
        unsigned int buflen);

public:
    /* Constructor */
    LeapMotionManager();

    /* Destructor */
    ~LeapMotionManager();

    /* Methods */
    bool processFrame(LeapDataStruct leapData);
};

#endif /* _LEAPMOTIONMANAGER_H_ */
