//*****************************************************************************
//
// Finger.h
//
// Class responsible for modelling a finger on a hand.
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
#ifndef _FINGER_H_
#define _FINGER_H_

#include <memory>
#include <SDL.h>

#include "Image.h"

#define NUM_FINGERS 5

enum class FingerType : unsigned char
{
    THUMB   = 0,
    INDEX   = 1,
    MIDDLE  = 2,
    RING    = 3,
    PINKY   = 4,
};

class Finger
{
private:
    /* Fields */
    FingerType _type;
    std::unique_ptr<Image> image;

public:
    /* Constructor */
    Finger(FingerType type, SDL_Renderer *renderer);

    /* Destructor */
    ~Finger();

    /* Methods */
    const std::unique_ptr<Image> &getImage();
};

//*****************************************************************************
//
//! Operator overload for unary operation. Converts the type to a value.
//!
//! \param type FingerType class enumeration.
//!
//! \return Returns the value that the type represents.
//
//*****************************************************************************
inline unsigned char operator+(FingerType type)
{
    return static_cast<unsigned char>(type);
}

#endif /* _FINGER_H_ */
