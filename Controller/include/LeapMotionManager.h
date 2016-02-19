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

struct LeapData
{
    static const unsigned short _MAX_PAYLOAD = 256;
    static const unsigned int _IMAGE_SIZE = 153600;
    static const unsigned int _IMAGE_DATA_SIZE = 614400;

    //
    // Finger data
    //
    unsigned char data[LeapData::_MAX_PAYLOAD];
    SDL_Rect fingerRects[NUM_FINGERS];

    //
    // Image data
    //
    bool imageAvailable = false;
    int imageWidth;
    int imageHeight;
    int imageDepth;
    int imagePitch;
    unsigned char *imageDataRGBA;
    SDL_Rect imageRenderRect;

    LeapData()
    {
        imageDataRGBA = new unsigned char[LeapData::_IMAGE_DATA_SIZE];
    }

    ~LeapData()
    {
        delete[] imageDataRGBA;
    }
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
    int _windowWidth;
    int _windowHeight;

    /* Methods */
    float _calculateTotalAngle(Leap::Vector *vectors, unsigned int size);
    float _radiansToDegrees(float angle);
    void _serialize(_LeapAngleStruct &leapData, unsigned char *buf,
        unsigned int buflen);

public:
    /* Constructor */
    LeapMotionManager(SDL_Window *window);

    /* Destructor */
    ~LeapMotionManager();

    /* Methods */
    bool processFrame(LeapData &leapData);
};

#endif /* _LEAPMOTIONMANAGER_H_ */
