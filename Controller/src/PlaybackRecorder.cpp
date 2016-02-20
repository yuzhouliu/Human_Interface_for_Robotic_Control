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
// Feburary 19, 2016
//
//*****************************************************************************
#include "PlaybackRecorder.h"

#include <iostream>

//*****************************************************************************
//
//! Empty constructor for PlaybackRecorder.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
PlaybackRecorder::PlaybackRecorder()
    : _file(), _recording(false)
{

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

    _file << "HIRC" << fps;

    _recording = true;
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
            "recording!"<< std::endl;
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
    return true;
}

//*****************************************************************************
//
//! Appends data to file if recording. Called once per frame.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void PlaybackRecorder::update(char *data)
{
    if (!_recording)
    {
        return;
    }

    // TODO (Brandon): Append data to file
}
