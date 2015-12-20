#ifndef __SERVO_DRIVER_IF_H__
#define __SERVO_DRIVER_IF_H__

//*****************************************************************************
//
// servo_driver.h
//
// Header file for high-level Servo driver for CC3200-LAUNCHXL
//
// Project: Human Interface for Robotic Control
//
// Created:
// December 20, 2015
//
// Modified:
// December 20, 2015
//
//*****************************************************************************

// Defines for fingers
enum enum_Finger_Type {
    finger_thumb,
    finger_index,
    finger_middle,
    finger_ring,
    finger_pinky
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
void MoveServo(unsigned short usDegrees, enum_Finger_Type eFinger);

#endif //  __SERVO_DRIVER_IF_H__