//*****************************************************************************
//
// Finger.cpp
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
#include "Finger.h"

#include <string>

#include "Window.h"

//*****************************************************************************
//
//! Constructor for Finger. Acquires resources for image.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Finger::Finger(FingerType type, SDL_Renderer *renderer)
    : _type(type), _renderer(renderer)
{
    std::string fingerPaths[] = {
        "data/gfx/hand_right_thumb.png",
        "data/gfx/hand_right_index.png",
        "data/gfx/hand_right_middle.png",
        "data/gfx/hand_right_ring.png",
        "data/gfx/hand_right_pinky.png"
    };

    //
    // Static image for viewport
    //
    _staticImage = std::unique_ptr<Image>(new Image(renderer,
        fingerPaths[+type]));
    _staticImage->enableAlphaBlend();
    _staticImage->setAlphaBlend(0);

    //
    // Dynamic image for following finger positions
    //
    _image = std::unique_ptr<Image>(new Image(renderer,
        "data/gfx/pressure.png"));
    _image->enableAlphaBlend();
    _image->setAlphaBlend(0);

    //
    // Hide image at first
    //
    SDL_Rect rect = { 0, 0, 0, 0 };
    setRenderRect(rect);
}

//*****************************************************************************
//
//! Destructor for Finger.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Finger::~Finger()
{

}

//*****************************************************************************
//
//! Gets the image.
//!
//! \param None.
//!
//! \return Returns a reference to the image.
//
//*****************************************************************************
const std::unique_ptr<Image> &Finger::getImage()
{
    return _image;
}

//*****************************************************************************
//
//! Gets the static image.
//!
//! \param None.
//!
//! \return Returns a reference to the image.
//
//*****************************************************************************
const std::unique_ptr<Image> &Finger::getStaticImage()
{
    return _staticImage;
}

//*****************************************************************************
//
//! Sets the finger pressure.
//!
//! \param pressure the new pressure to set.
//!
//! \return None.
//
//*****************************************************************************
void Finger::setPressure(unsigned char pressure)
{
    _pressure = pressure;
}

//*****************************************************************************
//
//! Sets the rectangle that dictates where the texture will be rendered.
//!
//! \param renderRect to set.
//!
//! \return None.
//
//*****************************************************************************
void Finger::setRenderRect(SDL_Rect &renderRect)
{
    _image->setRenderRect(renderRect);
}

//*****************************************************************************
//
//! Renders finger to screen.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Finger::render()
{
    SDL_RenderSetViewport(_renderer, &Window::gPrimaryViewport);
    _image->setAlphaBlend(_pressure);
    _image->onRender();

    SDL_RenderSetViewport(_renderer, &Window::gSecondaryViewport);
    _staticImage->setAlphaBlend(_pressure);
    _staticImage->onRender();
}
