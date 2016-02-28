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
// January 6, 2015
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
#include "pwm_break_out_if.h"

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
// Initializes the Servo motors for operation
//
// \param none
//
// \return None.
//
//****************************************************************************
void InitServos_PWM_Breakout()
{
    PWM_reset();
    setPWMFreq(50);			// Set the frequency of the PWM output to  50Hz (20ms) for servo
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
// Checks if the input degrees is within the limits of the servo type
//
// \param *ucDegrees -> Pointer to Value from 0 to 255
// \param eFinger -> finger (ex: finger_thumb, finger_index, etc)
//
// \return None. *ucDegrees modified if above limit
//
//****************************************************************************
void CheckDegreeAgainstLimit(unsigned char *ucDegrees, enum Servo_Joint_Type eServoJoint)
{
	float calibrated_degree;

    switch(eServoJoint)
    {
        case SERVO_FINGER_THUMB:
        	if (*ucDegrees < FINGER_MINIMUM_POS_LIMIT)
        	{
        		*ucDegrees = 0;
        	}
        	else
        	{
        		calibrated_degree = (float) (*ucDegrees-FINGER_MINIMUM_POS_LIMIT);
        		*ucDegrees = (unsigned char) (calibrated_degree * FINGER_THUMB_SCALE);
        	}
            break;
        case SERVO_FINGER_INDEX:
        	if (*ucDegrees < FINGER_MINIMUM_POS_LIMIT)
			{
				*ucDegrees = 0;
			}
			else
			{
				calibrated_degree = (float) (*ucDegrees-FINGER_MINIMUM_POS_LIMIT);
				*ucDegrees = (unsigned char) (calibrated_degree * FINGER_INDEX_SCALE);
			}
            break;
        case SERVO_FINGER_MIDDLE:
        	if (*ucDegrees < FINGER_MINIMUM_POS_LIMIT)
			{
				*ucDegrees = 0;
			}
			else
			{
				calibrated_degree = (float) (*ucDegrees-FINGER_MINIMUM_POS_LIMIT);
				*ucDegrees = (unsigned char) (calibrated_degree * FINGER_MIDDLE_SCALE);
			}
            break;
        case SERVO_FINGER_RING:
        	if (*ucDegrees < FINGER_MINIMUM_POS_LIMIT)
			{
				*ucDegrees = 0;
			}
			else
			{
				calibrated_degree = (float) (*ucDegrees-FINGER_MINIMUM_POS_LIMIT);
				*ucDegrees = (unsigned char) (calibrated_degree * FINGER_RING_SCALE);
			}
            break;
        case SERVO_FINGER_PINKY:
        	if (*ucDegrees < FINGER_MINIMUM_POS_LIMIT)
			{
				*ucDegrees = 0;
			}
			else
			{
				calibrated_degree = (float) (*ucDegrees-FINGER_MINIMUM_POS_LIMIT);
				*ucDegrees = (unsigned char) (calibrated_degree * FINGER_PINKY_SCALE);
			}
            break;
        case SERVO_WRIST:
            if (*ucDegrees > WRIST_POS_LIMIT) 
            {
            	*ucDegrees = WRIST_POS_LIMIT;
            }
            break;
        default:
            //UART_PRINT("[CheckDegreeAgainstLimit] Invalid Finger input\n");
            return;
    }

    return;
}

//*****************************************************************************
// Converts the input of degrees to a Match value and Prescale value
//
// \param usDegrees is the rotation in degrees
// \param matchVal is reference to 16-bit value loaded into match
// \param prescaleVal is reference to 8-bt value loaded into prescale
//
// \return None. (all values updated by reference)
//*****************************************************************************
void Convert_Degrees_To_DutyCycle(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint, 
                                    unsigned short *usMatchVal, unsigned char *ucPrescaleVal)
{
    unsigned int uiDegreesToMatch;

    switch(eServoJoint) {
        case SERVO_FINGER_THUMB:
        case SERVO_FINGER_INDEX:
        case SERVO_FINGER_MIDDLE:
        case SERVO_FINGER_RING:
        case SERVO_FINGER_PINKY:
            uiDegreesToMatch = HK15298B_PWM_0_DEGREES + (ucDegrees * HK15298B_PWM_MATCH_PER_DEGREE);
            break;
        case SERVO_WRIST:
            uiDegreesToMatch = HS311_PWM_0_DEGREES + (ucDegrees * HS311_PWM_MATCH_PER_DEGREE);
            break;
        default:
            //UART_PRINT("[Convert_Degrees_To_DutyCycle] Invalid Servo input\n");
            return;
    }
    
    // Break up the match value to a prescale (MSB), and match value (2 LSB)
    *usMatchVal = uiDegreesToMatch & 0xFFFF;
    *ucPrescaleVal = (uiDegreesToMatch >> 16) & 0xFF;

    return;
}

//*****************************************************************************
// Converts the input of degrees to a Match value and Prescale value
//
// \param usDegrees is the rotation in degrees
//
// \return None. (all values updated by reference)
//*****************************************************************************
float Convert_Degrees_To_DutyCycle_PWM_Breakout(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint)
{
	float percent_of_limit;
	float intermediate;
    float pwm_duty_cycle;

    switch(eServoJoint) {
        case SERVO_FINGER_THUMB:
        case SERVO_FINGER_INDEX:
        case SERVO_FINGER_MIDDLE:
        case SERVO_FINGER_RING:
        case SERVO_FINGER_PINKY:
        	percent_of_limit = (float)((float)ucDegrees/HK15298B_MAX_DEG_LIMIT);
        	intermediate = percent_of_limit*HK15298B_PWM_DUTY_CYCLE_SCALING;
        	pwm_duty_cycle = intermediate + HK15298B_PWM_0_DEGREES_DUTY_CYCLE;
            break;
        case SERVO_WRIST:
        	percent_of_limit = (float)((float)ucDegrees/HS311_MAX_DEG_LIMIT);
        	intermediate = percent_of_limit*HS311_PWM_DUTY_CYCLE_SCALING;
        	pwm_duty_cycle = intermediate + HS311_PWM_0_DEGREES_DUTY_CYCLE;
            break;
        default:
            UART_PRINT("[Convert_Degrees_To_DutyCycle] Invalid Servo input\n\r");
            return 0.0;
    }

    UART_PRINT("percent_of_limit: %f\n\r", percent_of_limit);
    UART_PRINT("intermediate: %f\n\r", intermediate);
    UART_PRINT("HK15298B_PWM_DUTY_CYCLE_SCALING: %f\n\r", HK15298B_PWM_DUTY_CYCLE_SCALING);
    UART_PRINT("HK15298B_PWM_0_DEGREES_DUTY_CYCLE: %f\n\r", HK15298B_PWM_0_DEGREES_DUTY_CYCLE);
    UART_PRINT("pwm_duty_cycle: %f\n\r", pwm_duty_cycle);
    return pwm_duty_cycle;
}

//****************************************************************************
//
// Moves the servo motor by degrees on the finger specified
//
// \param ucDegrees -> 0 to 100 for fingers, 0 to 180 for wrist
// \param eServoJoint -> Servo Joint type (ex: finger_thumb, finger_index, etc)
//
// \return None.
//
//****************************************************************************
void MoveServo(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint)
{
    unsigned short usMatchVal;
    unsigned char usPrescaleVal;

    // Checks the input Degrees against the limits specified for the Servo of the joint
    CheckDegreeAgainstLimit(&ucDegrees, eServoJoint);

    // Converts the degrees specified to the appropriate duty cycle
    Convert_Degrees_To_DutyCycle(ucDegrees, eServoJoint, &usMatchVal, &usPrescaleVal);

    // Updates the proper timer (PWM) module
    switch(eServoJoint) {
        case SERVO_FINGER_THUMB:
            UpdatePWM_DutyCycle(TIMERA3_BASE, TIMER_A, usMatchVal, usPrescaleVal);
            break;
        case SERVO_FINGER_INDEX:
            UpdatePWM_DutyCycle(TIMERA2_BASE, TIMER_B, usMatchVal, usPrescaleVal);
            break;
        case SERVO_FINGER_MIDDLE:
            UpdatePWM_DutyCycle(TIMERA3_BASE, TIMER_B, usMatchVal, usPrescaleVal);
            break;
        case SERVO_FINGER_RING:
            UpdatePWM_DutyCycle(TIMERA1_BASE, TIMER_A, usMatchVal, usPrescaleVal);
            break;
        case SERVO_FINGER_PINKY:
        	UpdatePWM_DutyCycle(TIMERA1_BASE, TIMER_B, usMatchVal, usPrescaleVal);
            break;
        case SERVO_WRIST:
        	UpdatePWM_DutyCycle(TIMERA0_BASE, TIMER_A, usMatchVal, usPrescaleVal);
            break;
        default:
            //UART_PRINT("[MoveServo] Invalid Finger input\n");
        	return;
    }
}

//****************************************************************************
//
// Moves the servo motor by degrees on the finger specified
//
// \param ucDegrees -> 0 to 100 for fingers, 0 to 180 for wrist
// \param eServoJoint -> Servo Joint type (ex: finger_thumb, finger_index, etc)
//
// \return None.
//
//****************************************************************************
void MoveServo_PWM_Breakout(unsigned char ucDegrees, enum Servo_Joint_Type eServoJoint)
{
	float pwm_duty_cycle;

    // Checks the input Degrees against the limits specified for the Servo of the joint
    CheckDegreeAgainstLimit(&ucDegrees, eServoJoint);

    // Converts the degrees specified to the appropriate duty cycle percentage
    pwm_duty_cycle = Convert_Degrees_To_DutyCycle_PWM_Breakout(ucDegrees, eServoJoint);

    // Updates the proper timer (PWM) module
    switch(eServoJoint) {
        case SERVO_FINGER_THUMB:
        	setPWM_DutyCycle(FINGER_THUMB_MAPPING, pwm_duty_cycle);
            break;
        case SERVO_FINGER_INDEX:
        	setPWM_DutyCycle(FINGER_INDEX_MAPPING, pwm_duty_cycle);
            break;
        case SERVO_FINGER_MIDDLE:
        	setPWM_DutyCycle(FINGER_MIDDLE_MAPPING, pwm_duty_cycle);
            break;
        case SERVO_FINGER_RING:
        	setPWM_DutyCycle(FINGER_RING_MAPPING, pwm_duty_cycle);
            break;
        case SERVO_FINGER_PINKY:
        	setPWM_DutyCycle(FINGER_PINKY_MAPPING, pwm_duty_cycle);
            break;
        case SERVO_WRIST:
        	setPWM_DutyCycle(FINGER_WRIST_MAPPING, pwm_duty_cycle);
            break;
        default:
            //UART_PRINT("[MoveServo] Invalid Finger input\n");
        	return;
    }
}
