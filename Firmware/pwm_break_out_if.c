//*****************************************************************************
// pwm_break_out_if.c
//
// This implements the driver for PCA9685 16 x 12 bit PWM board
//
// This source file uses code snippets from Texas Instruments Incorporated's
// CC3200-LAUNCHXL sample projects. Copyright notice is moved to the end of
// this file.
//
// Author: Minh Mai
//
// Created: February 16 2016
//
// Modified: February 16 2016
//
//*****************************************************************************
#include "pwm_break_out_if.h"
// Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_common_reg.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"
#include "hw_memmap.h"

// common interface includes
#include "udma_if.h"
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "i2c_if.h"

#include "timer.h"

#define FAILURE                 -1
#define SUCCESS                 0
#define SYS_CLK                 80000000
#define PRESCALE                255UL
#define MAXSTEP                 4096
#define MILLISECONDS_TO_TICKS(ms)   (((SYS_CLK/PRESCALE -1)/1000) * (ms))
#define DUTYCYCLE_TO_STEPS(percent) (percent*4096/100)

/* HELPER FUNCTION */
static int writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value);
static int16_t readRegister(uint8_t i2cAddress, uint8_t reg);
static void delay(int time_ms);




