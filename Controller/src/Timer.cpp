//*****************************************************************************
//
// Timer.cpp
//
// Class that offers timing capabilities.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 21, 2015
//
// Modified:
// February 24, 2016
//
//*****************************************************************************
#include "Timer.h"

#include <SDL.h>

//*****************************************************************************
//
//! Empty constructor for Timer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Timer::Timer()
    : _startTime(0), _tempTime(0), _pausedTime(0), _started(false),
      _paused(false)
{

}

//*****************************************************************************
//
//! Empty destructor for Timer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
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
    if (!_started)
    {
        _startTime = SDL_GetTicks();
        _tempTime = _startTime;
        _pausedTime = 0;
        _started = true;
        _paused = false;
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
    if (_started)
    {
        _startTime = 0;
        _tempTime = 0;
        _pausedTime = 0;
        _started = false;
        _paused = false;
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
    if (_started)
    {
        if (!_paused)
        {
            _pausedTime = getTimeOnTimer();
            _paused = true;
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
    if (_started)
    {
        if (_paused)
        {
            _tempTime = SDL_GetTicks() - _pausedTime;
            _pausedTime = 0;
            _paused = false;
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
    if (_started)
    {
        if (_paused)
        {
            return _pausedTime;
        }
        else
        {
            return SDL_GetTicks() - _tempTime;
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
    if (_started)
    {
        return SDL_GetTicks() - _startTime;
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
    return _started;
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
    return _paused;
}
