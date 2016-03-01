//*****************************************************************************
//
// PlaybackStreamer.cpp
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
// Feburary 29, 2016
//
//*****************************************************************************
#include "PlaybackStreamer.h"

#include <iostream>
#include <string>

#include <SDL_ttf.h>

#include "Window.h"

//*****************************************************************************
//
//! Empty constructor for PlaybackStreamer.
//!
//! \param window the sdl window of the application.
//!
//! \return None.
//
//*****************************************************************************
PlaybackStreamer::PlaybackStreamer(SDL_Window *window)
    : _file(), _streaming(false), _delayElapsed(false), _fps(0), _timer()
{
    //SDL_Color color = {0x27, 0xBE, 0x64, 0xFF}; // Green color

    //
    // Gets the renderer and window dimensions
    //
    _renderer = SDL_GetRenderer(window);
    int windowWidth = PRIMARY_VIEWPORT_WIDTH - PRIMARY_VIEWPORT_X;
    int windowHeight = PRIMARY_VIEWPORT_HEIGHT - PRIMARY_VIEWPORT_Y;

    //
    // Opens font to use for text
    //
    TTF_Font *font = TTF_OpenFont("data/font/kenvector_future_thin.ttf", 20);
    if (font == nullptr)
    {
        std::cout << "[ERROR] PlaybackStreamer::PlaybackStreamer(): Font "\
            "could not be loaded." << std::endl;
        return;
    }

    //
    // TODO (Brandon): Investigate why color does not work if it is declared
    // here.
    //
    SDL_Color color = {0x27, 0xBE, 0x64, 0xFF}; // Green color
    SDL_Rect renderRect;

    //
    // Creates image for delay text
    //
    SDL_Surface *delayTextSurface = TTF_RenderText_Blended(font,
        "Playback will start in 2 seconds...", color);
    if (delayTextSurface == nullptr)
    {
        std::cout << "[ERROR] PlaybackStreamer::PlaybackStreamer(): Surface "\
            "could not be loaded." << std::endl;
        return;
    }
    _delayText = std::unique_ptr<Image>(new Image());
    _delayText->setRenderer(_renderer);
    _delayText->setTexture(delayTextSurface);
    renderRect.x = (windowWidth - _delayText->getWidth())/2;
    renderRect.y = windowHeight - 50;
    renderRect.w = _delayText->getWidth();
    renderRect.h = _delayText->getHeight();
    _delayText->setRenderRect(renderRect);

    //
    // Creates image for playing text
    //
    SDL_Surface *playingTextSurface = TTF_RenderText_Blended(font,
        "Playing", color);
    if (playingTextSurface == nullptr)
    {
        std::cout << "[ERROR] PlaybackStreamer::PlaybackStreamer(): Surface "\
            "could not be loaded." << std::endl;
        return;
    }
    _playingText = std::unique_ptr<Image>(new Image());
    _playingText->setRenderer(_renderer);
    _playingText->setTexture(playingTextSurface);
    renderRect.x = (windowWidth - _playingText->getWidth())/2 + 25;
    renderRect.y = windowHeight - 50;
    renderRect.w = _playingText->getWidth();
    renderRect.h = _playingText->getHeight();
    _playingText->setRenderRect(renderRect);

    //
    // Creates image for playing sprite
    //
    _playingImage = std::unique_ptr<Image>(new Image(_renderer,
        "data/gfx/play.png"));
    renderRect.x = renderRect.x - 50;
    renderRect.y = windowHeight - 50;
    renderRect.w = _playingText->getHeight();
    renderRect.h = renderRect.w;
    _playingImage->setRenderRect(renderRect);

    SDL_FreeSurface(delayTextSurface);
    SDL_FreeSurface(playingTextSurface);
    TTF_CloseFont(font);
}

//*****************************************************************************
//
//! Destructor for PlaybackStreamer. Releases file handle if required.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
PlaybackStreamer::~PlaybackStreamer()
{
    if (_file.is_open())
    {
        _file.close();
    }
}

//*****************************************************************************
//
//! Returns true if streaming in progress.
//!
//! \param None.
//!
//! \return Returns \b true if streaming in progress and \b false otherwise.
//
//*****************************************************************************
bool PlaybackStreamer::isStreaming()
{
    return _streaming;
}

//*****************************************************************************
//
//! Starts streaming data from file.
//!
//! \param filePath path to file to stream data from.
//!
//! \return Returns \b true if streaming started successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool PlaybackStreamer::startStreaming(char *filePath)
{
    if (_streaming)
    {
        std::cout << "[WARNING] PlaybackStreamer::startStreaming(): Already "\
            "streaming!"<< std::endl;
        return false;
    }

    _file.open(filePath, std::fstream::binary);
    if (!_file.is_open())
    {
        std::cout << "[ERROR] PlaybackStreamer::startStreaming(): Cannot "\
            "open file!"<< std::endl;
        return false;
    }

    //
    // Check for HIRC constant
    //
    char hircBuf[5+1];
    _file.read(hircBuf, 5);
    hircBuf[5] = '\0';
    std::string hircString(hircBuf);
    if (hircString != "HIRC\r")
    {
        std::cout << "[ERROR] PlaybackStreamer::startStreaming(): Not an "\
            "HIRC file!"<< std::endl;
        return false;
    }

    //
    // Check the fps
    //
    char fpsBuf;
    _file.read(&fpsBuf, 1);
    _fps = static_cast<int>(fpsBuf);

    _streaming = true;
    _delayElapsed = false;
    _timer.start();

    return true;
}

//*****************************************************************************
//
//! Stops streaming data.
//!
//! \param None.
//!
//! \return Returns \b true if streaming stopped successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool PlaybackStreamer::stopStreaming()
{
    //
    // TODO (Brandon): This could cause a state issue when startStream fails
    // because file.close() is called after this. Fix this later.
    //
    if (!_streaming)
    {
        std::cout << "[WARNING] PlaybackStreamer::stopStreaming(): Not "\
            "currently streaming!"<< std::endl;
        return false;
    }

    if (!_file.is_open())
    {
        std::cout << "[ERROR] PlaybackStreamer::stopStreaming(): File not "\
            "open!"<< std::endl;
        return false;
    }

    _file.close();
    _streaming = false;
    _fps = 0;
    _timer.stop();

    return true;
}

//*****************************************************************************
//
//! Populates leapData if streaming. Called once per frame.
//!
//! \param leapData LeapData structure storing data fields to stream to.
//!
//! \return None.
//
//*****************************************************************************
void PlaybackStreamer::update(LeapData &leapData)
{
    //
    // Not currently streaming, so do not update
    //
    if (!_streaming)
    {
        return;
    }

    char angleData[NUM_FINGERS+1]; // NUM_FINGERS + WRIST
    _file.read(angleData, NUM_FINGERS+1);

    //
    // File streaming complete
    //
    if (_file.rdstate() & std::fstream::eofbit)
    {
        notify(EVENT_STOP_STREAMING);
        return;
    }

    //
    // Wait two seconds before starting playback stream to allow InMoov hand
    // to synchronize with recorded movement
    //
    unsigned int timeOnTimer = _timer.getTimeOnTimer();
    if (timeOnTimer < 2000)
    {
        //
        // Return to start position (continually stream starting position for
        // two seconds
        //
        _file.seekg(6);

        std::cout << "[NOTICE] PlaybackStreamer::update(): 2 seconds has not "\
            "yet elasped. Time left = " << (2000 - timeOnTimer) << std::endl;

        return;
    }
    _delayElapsed = true;

    //
    // Updates struct with recorded data for playback streaming
    //
    int i;
    for (i=0; i<NUM_FINGERS; i++)
    {
        leapData.totalAngle[i] = angleData[i];
    }
    leapData.wristAngle = angleData[i];
}

//*****************************************************************************
//
//! Gets the maximum FPS that the stream will maintain.
//!
//! \param None.
//!
//! \return The maximum FPS that the stream will maintain.
//
//*****************************************************************************
int PlaybackStreamer::getStreamingFPS()
{
    return _fps;
}

//*****************************************************************************
//
//! Renders any text or icon related to streaming.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void PlaybackStreamer::render()
{
    if (_streaming)
    {
        SDL_RenderSetViewport(_renderer, &Window::gPrimaryViewport);
        if (_delayElapsed)
        {
            _playingImage->onRender();
            _playingText->onRender();
        }
        else
        {
            _delayText->onRender();
        }
    }
}
