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
// January 6, 2015
//
//*****************************************************************************

// Define number of servos (Fingers + Wrist)
#define NUM_SERVOS 6

// The following values are calculated using technique described in servo_driver.c
// Define the PWM Duty Cyle Match values for HS-311 servo
#define HS311_MAX_DEG_LIMIT 180
#define HS311_PWM_0_DEGREES 0x9C40UL                            // 0.5ms (out of 20ms) duty cycle
#define HS311_PWM_180_DEGREES 0x30D54UL                         // 2.5ms (out of 20ms) duty cycle
#define HS311_PWM_MATCH_PER_DEGREE (0x30D54UL-0x9C40UL)/180     // Equates to 0x379 or 889

// Define the PWM Duty Cycle Match values for HK15298B servo
#define HK15298B_MAX_DEG_LIMIT 100
#define HK15298B_PWM_0_DEGREES 0x11940UL						// 0.9ms (out of 20ms) duty cycle
//#define HK15298B_PWM_0_DEGREES 0x157C0UL                        // 1.1ms (out of 20ms) duty cycle
#define HK15298B_PWM_100_DEGREES 0x251C0UL                      // 1.9ms (out of 20ms) duty cycle
#define HK15298B_PWM_MATCH_PER_DEGREE (0x251C0UL-0x157C0UL)/100 // Equates to 0x280 or 640    

// Define max position for wrist and fingers
#define FINGER_THUMB_POS_LIMIT 100      // HK15298B servo
#define FINGER_INDEX_POS_LIMIT 100      // HK15298B servo
#define FINGER_MIDDLE_POS_LIMIT 100     // HK15298B servo
#define FINGER_RING_POS_LIMIT 100       // HK15298B servo
#define FINGER_PINKY_POS_LIMIT 100      // HK15298B servo
#define WRIST_POS_LIMIT 180             // HS311 servo


// Defines for fingers
enum Servo_Joint_Type {
    SERVO_FINGER_THUMB=0,
    SERVO_FINGER_INDEX=1,
    SERVO_FINGER_MIDDLE=2,
    SERVO_FINGER_RING=3,
    SERVO_FINGER_PINKY=4,
    SERVO_WRIST=5
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
// Checks if the input degrees is within the limits of the servo type
//****************************************************************************
void CheckDegreeAgainstLimit(unsigned char *ucDegrees, enum Servo_Joint_Type eServoJoint);

//*****************************************************************************
// Converts the input of degrees to a Match value and Prescale value
//*****************************************************************************
void Convert_Degrees_To_DutyCycle(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint,
                                    unsigned short *usMatchVal, unsigned char *ucPrescaleVal);

//****************************************************************************
// Moves the servo motor by degrees on the finger specified
//****************************************************************************
void MoveServo(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint);

//****************************************************************************
// Stops the servo motor from moving further
//****************************************************************************
void StopServo(enum Servo_Joint_Type eFinger);

#endif //  __SERVO_DRIVER_IF_H__
