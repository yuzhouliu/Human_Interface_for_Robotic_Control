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
// Feburary 28, 2016
//
//*****************************************************************************
#ifndef _PLAYBACKRECORDER_H_
#define _PLAYBACKRECORDER_H_

#include <fstream>
#include <memory>

#include "SDL.h"

#include "Image.h"
#include "LeapMotionManager.h" /* LeapData */
#include "Timer.h"

class PlaybackRecorder
{
private:
    /* Fields */
    std::ofstream _file;
    bool _recording;
    bool _delayElapsed;
    Timer _timer;
    std::unique_ptr<Image> _recordingImage;
    std::unique_ptr<Image> _recordingText;
    std::unique_ptr<Image> _delayText;

public:
    /* Constructor */
    PlaybackRecorder(SDL_Window *window);

    /* Destructor */
    ~PlaybackRecorder();

    /* Methods */
    bool isRecording();
    bool startRecording(char *filePath, int fps);
    bool stopRecording();
    void update(LeapData &leapData);
    void render();
};

#endif /* _PLAYBACKRECORDER_H_ */
