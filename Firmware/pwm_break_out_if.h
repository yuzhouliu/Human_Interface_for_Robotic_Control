//*****************************************************************************
//
// pwm_break_out_if.h
//
// This is the driver library for Adafruit PCA9685 16 x 12 bit PWM board
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Author: Minh Mai
//
// Created: February 16, 2016
//
// Modified: February 16, 2016
//
//*****************************************************************************

#ifndef __PWM_BREAK_OUT_IF_H__
#define __PWM_BREAK_OUT_IF_H__

// Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"
//******************************************************************************
//                      Interface Functions
//******************************************************************************
//reset the PWM module (optional)
void PWM_reset(void);
//set the Frequency for the PWM
void setPWMFreq(float freq);
//set duty cycle for a specific PWM channel, channel from 0 to 15
int setPWM_DutyCycle(uint8_t channel, float duty_cycle);
//set the start (on) and end (off) of the high segment of the PWM pulse
void setPWMOnOff(uint8_t channel, uint16_t on, uint16_t off);

//******************************************************************************
//                      EXAMPLE
//******************************************************************************
// The working flow for the PWM could be:
// First, You need to set up the operating frequency for the PWM module
// The set up frequency only need to be done one at the beginning of the code
// setPWMFreq(50);//set the frequency for the PWM to 50 Hz
// After set up the frequency, you can set the duty cycle for the channels
// setPWM_DutyCycle(0,50); //set the duty cycle for channel 0 to 50%
//******************************************************************************
//                          Constants
//******************************************************************************
#define PWM_DEBUG                       (1)
#define PWM_ADDRESS                     (0x40)

#define PCA9685_MODE1                   (0x0)
#define PCA9685_PRESCALE                (0xFE)

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

#define SLEEPMODE   0x31
#define WORKMODE    0x21

#endif // __PWM_BREAK_OUT_IF_H__
