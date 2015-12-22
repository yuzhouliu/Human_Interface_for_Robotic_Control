//*****************************************************************************
//
// Timer.h
//
// Class that offers timing capabilities.
//
// Created:
// December 21, 2015
//
// Modified:
// December 21, 2015
//
//*****************************************************************************
#ifndef _TIMER_H_
#define _TIMER_H_

//extern Uint32;
#include <SDL.h>

class Timer
{
private:
    /* Fields */
    Uint32 startTime;
    Uint32 tempTime;
    Uint32 pausedTime;
    bool started;
    bool paused;

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
    Uint32 getTimeOnTimer();
    Uint32 getTimeSinceTimerStarted();
    bool isStarted();
    bool isPaused();
};

#endif /* _TIMER_H_ */
