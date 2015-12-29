//*****************************************************************************
//
// LeapMotionManager.cpp
//
// Class responsible for interfacing with the Leap Motion Controller.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 28, 2015
//
// Modified:
// December 28, 2015
//
//*****************************************************************************
#include "LeapMotionManager.h"

//*****************************************************************************
//
//! Empty constructor for LeapMotionManager.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
LeapMotionManager::LeapMotionManager()
{

}

//*****************************************************************************
//
//! Empty destructor for LeapMotionManager.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
LeapMotionManager::~LeapMotionManager()
{

}

//*****************************************************************************
//
//! Processes the Leap frame. Called once every application frame.
//!
//! \param buf buffer to store processed data.
//! \param buflen the size of the buffer
//!
//! \return None.
//
//*****************************************************************************
void LeapMotionManager::processFrame(char *buf, unsigned int buflen)
{
    LeapDataStruct leapData;
    Leap::Frame frame = _controller.frame();
    Leap::HandList hands = frame.hands();

    //
    // TODO (Brandon): Populate LeapDataStruct structure with relevent info
    //


    //
    // Serialize data
    //
    _serialize(leapData, buf, buflen);
}

//*****************************************************************************
//
//! Processes the Leap frame. Called once every application frame.
//!
//! \param buf buffer to store serialized data.
//! \param buflen the size of the buffer
//!
//! \return None.
//
//*****************************************************************************
void LeapMotionManager::_serialize(LeapDataStruct &leapData, char *buf,
    unsigned int buflen)
{
    //
    // TODO (Brandon): Figure out serialization format and serialize data
    //
}
