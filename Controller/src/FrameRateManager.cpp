//*****************************************************************************
//
// FrameRateManager.cpp
//
// Class that offers frame rate capping capabilities.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 21, 2015
//
// Modified:
// Feburary 19, 2016
//
//*****************************************************************************
#include "FrameRateManager.h"

#include <SDL.h>

//*****************************************************************************
//
//! Empty constructor for FrameRateManager.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
FrameRateManager::FrameRateManager()
    : _fps(_DEFAULT_FRAMES_PER_SECOND)
{

}

//*****************************************************************************
//
//! Empty destructor for FrameRateManager.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
FrameRateManager::~FrameRateManager()
{

}

//*****************************************************************************
//
//! Begins timer to start tracking frame rate.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void FrameRateManager::beginFrame()
{
    _fpsTimer.start();
}

//*****************************************************************************
//
//! Waits until frame rate elapses then ends timer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void FrameRateManager::endFrame()
{
    int time = _fpsTimer.getTimeOnTimer();
    if (time < 1000/_fps)
    {
        SDL_Delay(1000/_fps - time);
    }
    _fpsTimer.stop();
}

//*****************************************************************************
//
//! Gets the maximum FPS that the class will maintain.
//!
//! \param None.
//!
//! \return The maximum FPS that the class will maintain.
//
//*****************************************************************************
int FrameRateManager::getFPS()
{
    return _fps;
}

//*****************************************************************************
//
//! Sets the maximum FPS that the class will maintain.
//!
//! \param fps the FPS to set.
//!
//! \return None.
//
//*****************************************************************************
void FrameRateManager::setFPS(int fps)
{
    _fps = fps;
}
