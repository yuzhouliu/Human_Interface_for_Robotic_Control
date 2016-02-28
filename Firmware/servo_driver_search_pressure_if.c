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

//****************************************************************************
// Moves the servo motor by degrees on the finger specified
//****************************************************************************
void MoveServo_SearchPressure(unsigned char ucCommand)
{
    static struct ServoPosition ServoPositionRecord;    // Track current position
    //static unsigned char ucPreviousCommand = 1;         // Store previous command (open/close)
    //static unsigned char ucDegreesTarget = 0;           // Target degrees
    unsigned char ucDegreesCurrent;                     // Current Position of finger
    
    unsigned char ucFingerIndex;                        // Temp i
    unsigned char ucIterationIndex;                     // Temp i for iterations
    unsigned short usPressureSensorReading;             // Stores pressure sensor reading

    // If command has changed, update target position
    //if (ucCommand != ucPreviousCommand)
    //{
    //    if (ucCommand == CMD_CLOSE)
    //    {
    //        ucDegreesTarget = FINGER_THUMB_POS_LIMIT;
    //    }
    //    else if (ucCommand == CMD_OPEN)
    //    {
    //        ucDegreesTarget = FINGER_MIN_POS_LIMIT;
    //    }
    //}

    // Close/Open the hand incrementally _x_ number of times
    for (ucIterationIndex = 0; ucIterationIndex < SCALE_SPEED; ucIterationIndex++)
    {
        // Increment each finger
        for (ucFingerIndex = 0; ucFingerIndex < NUM_FINGERS; ucFingerIndex++)
        {
            // Get the current position of the finger
            ucDegreesCurrent = GetFingerPosition(&ServoPositionRecord, (enum Fingertip_Sensor_Type)ucFingerIndex);

            // If the target has already been reached for this finger, skip to next finger
            /*
            if ( ucDegreesCurrent == ucDegreesTarget )
            {
                continue;
            }
            */

            // Get the Pressure sensor reading
            usPressureSensorReading = GetSensorReading((enum Fingertip_Sensor_Type)ucFingerIndex);
            UART_PRINT("Sensor Reading: %d\n\r", usPressureSensorReading);

            // Move servo accordingly with respect to Pressure sensor reading
            if (usPressureSensorReading > PRESSURE_THRESHOLD)
            {
                //ADC reading BELOW (smaller value means more pressure) threshold, Move servo towards Target
                if (ucCommand == CMD_CLOSE)
                {
                    if ((ucDegreesCurrent += POSITION_INCREMENT) > FINGER_THUMB_POS_LIMIT)
                    {
                       ucDegreesCurrent = FINGER_THUMB_POS_LIMIT;
                    }
                }
                else if (ucCommand == CMD_OPEN)
                {
                    if (ucDegreesCurrent < (FINGER_MINIMUM_POS_LIMIT + POSITION_INCREMENT))
                    {
                        ucDegreesCurrent = FINGER_MINIMUM_POS_LIMIT;
                    }
                    else 
                    {
                        ucDegreesCurrent -= POSITION_INCREMENT;
                    }
                }       
            }        
            else if (usPressureSensorReading < (PRESSURE_THRESHOLD + TOLERANCE) )
            {
                // ABOVE threshold+tolerance (smaller value means more pressure), retreat by _x_ degrees each time
                if (ucCommand == CMD_CLOSE)
                {
                    if (ucDegreesCurrent < (FINGER_MINIMUM_POS_LIMIT + RETREAT_DECREMENT))
                    {
                        ucDegreesCurrent = FINGER_MINIMUM_POS_LIMIT;
                    }
                    else 
                    {
                        ucDegreesCurrent -= RETREAT_DECREMENT;
                    }
                }
                else if (ucCommand == CMD_OPEN)
                {
                    if ((ucDegreesCurrent += RETREAT_DECREMENT) > FINGER_THUMB_POS_LIMIT)
                    {
                       ucDegreesCurrent = FINGER_THUMB_POS_LIMIT;
                    }
                }
            }
            else
            {
                // We are right in threshold+tolerance, then do nothing
            }              

            // Move the servo, update the record struct
            MoveServo(ucDegreesCurrent, (enum Servo_Joint_Type)ucFingerIndex);
            SetFingerPosition(&ServoPositionRecord, (enum Fingertip_Sensor_Type)ucFingerIndex, ucDegreesCurrent);
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
