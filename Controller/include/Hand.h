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
// December 30, 2015
//
//*****************************************************************************
#ifndef _HAND_H_
#define _HAND_H_

#include <memory>
#include <vector>
#include <SDL.h>

#include "Finger.h"

struct FingerPressureStruct
{
    unsigned char pressure[NUM_FINGERS];
};

class Hand
{
private:
    /* Fields */
    std::unique_ptr<Image> _baseImage;
    std::vector<std::unique_ptr<Finger>> _fingerList;

public:
    /* Constructor */
    Hand(SDL_Renderer *renderer, unsigned short screenWidth,
        unsigned short screenHeight);

    /* Destructor */
    ~Hand();

    /* Methods */
    void update(FingerPressureStruct *fingerPressures);
    void render();
};

#endif /* _HAND_H_ */
