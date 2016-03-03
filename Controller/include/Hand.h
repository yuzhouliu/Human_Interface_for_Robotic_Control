//*****************************************************************************
//
// Hand.h
//
// Class responsible for modelling a hand.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 29, 2015
//
// Modified:
// March 3, 2016
//
//*****************************************************************************
#ifndef _HAND_H_
#define _HAND_H_

#include <memory>
#include <vector>
#include <SDL.h>

#include "Finger.h"
#include "LeapMotionManager.h" /* LeapDataStruct */

struct FingerPressureStruct
{
    unsigned char pressure[NUM_FINGERS];
};

class Hand
{
private:
    /* Fields */
    SDL_Renderer *_renderer;
    std::unique_ptr<Image> _image;
    std::unique_ptr<Image> _staticImage;
    std::unique_ptr<Image> _trackedPalm;
    std::vector<std::unique_ptr<Finger>> _fingerList;
    std::unique_ptr<Image> _thumbText;
    std::unique_ptr<Image> _indexText;
    std::unique_ptr<Image> _middleText;
    std::unique_ptr<Image> _ringText;
    std::unique_ptr<Image> _pinkyText;
    std::unique_ptr<Image> _wristText;
    std::vector<Image*> _numberTexts;
    int _recentAngles[NUM_FINGERS + 1];

public:
    /* Constructor */
    Hand(SDL_Renderer *renderer);

    /* Destructor */
    ~Hand();

    /* Methods */
    void update(LeapData &leapData, FingerPressureStruct &fingerPressures);
    void render();
};

#endif /* _HAND_H_ */
