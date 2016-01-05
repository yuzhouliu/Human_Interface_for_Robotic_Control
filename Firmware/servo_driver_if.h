#ifndef __SERVO_DRIVER_IF_H__
#define __SERVO_DRIVER_IF_H__

//*****************************************************************************
//
// servo_driver_if.h
//
// Header file for high-level Servo driver for CC3200-LAUNCHXL
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Project: Human Interface for Robotic Control
//
// Created:
// December 20, 2015
//
// Modified:
// January 4, 2016
//
//*****************************************************************************

// Define number of fingers
#define NUM_FINGERS 6

// Define max position for wrist and fingers
#define WRIST_MAX_POSITION 180
#define FINGER_MAX_POSITION 90

// Defines for fingers
enum Finger_Type {
    FINGER_THUMB=0,
    FINGER_INDEX=1,
    FINGER_MIDDLE=2,
    FINGER_RING=3,
    FINGER_PINKY=4,
    WRIST=5
};

//****************************************************************************
// Initializes the Servo motors for operation
//****************************************************************************
void InitServos();

//****************************************************************************
// Disables the Servo motors
//****************************************************************************
void DisableServos();

//****************************************************************************
// Moves the servo motor by degrees on the finger specified
//****************************************************************************
void MoveServo(unsigned char ucDegrees, enum Finger_Type eFinger);

#endif //  __SERVO_DRIVER_IF_H__
