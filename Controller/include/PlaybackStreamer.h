//*****************************************************************************
//
// PlaybackStreamer.h
//
// Class responsible for streaming recorded data from file.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// Feburary 19, 2016
//
// Modified:
// Feburary 24, 2016
//
//*****************************************************************************
#ifndef _PLAYBACKSTREAMER_H_
#define _PLAYBACKSTREAMER_H_

#include <fstream>

#include "IObservable.h"
#include "LeapMotionManager.h" /* LeapData */
#include "Timer.h"

class PlaybackStreamer : public IObservable
{
private:
    /* Fields */
    std::ifstream _file;
    bool _streaming;
    int _fps;
    Timer _timer;

public:
    /* Constructor */
    PlaybackStreamer();

    /* Destructor */
    ~PlaybackStreamer();

    /* Methods */
    bool isStreaming();
    bool startStreaming(char *filePath);
    bool stopStreaming();
    void update(LeapData &leapData);
    int getStreamingFPS();
};

#endif /* _PLAYBACKSTREAMER_H_ */
