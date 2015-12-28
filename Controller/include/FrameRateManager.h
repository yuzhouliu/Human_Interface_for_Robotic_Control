//*****************************************************************************
//
// FrameRateManager.h
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
// December 27, 2015
//
//*****************************************************************************
#ifndef _FRAMERATEMANAGER_H_
#define _FRAMERATEMANAGER_H_

#include "Timer.h"

class FrameRateManager
{
private:
    /* Fields */
    const int _DEFAULT_FRAMES_PER_SECOND = 10;
    Timer _fpsTimer;
    int _fps;

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
