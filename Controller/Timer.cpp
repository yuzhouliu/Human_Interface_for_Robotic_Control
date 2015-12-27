//*****************************************************************************
//
// Timer.cpp
//
// Class that offers timing capabilities.
//
// Created:
// December 21, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#include "Timer.h"

#include <SDL.h>

Timer::Timer()
    : startTime(0), tempTime(0), pausedTime(0), started(false), paused(false)
{

}

Timer::~Timer()
{

}

//*****************************************************************************
//
//! Starts timer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Timer::start()
{
    if (!started)
    {
        startTime = SDL_GetTicks();
        tempTime = startTime;
        pausedTime = 0;
        started = false;
        paused = false;
    }
}

//*****************************************************************************
//
//! Stops timer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Timer::stop()
{
    if (started)
    {
        startTime = 0;
        tempTime = 0;
        pausedTime = 0;
        started = false;
        paused = false;
    }
}

//*****************************************************************************
//
//! Pauses timer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Timer::pause()
{
    if (started)
    {
        if (!paused)
        {
            pausedTime = getTimeOnTimer();
            paused = true;
        }
    }
}

//*****************************************************************************
//
//! Resumes timer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Timer::resume()
{
    if (started)
    {
        if (paused)
        {
            tempTime = SDL_GetTicks() - pausedTime;
            pausedTime = 0;
            paused = false;
        }
    }
}

//*****************************************************************************
//
//! Gets the current time on the timer.
//!
//! \param None.
//!
//! \return Returns the current time on the timer.
//
//*****************************************************************************
unsigned int Timer::getTimeOnTimer()
{
    if (started)
    {
        if (paused)
        {
            return pausedTime;
        }
        else
        {
            return SDL_GetTicks() - tempTime;
        }
    }
    else
    {
        return 0;
    }
}

//*****************************************************************************
//
//! Gets the time since the timer started.
//!
//! \param None.
//!
//! \return Returns the time since the timer started.
//
//*****************************************************************************
unsigned int Timer::getTimeSinceTimerStarted()
{
    if (started)
    {
        return SDL_GetTicks() - startTime;
    }
    else
    {
        return 0;
    }
}

//*****************************************************************************
//
//! Checks if the timer has been started.
//!
//! \param None.
//!
//! \return Returns \b true if the timer has been started and \b false if the
//! timer has not been started.
//
//*****************************************************************************
bool Timer::isStarted()
{
    return started;
}

//*****************************************************************************
//
//! Checks if the timer has been paused.
//!
//! \param None.
//!
//! \return Returns \b true if the timer has been paused and \b false if the
//! timer has not been paused.
//
//*****************************************************************************
bool Timer::isPaused()
{
    return paused;
}
