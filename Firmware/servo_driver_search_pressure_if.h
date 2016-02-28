#ifndef __SERVO_DRIVER_SEARCH_PRESSURE_IF_H__
#define __SERVO_DRIVER_SEARCH_PRESSURE_IF_H__

//*****************************************************************************
//
// servo_driver_search_pressure_if.h
//
// Header file for high-level Servo driver that searches for pressure to be
// over threshold and retracts.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Project: Human Interface for Robotic Control
//
// Created:
// February 18, 2016
//
// Modified:
// February 18, 2016
//
//*****************************************************************************

#define CMD_CLOSE 0
#define CMD_OPEN 1

#define SCALE_SPEED 1

#define POSITION_INCREMENT 5    // Movement increment in degrees
#define RETREAT_DECREMENT 2     // Movement to retreat if above threshold
#define PRESSURE_THRESHOLD 2400  // Threshold pressure before servo stops
#define TOLERANCE 50           // Tolerance value for holding position

struct ServoPosition {
    // Current servo position
    unsigned char ucThumb_Current;
    unsigned char ucIndex_Current;
    unsigned char ucMiddle_Current;
    unsigned char ucRing_Current;
    unsigned char ucPinky_Current;
};

#include "adc_driver_if.h"

//****************************************************************************
// Moves the servo motor by degrees on the finger specified
// Moves in increments, checking to see if pressure is over threshold
//****************************************************************************
void MoveServo_SearchPressure(unsigned char ucCommand);

//****************************************************************************
// Gets the current position of the servo from the ServoPosition struct
//****************************************************************************
unsigned char GetFingerPosition(struct ServoPosition *PositionRecord, enum Fingertip_Sensor_Type eSensorJoint);

//****************************************************************************
// Updates the ServoPosition struct with the current position
//****************************************************************************
void SetFingerPosition(struct ServoPosition *PositionRecord, enum Fingertip_Sensor_Type eSensorJoint, unsigned char ucDegreesCurrent);

#endif //  __SERVO_DRIVER_IF_H__
