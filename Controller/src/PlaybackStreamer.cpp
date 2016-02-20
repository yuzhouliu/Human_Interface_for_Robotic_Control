
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
// Feburary 19, 2016
//
//*****************************************************************************
#include "PlaybackStreamer.h"

#include <iostream>

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
    : _file(), _streaming(false)
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

    _file.open(filePath);
    if (!_file.is_open())
    {
        std::cout << "[ERROR] PlaybackStreamer::startStreaming(): Cannot "\
            "open file!"<< std::endl;
        return false;
    }

    // TODO (Brandon): Check for HIRC constant

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
    // TODO (Brandon): Implement
    return false;
}
