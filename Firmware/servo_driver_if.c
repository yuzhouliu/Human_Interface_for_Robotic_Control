//*****************************************************************************
//
// servo_driver_if.c
//
// High-Level servo driver for CC3200-LAUNCHXL
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

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "timer.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"

#include "servo_driver_if.h"

// Low-level Servo driver include
#include "servo_driver.h"

//****************************************************************************
//
// Initializes the Servo motors for operation
//
// \param none
//
// \return None.
//
//****************************************************************************
void InitServos()
{
    InitPWMModules();
}

//****************************************************************************
//
// Disables the Servo motors
//
// \param none
//
// \return None.
//
//****************************************************************************
void DisableServos()
{
    DeInitPWMModules();
}

//****************************************************************************
//
// Moves the servo motor by degrees on the finger specified
//
// \param usDegrees -> 0 to 180
// \param eFinger -> finger (ex: finger_thumb, finger_index, etc)
//
// \return None.
//
//****************************************************************************
void MoveServo(unsigned char ucDegrees, enum Finger_Type eFinger)
{
    unsigned short usMatchVal;
    unsigned char usPrescaleVal;

    /*
    // Check if Degrees input is within max of the finger/wrist type
    switch(eFinger) {
        case FINGER_THUMB:
        case FINGER_INDEX:
        case FINGER_MIDDLE:
        case FINGER_RING:
        case FINGER_PINKY:
        	if (ucDegrees > FINGER_MAX_POSITION)
        	{
        		ucDegrees = FINGER_MAX_POSITION;
        	}
        	break;
        case WRIST:
        	if (ucDegrees > WRIST_MAX_POSITION)
        	{
        		ucDegrees = WRIST_MAX_POSITION;
        	}
            break;
        default:
        	//UART_PRINT("[MoveServo] Invalid Finger input\n");
            return;
    }
    */

    Convert_Degrees_To_DutyCycle(ucDegrees, &usMatchVal, &usPrescaleVal);

    switch(eFinger) {
        case FINGER_THUMB:
            UpdatePWM_DutyCycle(TIMERA3_BASE, TIMER_A, usMatchVal, usPrescaleVal);
            break;
        case FINGER_INDEX:
            UpdatePWM_DutyCycle(TIMERA2_BASE, TIMER_B, usMatchVal, usPrescaleVal);
            break;
        case FINGER_MIDDLE:
            UpdatePWM_DutyCycle(TIMERA3_BASE, TIMER_B, usMatchVal, usPrescaleVal);
            break;
        case FINGER_RING:
            UpdatePWM_DutyCycle(TIMERA1_BASE, TIMER_A, usMatchVal, usPrescaleVal);
            break;
        case FINGER_PINKY:
        	UpdatePWM_DutyCycle(TIMERA1_BASE, TIMER_B, usMatchVal, usPrescaleVal);
            break;
        case WRIST:
        	UpdatePWM_DutyCycle(TIMERA0_BASE, TIMER_A, usMatchVal, usPrescaleVal);
            break;
        default:
            //UART_PRINT("[MoveServo] Invalid Finger input\n");
        	return;
    }
}
