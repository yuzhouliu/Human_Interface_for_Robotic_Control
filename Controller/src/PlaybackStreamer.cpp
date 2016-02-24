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
// Feburary 24, 2016
//
//*****************************************************************************
#include "PlaybackStreamer.h"

#include <iostream>
#include <string>

#include "Window.h"

//*****************************************************************************
//
//! Empty constructor for PlaybackStreamer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
PlaybackStreamer::PlaybackStreamer()
    : _file(), _streaming(false), _fps(0)
{

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
