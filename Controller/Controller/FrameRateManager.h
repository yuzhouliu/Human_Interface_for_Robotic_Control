//*****************************************************************************
//
// FrameRateManager.h
//
// Class that offers frame rate capping capabilities.
//
// Created:
// December 21, 2015
//
// Modified:
// December 21, 2015
//
//*****************************************************************************
#ifndef _FRAMERATEMANAGER_H_
#define _FRAMERATEMANAGER_H_

//extern class Timer;
#include "Timer.h"

class FrameRateManager
{
private:
    /* Fields */
    const int DEFAULT_FRAMES_PER_SECOND = 60;
    Timer fpsTimer;
    int fps;

public:
    /* Constructor */
    FrameRateManager();

    /* Destructor */
    ~FrameRateManager();

    /* Methods */
    void beginFrame();
    void endFrame();
    void setFPS(int fps);
};

#endif /* _FRAMERATEMANAGER_H_ */
