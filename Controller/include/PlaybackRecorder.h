//*****************************************************************************
//
// PlaybackRecorder.h
//
// Class responsible for recording data stream and saving to disk.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// Feburary 19, 2016
//
// Modified:
// Feburary 19, 2016
//
//*****************************************************************************
#ifndef _PLAYBACKRECORDER_H_
#define _PLAYBACKRECORDER_H_

#include <fstream>

#include "LeapMotionManager.h" /* LeapData */

class PlaybackRecorder
{
private:
    /* Fields */
    std::ofstream _file;
    bool _recording;

public:
    /* Constructor */
    PlaybackRecorder();

    /* Destructor */
    ~PlaybackRecorder();

    /* Methods */
    bool isRecording();
    bool startRecording(char *filePath, int fps);
    bool stopRecording();
    void update(LeapData &leapData);
};

#endif /* _PLAYBACKRECORDER_H_ */
