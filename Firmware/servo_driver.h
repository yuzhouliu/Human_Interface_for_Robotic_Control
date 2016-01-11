#ifndef __SERVO_DRIVER_H__
#define __SERVO_DRIVER_H__

//*****************************************************************************
//
// servo_driver.h
//
// Header file for low-level Servo driver for CC3200-LAUNCHXL
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

// PWM Reload value to achieve 20 ms time period
#define PWM_PRESCALE 0x18
#define PWM_INTERVAL_RELOAD 0x6A00

//*****************************************************************************
// Sets up the Timer for PWM mode
//*****************************************************************************
void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert);

//*****************************************************************************
// Sets up the identified timers as PWM to drive the peripherals
//*****************************************************************************
void InitPWMModules();

//*****************************************************************************
// Disables Timer PWMs
//*****************************************************************************
void DeInitPWMModules();

//*****************************************************************************
// Updates the TimerMatch and PrescaleMatch for specified Timer PWM
//*****************************************************************************
void UpdatePWM_DutyCycle(unsigned long ulBase, unsigned long ulTimer,
                     unsigned short usMatchVal, unsigned char ucPrescaleVal);

#endif //  __SERVO_DRIVER_H__
