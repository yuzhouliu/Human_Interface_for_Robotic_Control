//*****************************************************************************
// pwm_break_out_if.h
//
// This is the driver library for Adafruit PCA9685 16 x 12 bit PWM board
//
// Author: Minh Mai
//
// Created: February 16 2016
//
// Modified: February 16 2016
//
//*****************************************************************************
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
//reset the PWM module
void reset(void);
//set the Frequency for the PWM
void setPWMFreq(float freq);
//set duty cycle for a specific PWM channel, channel from 0 to 15
void setPWM_DutyCycle(uint8_t channel, uint16_t duty_cycle);
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
