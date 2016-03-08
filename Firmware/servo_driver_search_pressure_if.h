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

#define POSITION_INCREMENT 5    		// Movement increment in degrees
#define RETREAT_DECREMENT 2     		// Movement to retreat if above threshold

// Pressure Threshold values for each finger before servo stops
#define PRESSURE_THRESHOLD_THUMB 2400  	// Threshold pressure before servo stops
#define PRESSURE_THRESHOLD_INDEX 900  	// Threshold pressure before servo stops
#define PRESSURE_THRESHOLD_MIDDLE 900  // Threshold pressure before servo stops
#define PRESSURE_THRESHOLD_RING 920  	// Threshold pressure before servo stops
#define PRESSURE_THRESHOLD_PINKY 920  	// Threshold pressure before servo stops

#define TOLERANCE 100           			// Tolerance value for holding position

// Position limits
#define FINGER_OPEN_POS_LIMIT 0

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
void MoveServo_SearchPressure(unsigned char ucCommand, unsigned char * data);

//****************************************************************************
// Gets the current position of the servo from the ServoPosition struct
//****************************************************************************
unsigned char GetFingerPosition(struct ServoPosition *PositionRecord, enum Fingertip_Sensor_Type eSensorJoint);

//****************************************************************************
// Updates the ServoPosition struct with the current position
//****************************************************************************
void SetFingerPosition(struct ServoPosition *PositionRecord, enum Fingertip_Sensor_Type eSensorJoint, unsigned char ucDegreesCurrent);

//****************************************************************************
// Gets the Pressure tolerance value for the finger specified
//****************************************************************************
unsigned short GetFingerTolerance( enum Fingertip_Sensor_Type eSensorJoint );

//****************************************************************************
// Converts 16-bit unsigned short to 2 8-bit unsigned chars
// highByte is MSB, lowByte is LSB following Big Endian notation
//****************************************************************************
void UnsignedShort_to_UnsignedChar(unsigned short input_16_bits, unsigned char *highByte, unsigned char *lowByte);

#endif //  __SERVO_DRIVER_IF_H__
