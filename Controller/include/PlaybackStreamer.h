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
// Feburary 27, 2016
//
//*****************************************************************************
#ifndef _PLAYBACKSTREAMER_H_
#define _PLAYBACKSTREAMER_H_

#include <fstream>

#include "SDL.h"

#include "Image.h"
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
    std::unique_ptr<Image> _playingImage;
    std::unique_ptr<Image> _playingText;
    std::unique_ptr<Image> _delayText;

public:
    /* Constructor */
    PlaybackStreamer(SDL_Renderer *renderer);

    /* Destructor */
    ~PlaybackStreamer();

    /* Methods */
    bool isStreaming();
    bool startStreaming(char *filePath);
    bool stopStreaming();
    void update(LeapData &leapData);
    int getStreamingFPS();
    void render();
};

#endif /* _PLAYBACKSTREAMER_H_ */
