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
// December 27, 2015
//
//*****************************************************************************
#ifndef _TIMER_H_
#define _TIMER_H_

class Timer
{
private:
    /* Fields */
    unsigned int startTime;
    unsigned int tempTime;
    unsigned int pausedTime;
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
    unsigned int getTimeOnTimer();
    unsigned int getTimeSinceTimerStarted();
    bool isStarted();
    bool isPaused();
};

#endif /* _TIMER_H_ */
