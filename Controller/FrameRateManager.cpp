//*****************************************************************************
//
// FrameRateManager.cpp
//
// Class that offers frame rate capping capabilities.
//
// Created:
// December 21, 2015
//
// Modified:
// December 27, 2015
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
    : fps(DEFAULT_FRAMES_PER_SECOND)
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
    fpsTimer.start();
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
    int time = fpsTimer.getTimeOnTimer();
    if (time < 1000/fps)
    {
        SDL_Delay(1000/fps - time);
    }
    fpsTimer.stop();
}

//*****************************************************************************
//
//! Sets the minimum FPS that the class will maintain.
//!
//! \param Sets the frames per second to \i fps.
//!
//! \return None.
//
//*****************************************************************************
void FrameRateManager::setFPS(int fps)
{
    this->fps = fps;
}
