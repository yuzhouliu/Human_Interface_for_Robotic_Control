//*****************************************************************************
//
// pwm_break_out_if.c
//
// This implements the driver for PCA9685 16 x 12 bit PWM board
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// This source file uses code snippets from Texas Instruments Incorporated's
// CC3200-LAUNCHXL sample projects. Copyright notice is moved to the end of
// this file.
//
// Author: Minh Mai
//
// Created: February 16, 2016
//
// Modified: February 16, 2016
//
//*****************************************************************************
#include "pwm_break_out_if.h"
// Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
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
#define MAXSTEP                 4095
#define MILLISECONDS_TO_TICKS(ms)   (((SYS_CLK/PRESCALE -1)/1000) * (ms))
#define DUTYCYCLE_TO_STEPS(percent) (percent*MAXSTEP/100)

/* HELPER FUNCTION */
static uint8_t read(uint8_t regAddress);
static int write(uint8_t regAddress, uint8_t data);
static void delay(int time_ms);

//****************************************************************************
//
//! Reset the PWM module
//!
//! \param None
//!
//! \return None
//
//****************************************************************************
void PWM_reset(void) {
    write(PCA9685_MODE1, 0x0);
}
//****************************************************************************
//
//! Set operating frequency of the PWM module
//!
//! \param freq: in Hz, between 24Hz to 1526Hz
//!
//! \return void
//
//****************************************************************************
void setPWMFreq(float freq)
{
	freq = (freq * 9)/10; // Correct for overshoot in the frequency setting
    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    //print out prescale value if wanted
    #ifdef PWM_DEBUG
    UART_PRINT("Estimated pre-scale: "); 
    UART_PRINT("%4.2f\n\r",prescaleval);
    #endif
    uint8_t prescale = floor(prescaleval + 0.5);
    #ifdef PWM_DEBUG
    UART_PRINT("Final pre-scale: "); 
    UART_PRINT("%d\n\r",prescale);
    #endif
    PWM_reset();
    uint8_t oldmode = read(PCA9685_MODE1);
    uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
    write(PCA9685_MODE1, newmode); // go to sleep
    write(PCA9685_PRESCALE, prescale); // set the prescaler
    write(PCA9685_MODE1, oldmode);
    delay(5);
    write(PCA9685_MODE1, oldmode | 0xa1);  // This sets the MODE1 register 
                                            // to turn on auto increment.
    oldmode = read(PCA9685_MODE1);
}
//****************************************************************************
//
//! Set duty cycle for a specific PWM channel, channel from 0 to 15
//!
//! \param: 
//!         channel: decimal value from 0 to 15
//!         duty_cycle: duty cycle of the PWM(in perecentage): from 0 to 100
//!
//! \return 0: SUCCESS; >0 Failure
//
//****************************************************************************
int setPWM_DutyCycle(uint8_t channel, float duty_cycle)
{
    uint16_t step;
    //convert duty cycle to steps
    step = DUTYCYCLE_TO_STEPS(duty_cycle);
    // channel need to be from 0 to 15
    if (channel > 15) return FAILURE; 
    // check special case
    if (step == 4095) 
    {
    // Special value for signal fully on.
      setPWMOnOff(channel, 4096, 0);
    }
    else if (step == 0) {
    // Special value for signal fully off.
      setPWMOnOff(channel, 0, 4096);
    }
    else {
      setPWMOnOff(channel, 0, step);
    }
    return SUCCESS;
}
//****************************************************************************
//
//! This function sets the start (on) and end (off) of the high segment of the 
//! PWM pulse on a specific channel. You specify the 'tick' value between 
//! 0..4095 when the signal will turn on, and when it will turn off. Channel 
//! indicates which of the 16 PWM outputs should be updated with the new values.
//!
//! /param:
//! channel: The channel that should be updated with the new values (0..15)
//! on: The tick (between 0..4095) when the signal transition from low to high
//! off:the tick (between 0..4095) when the signal transition from high to low
//!
//! \return 0: SUCCESS; >0 Failure
//****************************************************************************
void setPWMOnOff(uint8_t channel, uint16_t on, uint16_t off)
{
    uint8_t LSB, MSB;
    uint8_t regLAddress,regHAddress;
    //Write to register ON
    LSB = on & 0xFF; // extract the 8 LSB of variable on
    MSB = on >> 8; // extract the 8 MSB of variable on
    regLAddress = LED0_ON_L+4*channel;//get the LOW register of the channel
    regHAddress = LED0_ON_H+4*channel;//get the HIGH register of the channel
    write(regLAddress, LSB); //write 8 LSB to LOW register
    write(regHAddress, MSB); //write 8 MSB to HIGH register
    //Write to register OFF
    LSB = off & 0xFF;
    MSB = off >> 8;
    regLAddress = LED0_OFF_L+4*channel;//get the L register of the channel
    regHAddress = LED0_OFF_H+4*channel;//get the H register of the channel
    write(regLAddress, LSB); //write 8 LSB to L register
    write(regHAddress, MSB); //write 8 MSB to H register
}
//****************************************************************************
//
//! Read an 8 bit data from a register on the PWM board
//!
//! \param  regAddress: register's address
//
//!  \return 8 bit data
//****************************************************************************
static uint8_t read(uint8_t regAddress) 
{
    int iRetVal;
    unsigned char DataBuf[1];
    //Construct the data buffer
    DataBuf[0] = (unsigned char) regAddress;
    //Send command to the PWM board
    iRetVal = I2C_IF_Write((unsigned char)PWM_ADDRESS, DataBuf, 1, 1);
    if(iRetVal == FAILURE)
    {
        #ifdef PWM_DEBUG
        UART_PRINT("I2C Read failed\n\r");
        #endif
        return 0; // TODO: figure out error code, use 0 as temp
    }
    //Read the value from the register
    iRetVal = I2C_IF_Read((unsigned char)PWM_ADDRESS, DataBuf, 1);
    if(iRetVal == SUCCESS)
    {
        #ifdef PWM_DEBUG
        //UART_PRINT("I2C Read complete\n\r");
        #endif 
        return ((uint8_t)DataBuf[0]);
    }
    else
    {
        #ifdef PWM_DEBUG
        UART_PRINT("I2C Read failed\n\r");
        #endif
        return 0;// TODO: figure out error code, use 0 as temp
    }
}
//****************************************************************************
//
//! Write an 8 bit data to a register on the PWM board
//!
//! \param  regAddress: the address of the register
//          data: the 8 bit data
//
//!  \return 0: SUCCESS; >0 Failure
//****************************************************************************
static int write(uint8_t regAddress, uint8_t data) 
{
    unsigned char DataBuf[2];
    int iRetVal;

    //Construct the Data buffer befor sending
    DataBuf[0] = (unsigned char)regAddress;
    DataBuf[1] = (unsigned char)data;

    // Write the data to the register on the PWM Board
    iRetVal = I2C_IF_Write((unsigned char)PWM_ADDRESS, DataBuf, 2, 1);

    if(iRetVal == SUCCESS)
    {
        #ifdef PWM_DEBUG
        //UART_PRINT("I2C Write complete\n\r");
        #endif
        return SUCCESS;
    }
    else
    {
        #ifdef PWM_DEBUG
        UART_PRINT("I2C Write failed\n\r");
        #endif
        return FAILURE;
    }
}
//****************************************************************************
//
//! The delay function uses timer to implement the delay time in milliseconds
//!
//! \param time in millisecond
//
//!  \return void
//****************************************************************************
static void delay(int time_ms)
{
    // Initialize Timer 0B as one-shot down counter.
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_TIMERA0);
    MAP_TimerConfigure(TIMERA0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_ONE_SHOT);
    MAP_TimerPrescaleSet(TIMERA0_BASE, TIMER_B, PRESCALE);
    //Load the value in milisecond
    MAP_TimerLoadSet(TIMERA0_BASE, TIMER_B, MILLISECONDS_TO_TICKS(time_ms));
    // Enable the timer
    MAP_TimerEnable(TIMERA0_BASE, TIMER_B);
    //Stall during debug
    MAP_TimerControlStall(TIMERA0_BASE, TIMER_B, 1);
    // Enable interrupt upon Time-out
    MAP_TimerIntEnable(TIMERA0_BASE, TIMER_TIMB_TIMEOUT);
    // Clear Interrupt Flag
    MAP_TimerIntClear(TIMERA0_BASE, MAP_TimerIntStatus(TIMERA0_BASE, true));
    //Wait until timer time-out
    while (MAP_TimerIntStatus(TIMERA0_BASE, true) != TIMER_TIMB_TIMEOUT){}
    //Disable the timer
    MAP_TimerDisable(TIMERA0_BASE, TIMER_B);
    //Disable Interrupt
    MAP_TimerIntDisable(TIMERA0_BASE, TIMER_TIMB_TIMEOUT);
    MAP_TimerIntUnregister(TIMERA0_BASE, TIMER_B);
}
//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
