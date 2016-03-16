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
// Feburary 29, 2016
//
//*****************************************************************************
#ifndef _FINGER_H_
#define _FINGER_H_

#include <memory>
#include <SDL.h>

#include "Image.h"

#define NUM_FINGERS 5

#define MAX_SENSOR_READING 1100
#define SENSOR_SCALAR 3.05

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
    SDL_Renderer *_renderer;
    std::unique_ptr<Image> _image;
    std::unique_ptr<Image> _staticImage;
    unsigned char _pressure;

public:
    /* Constructor */
    Finger(FingerType type, SDL_Renderer *renderer);

    /* Destructor */
    ~Finger();

    /* Methods */
    const std::unique_ptr<Image> &getImage();
    const std::unique_ptr<Image> &getStaticImage();
    void setPressure(unsigned char pressure);
    void setRenderRect(SDL_Rect &renderRect);
    void render();
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
