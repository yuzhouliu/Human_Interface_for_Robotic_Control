#ifndef __ADC_DRIVER_IF_H__
#define __ADC_DRIVER_IF_H__

//*****************************************************************************
//
// adc_driver_if.h
//
// Header file for high-level ADC driver for CC3200-LAUNCHXL
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

// Defines for fingers
enum Finger_Type {
    FINGER_THUMB,
    FINGER_INDEX,
    FINGER_MIDDLE,
    FINGER_RING,
    FINGER_PINKY,
    WRIST
};

void GetADC(enum Finger_Type eFinger);

#endif //  __ADC_DRIVER_IF_H__