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

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "timer.h"
#include "utils.h"
#include "prcm.h"
#include "common.h"
#include "uart.h"
#include "uart_if.h"

#include "servo_driver_if.h"
#include "servo_driver_search_pressure_if.h"
#include "servo_driver.h"
#include "adc_driver_if.h"
#include "msg_util_if.h"

//****************************************************************************
// Moves the servo motor by degrees on the finger specified
//****************************************************************************
void MoveServo_SearchPressure(unsigned char ucCommand, char * data)
{
    static struct ServoPosition ServoPositionRecord;    // Track current position
    unsigned char ucDegreesCurrent;                     // Current Position of finger
    
    unsigned char ucFingerIndex;                        // Temp i for looping through fingers
    unsigned char ucIterationIndex;                     // Temp i for iterations

    unsigned char ucDegreesLimit;						// The position limit for the finger
    unsigned short usPressureSensorReading;             // Stores pressure sensor reading
    unsigned short usPressureSensorTolerance;			// Tolerance for the current finger

    unsigned char highByte;								// Storing High Byte of sensor reading
    unsigned char lowByte;								// Storing Low Byte of sensor reading


    // Close/Open the hand incrementally _x_ number of times
    for (ucIterationIndex = 0; ucIterationIndex < SCALE_SPEED; ucIterationIndex++)
    {
        // Increment each finger
        for (ucFingerIndex = 0; ucFingerIndex < NUM_FINGERS; ucFingerIndex++)
        {
            // Get the current position of the finger
            ucDegreesCurrent = GetFingerPosition(&ServoPositionRecord, (enum Fingertip_Sensor_Type)ucFingerIndex);

            // Get the max position for this finger
            ucDegreesLimit = GetFingerPositionLimit((enum Servo_Joint_Type) ucFingerIndex);

            // Get the Pressure Sensor Tolerance for current finger
            usPressureSensorTolerance = GetFingerTolerance( (enum Fingertip_Sensor_Type)ucFingerIndex );

            // Get the Pressure sensor reading
            usPressureSensorReading = GetSensorReading((enum Fingertip_Sensor_Type)ucFingerIndex);
            UART_PRINT("Finger: %d, Reading: %d, Tolerance: %d ", ucFingerIndex, usPressureSensorReading, usPressureSensorTolerance);

            // Close the hand incrementally checking the pressure sensor each time
            if (ucCommand == CMD_CLOSE)
            {
            	//ADC reading BELOW threshold(bigger value means less pressure), Move towards Target
            	if (usPressureSensorReading > usPressureSensorTolerance)
            	{
            		UART_PRINT(" ++ ");

            		// Limit the position of the finger
            		if ((ucDegreesCurrent += POSITION_INCREMENT) > ucDegreesLimit)
					{
					   ucDegreesCurrent = ucDegreesLimit;
					}
            	}
            	// ADC reading ABOVE threshold+tolerance (smaller value means more pressure), retreat position
            	else if (usPressureSensorReading < (usPressureSensorTolerance + TOLERANCE) )
            	{
            		UART_PRINT(" -- ");
            		if (ucDegreesCurrent < (FINGER_OPEN_POS_LIMIT + RETREAT_DECREMENT))
					{
						ucDegreesCurrent = FINGER_OPEN_POS_LIMIT;
					}
					else
					{
						ucDegreesCurrent -= RETREAT_DECREMENT;
					}
            	}
            	// We are right in threshold+tolerance, then do nothing
            	else
            	{
            		UART_PRINT(" // ");
            	}
            }
            // Set the fingers to open position
            else if (ucCommand == CMD_OPEN)
            {
            	ucDegreesCurrent = FINGER_OPEN_POS_LIMIT;
            }

            UART_PRINT(" DegreesCurrent: %d\n\r", ucDegreesCurrent);
            // Move the servo, update the record struct
            MoveServo_PWM_Breakout(ucDegreesCurrent, (enum Servo_Joint_Type)ucFingerIndex);
            SetFingerPosition(&ServoPositionRecord, (enum Fingertip_Sensor_Type)ucFingerIndex, ucDegreesCurrent);

            // Convert the sensor reading to it's high and low bytes and store in data array
            UnsignedShort_to_UnsignedChar(usPressureSensorReading, &highByte, &lowByte);
            data[ucFingerIndex*2] = (char)highByte;
            data[ucFingerIndex*2+1] = (char)lowByte;
        }
    }
}


//****************************************************************************
// Gets the current position of the servo from the ServoPosition struct
//****************************************************************************
unsigned char GetFingerPosition(struct ServoPosition *PositionRecord, enum Fingertip_Sensor_Type eSensorJoint)
{
    switch(eSensorJoint)
    {
        case SENSOR_FINGER_THUMB:
            return PositionRecord->ucThumb_Current;
        case SENSOR_FINGER_INDEX:
            return PositionRecord->ucIndex_Current;
        case SENSOR_FINGER_MIDDLE:
            return PositionRecord->ucMiddle_Current;
        case SENSOR_FINGER_RING:
            return PositionRecord->ucRing_Current;
        case SENSOR_FINGER_PINKY:
            return PositionRecord->ucPinky_Current;
        default:
        	return 0;
    }
}

//****************************************************************************
// Updates the ServoPosition struct with the current position
//****************************************************************************
void SetFingerPosition(struct ServoPosition *PositionRecord, enum Fingertip_Sensor_Type eSensorJoint, unsigned char ucDegreesCurrent)
{
    switch(eSensorJoint)
    {
        case SENSOR_FINGER_THUMB:
            PositionRecord->ucThumb_Current = ucDegreesCurrent;
            break;
        case SENSOR_FINGER_INDEX:
            PositionRecord->ucIndex_Current = ucDegreesCurrent;
            break;
        case SENSOR_FINGER_MIDDLE:
            PositionRecord->ucMiddle_Current = ucDegreesCurrent;
            break;
        case SENSOR_FINGER_RING:
            PositionRecord->ucRing_Current = ucDegreesCurrent;
            break;
        case SENSOR_FINGER_PINKY:
            PositionRecord->ucPinky_Current = ucDegreesCurrent;
            break;
    }
}

//****************************************************************************
// Gets the Pressure tolerance value for the finger specified
//****************************************************************************
unsigned short GetFingerTolerance( enum Fingertip_Sensor_Type eSensorJoint )
{
	switch(eSensorJoint)
	{
		case SENSOR_FINGER_THUMB:
			return PRESSURE_THRESHOLD_THUMB;
		case SENSOR_FINGER_INDEX:
			return PRESSURE_THRESHOLD_INDEX;
		case SENSOR_FINGER_MIDDLE:
			return PRESSURE_THRESHOLD_MIDDLE;
		case SENSOR_FINGER_RING:
			return PRESSURE_THRESHOLD_RING;
		case SENSOR_FINGER_PINKY:
			return PRESSURE_THRESHOLD_PINKY;
		default:
			return 0;
	}
}
