//*****************************************************************************
//
// PlaybackRecorder.cpp
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
#include "PlaybackRecorder.h"

#include <iostream>

#include "SDL.h"
#include "SDL_ttf.h"

//*****************************************************************************
//
//! Empty constructor for PlaybackRecorder.
//!
//! \param window the sdl window of the application.
//!
//! \return None.
//
//*****************************************************************************
PlaybackRecorder::PlaybackRecorder(SDL_Window *window)
    : _file(), _recording(false), _delayElapsed(false), _timer()
{
    //
    // Gets the renderer and window dimensions
    //
    SDL_Renderer *renderer = SDL_GetRenderer(window);
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    //
    // Opens font to use for text
    //
    TTF_Font *font = TTF_OpenFont("data/font/kenvector_future_thin.ttf", 16);
    if (font == nullptr)
    {
        std::cout << "[ERROR] PlaybackRecorder::PlaybackRecorder(): Font "\
            "could not be loaded." << std::endl;
        return;
    }

    SDL_Color color = {0xFF, 0, 0, 0xFF}; // Red color
    SDL_Rect renderRect;

    //
    // Creates image for delay text
    //
    SDL_Surface *delayTextSurface = TTF_RenderText_Solid(font,
        "Recording will start in 2 seconds", color);
    if (delayTextSurface == nullptr)
    {
        std::cout << "[ERROR] PlaybackRecorder::PlaybackRecorder(): Surface "\
            "could not be loaded." << std::endl;
        return;
    }
    _delayText = std::unique_ptr<Image>(new Image());
    _delayText->setRenderer(renderer);
    _delayText->setTexture(delayTextSurface);
    renderRect.x = 200;
    renderRect.y = 500;
    renderRect.w = _delayText->getWidth();
    renderRect.h = _delayText->getHeight();
    _delayText->setRenderRect(renderRect);

    //
    // Creates image for recording text
    //
    SDL_Surface *recordingTextSurface = TTF_RenderText_Solid(font,
        "Recording", color);
    if (recordingTextSurface == nullptr)
    {
        std::cout << "[ERROR] PlaybackRecorder::PlaybackRecorder(): Surface "\
            "could not be loaded." << std::endl;
        return;
    }
    _recordingText = std::unique_ptr<Image>(new Image());
    _recordingText->setRenderer(renderer);
    _recordingText->setTexture(recordingTextSurface);
    renderRect.x = 300;
    renderRect.y = 500;
    renderRect.w = _recordingText->getWidth();
    renderRect.h = _recordingText->getHeight();
    _recordingText->setRenderRect(renderRect);

    //
    // Creates image for recording sprite
    //
    _recordingImage = std::unique_ptr<Image>(new Image(renderer,
        "data/gfx/pressure.png"));
    renderRect.x = 200;
    renderRect.y = 500;
    renderRect.w = _recordingImage->getWidth();
    renderRect.h = _recordingImage->getHeight();
    _recordingImage->setRenderRect(renderRect);

    SDL_FreeSurface(delayTextSurface);
    SDL_FreeSurface(recordingTextSurface);
    TTF_CloseFont(font);
}

//*****************************************************************************
//
//! Destructor for PlaybackRecorder. Releases file handle if required.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
PlaybackRecorder::~PlaybackRecorder()
{
    if (_file.is_open())
    {
        _file.close();
    }
}

//*****************************************************************************
//
//! Returns true if recording in progress.
//!
//! \param None.
//!
//! \return Returns \b true if recording in progress and \b false otherwise.
//
//*****************************************************************************
bool PlaybackRecorder::isRecording()
{
    return _recording;
}

//*****************************************************************************
//
//! Starts recording data stream to file.
//!
//! \param filePath path to file to save data stream to.
//!
//! \return Returns \b true if recording started successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool PlaybackRecorder::startRecording(char *filePath, int fps)
{
    if (_recording)
    {
        std::cout << "[WARNING] PlaybackRecorder::startRecording(): Already "\
            "recording!"<< std::endl;
        return false;
    }

    _file.open(filePath);
    if (!_file.is_open())
    {
        std::cout << "[ERROR] PlaybackRecorder::startRecording(): Cannot "\
            "open file!"<< std::endl;
        return false;
    }

    //
    // Append HIRC constant and fps as header
    //
    char fpsByte = static_cast<char>(fps);
    _file << "HIRC" << fpsByte;

    _recording = true;
    _delayElapsed = false;
    _timer.start();

    return true;
}

//*****************************************************************************
//
//! Stops recording data stream and save file.
//!
//! \param None.
//!
//! \return Returns \b true if recording stopped successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool PlaybackRecorder::stopRecording()
{
    if (!_recording)
    {
        std::cout << "[WARNING] PlaybackRecorder::stopRecording(): Not "\
            "currently recording!"<< std::endl;
        return false;
    }

    if (!_file.is_open())
    {
        std::cout << "[ERROR] PlaybackRecorder::stopRecording(): File not "\
            "open!"<< std::endl;
        return false;
    }

    _file.close();
    _recording = false;
    _timer.stop();

    return true;
}

//*****************************************************************************
//
//! Appends data to file if recording. Called once per frame.
//!
//! \param leapData LeapData structure storing data fields to record.
//!
//! \return None.
//
//*****************************************************************************
void PlaybackRecorder::update(LeapData &leapData)
{
    //
    // Not currently recording, so do not update
    //
    if (!_recording)
    {
        return;
    }

    //
    // Wait two seconds before recording to allow user to set starting position
    // of hand
    //
    unsigned int timeOnTimer = _timer.getTimeOnTimer();
    if (timeOnTimer < 2000)
    {
        std::cout << "[NOTICE] PlaybackRecorder::update(): 2 seconds has not "\
            "yet elasped. Time left = " << (2000 - timeOnTimer) << std::endl;

        return;
    }
    _delayElapsed = true;

    //
    // Appends finger rotation data to file
    //
    for (int i=0; i<NUM_FINGERS; i++)
    {
        _file << leapData.totalAngle[i];
    }

    //
    // Appends wrist rotation data to file
    //
    _file << leapData.wristAngle;
}

//*****************************************************************************
//
//! Renders any text or icon related to recording.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void PlaybackRecorder::render()
{
    if (_recording)
    {
        if (_delayElapsed)
        {
            _recordingImage->onRender();
            _recordingText->onRender();
        }
        else
        {
            _delayText->onRender();
        }
    }
}
