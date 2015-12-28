#ifndef __ADC_DRIVER_IF_H__
#define __ADC_DRIVER_IF_H__

//*****************************************************************************
//
// adc_driver_if.h
//
// Header file for high-level ADC driver for CC3200-LAUNCHXL
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Project: Human Interface for Robotic Control
//
// Created:
// December 28, 2015
//
// Modified:
// December 28, 2015
//
//*****************************************************************************

// Defines for fingertip sensors
enum Finger_Type {
    FINGER_THUMB,
    FINGER_INDEX,
    FINGER_MIDDLE,
    FINGER_RING
};

float GetSensorReading(enum Finger_Type eFinger);

#endif //  __ADC_DRIVER_IF_H__
