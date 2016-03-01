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

#ifndef __SERVO_DRIVER_IF_H__
#define __SERVO_DRIVER_IF_H__

// Define number of servos (Fingers + Wrist)
#define NUM_SERVOS 6

// Define number of fingers
#define NUM_FINGERS 5

// Defines mapping on PWM Breakout Board
#define FINGER_THUMB_MAPPING 0
#define FINGER_INDEX_MAPPING 1
#define FINGER_MIDDLE_MAPPING 2
#define FINGER_RING_MAPPING 3
#define FINGER_PINKY_MAPPING 4
#define FINGER_WRIST_MAPPING 5

// Define the PWM Duty Cycle for degrees for HS311 Servo
#define HS311_MAX_DEG_LIMIT 180									// 180 max
#define HS311_PERIOD 20											// 20ms period
#define HS311_PWM_0_DEGREES_PERIOD 0.5							// 0.5ms duty cycle for 0
#define HS311_PWM_180_DEGREES_PERIOD 2.5						// 2.5ms duty cycle for 180
#define HS311_PWM_0_DEGREES_DUTY_CYCLE 2.5						// Duty cycle percent for 0 degrees
#define HS311_PWM_DUTY_CYCLE_SCALING 10.0						 // Duty cycle percent scaling

// Define the PWM Duty Cyle Match values for HS-311 servo
// Old code for CC3200 output
#define HS311_PWM_0_DEGREES 0x9C40UL                            // 0.5ms (out of 20ms) duty cycle
#define HS311_PWM_180_DEGREES 0x30D54UL                         // 2.5ms (out of 20ms) duty cycle
#define HS311_PWM_MATCH_PER_DEGREE (0x30D54UL-0x9C40UL)/180     // Equates to 0x379 or 889

// Define the PWM Duty Cycle for degrees for HK15298B servo
#define HK15298B_MAX_DEG_LIMIT 100
#define HK15298B_PERIOD 20										// 20ms period
#define HK15298B_PWM_0_DEGREES_PERIOD 1.1						// 1.1ms duty cycle for 0
#define HK15298B_PWM_180_DEGREES_PERIOD 1.9						// 1.9ms duty cycle for 100
#define HK15298B_PWM_0_DEGREES_DUTY_CYCLE 5.5					// Duty cycle percent for 0 degrees
#define HK15298B_PWM_DUTY_CYCLE_SCALING 4.0 					// Duty cycle percent scaling

// Define the PWM Duty Cyle Match values for HK15298B servo
// Old code for CC3200 output
//#define HK15298B_PWM_0_DEGREES 0x11940UL						// 0.9ms (out of 20ms) duty cycle
#define HK15298B_PWM_0_DEGREES 0x157C0UL                        // 1.1ms (out of 20ms) duty cycle
#define HK15298B_PWM_100_DEGREES 0x251C0UL                      // 1.9ms (out of 20ms) duty cycle
#define HK15298B_PWM_MATCH_PER_DEGREE (0x251C0UL-0x157C0UL)/100 // Equates to 0x280 or 640    

// Define max position for wrist and fingers
#define FINGER_THUMB_POS_LIMIT 90      // HK15298B servo
#define FINGER_INDEX_POS_LIMIT 130      // HK15298B servo
#define FINGER_MIDDLE_POS_LIMIT 130     // HK15298B servo
#define FINGER_RING_POS_LIMIT 130       // HK15298B servo
#define FINGER_PINKY_POS_LIMIT 130      // HK15298B servo
#define WRIST_POS_LIMIT 180             // HS311 servo
// Define min position for all servos
#define FINGER_MINIMUM_POS_LIMIT 30		// Anything below x degrees is interpreted as 0.

// Scaling for each finger
#define FINGER_THUMB_SCALE HK15298B_MAX_DEG_LIMIT/(FINGER_THUMB_POS_LIMIT-FINGER_MINIMUM_POS_LIMIT)
#define FINGER_INDEX_SCALE HK15298B_MAX_DEG_LIMIT/(FINGER_INDEX_POS_LIMIT-FINGER_MINIMUM_POS_LIMIT)
#define FINGER_MIDDLE_SCALE HK15298B_MAX_DEG_LIMIT/(FINGER_MIDDLE_POS_LIMIT-FINGER_MINIMUM_POS_LIMIT)
#define FINGER_RING_SCALE HK15298B_MAX_DEG_LIMIT/(FINGER_RING_POS_LIMIT-FINGER_MINIMUM_POS_LIMIT)
#define FINGER_PINKY_SCALE HK15298B_MAX_DEG_LIMIT/(FINGER_PINKY_POS_LIMIT-FINGER_MINIMUM_POS_LIMIT)

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
// Uses CC3200 Board
//****************************************************************************
void InitServos();

//****************************************************************************
// Initializes the Servo motors for operation
// Uses PWM Breakout Board
//****************************************************************************
void InitServos_PWM_Breakout();

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
// Uses CC3200
//*****************************************************************************
void Convert_Degrees_To_DutyCycle(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint,
                                    unsigned short *usMatchVal, unsigned char *ucPrescaleVal);

//*****************************************************************************
// Converts the input of degrees to a Match value and Prescale value
// Uses PWM Breakout Board
//*****************************************************************************
float Convert_Degrees_To_DutyCycle_PWM_Breakout(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint);

//****************************************************************************
// Moves the servo motor by degrees on the finger specified
// Uses CC3200 Board
//****************************************************************************
void MoveServo(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint);

//****************************************************************************
// Moves the servo motor by degrees on the finger specified
// Uses PWM Breakout Board
//****************************************************************************
void MoveServo_PWM_Breakout(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint);

//****************************************************************************
// Gets the Position limit of the Finger
//****************************************************************************
unsigned char GetFingerPositionLimit(enum Servo_Joint_Type eServoJoint);

#endif //  __SERVO_DRIVER_IF_H__
