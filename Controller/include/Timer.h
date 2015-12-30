//*****************************************************************************
//
// Timer.h
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
// December 28, 2015
//
//*****************************************************************************
#ifndef _TIMER_H_
#define _TIMER_H_

class Timer
{
private:
    /* Fields */
    unsigned int _startTime;
    unsigned int _tempTime;
    unsigned int _pausedTime;
    bool _started;
    bool _paused;

public:
    /* Constructor */
    Timer();

    /* Destructor */
    ~Timer();

    /* Methods */
    void start();
    void stop();
    void pause();
    void resume();
    unsigned int getTimeOnTimer();
    unsigned int getTimeSinceTimerStarted();
    bool isStarted();
    bool isPaused();
};

#endif /* _TIMER_H_ */
