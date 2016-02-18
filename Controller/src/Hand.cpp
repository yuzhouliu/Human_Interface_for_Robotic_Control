//*****************************************************************************
//
// Hand.cpp
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
// Feburary 18, 2016
//
//*****************************************************************************
#include "Hand.h"

//*****************************************************************************
//
//! Constructor for Hand. Acquires resources for finger models.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Hand::Hand(SDL_Renderer *renderer, unsigned short screenWidth,
    unsigned short screenHeight)
    : _renderer(renderer)
{
    //
    // Creates base hand image
    //
    _staticImage = std::unique_ptr<Image>(new Image(renderer,
        "data/gfx/hand_base.png"));

    //
    // Creates and add fingers to list
    //
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::THUMB, renderer)));
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::INDEX, renderer)));
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::MIDDLE, renderer)));
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::RING, renderer)));
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::PINKY, renderer)));

    //
    // Centre all images for aesthetic purposes
    //
    Image::centreImage(_staticImage, screenWidth, screenHeight);
    for (auto it = _fingerList.begin(); it != _fingerList.end(); it++)
    {
        Image::centreImage((*it)->getStaticImage(), screenWidth, screenHeight);
    }
}

//*****************************************************************************
//
//! Destructor for Hand.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Hand::~Hand()
{

}

//*****************************************************************************
//
//! Updates hand with finger pressure information.
//!
//! \param leapData a structure containing leap data information.
//! \param fingerPressures a structure containing finger pressure information.
//!
//! \return None.
//
//*****************************************************************************
void Hand::update(LeapDataStruct &leapData,
    FingerPressureStruct &fingerPressures)
{
    //
    // Updates hand image
    //
    _image;
    _image = std::unique_ptr<Image>(new Image(_renderer,
        "data/gfx/hand_base.png"));

    //
    // Updates finger pressures
    //
    for (int i=0; i<NUM_FINGERS; i++)
    {
        FingerType type = static_cast<FingerType>(i);
        auto pressureToSet = fingerPressures.pressure[i];
        _fingerList[+type]->setPressure(pressureToSet);
    }
}

//*****************************************************************************
//
//! Renders hand.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Hand::render()
{
    //
    // Renders all images to screen
    //
    //_staticImage->onRender();
    if (_image != nullptr)
    {
        _image->onRender();
    }

    for (auto it = _fingerList.begin(); it != _fingerList.end(); it++)
    {
        (*it)->render();
    }
}