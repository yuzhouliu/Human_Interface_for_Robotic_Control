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
// January 6, 2015
//
//*****************************************************************************

// Defines number of sensors
#define NUM_SENSORS 5

// Defines number of samples for averaging ADC reading
#define NO_OF_SAMPLES 2

// Defines for finger-tip sensors
enum Fingertip_Sensor_Type {
    SENSOR_FINGER_THUMB=0,
	SENSOR_FINGER_INDEX,
	SENSOR_FINGER_MIDDLE,
	SENSOR_FINGER_RING,
	SENSOR_FINGER_PINKY
};

//****************************************************************************
// Initializes the Sensor ADCs for operation
//****************************************************************************
void InitSensorADC(void);

//****************************************************************************
// Disables the Sensor ADCs
//****************************************************************************
void DisableSensorADC(void);

//*****************************************************************************
// Gets the Sensor Reading from Finger-Tip sensors using ADC
// Returns the raw ADC value from 0 to 4096
//*****************************************************************************
unsigned short GetSensorReading(enum Fingertip_Sensor_Type eFingerSensor);

//*****************************************************************************
// Converts raw ADC value (0 to 4096) to the associated voltage (0 to 1.4V)
// Returns a float value ranging from 0 - 1.4V
//*****************************************************************************
float ConvertADCtoVolts(unsigned short input_raw_adc);

#endif //  __ADC_DRIVER_IF_H__
