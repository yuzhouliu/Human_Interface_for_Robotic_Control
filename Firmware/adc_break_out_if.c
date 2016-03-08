//*****************************************************************************
//
// adc_break_out_if.c
//
// This implements the driver for Adafruit ADS1015 ADC breakout board
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
#include "adc_break_out_if.h"
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
#define PRESCALE				255UL
#define MILLISECONDS_TO_TICKS(ms)   (((SYS_CLK/PRESCALE -1)/1000) * (ms))

/* HELPER FUNCTION */
static int writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value);
static int16_t readRegister(uint8_t i2cAddress, uint8_t reg);
static void delay(int time_ms);

//****************************************************************************
//
//! Take the ADC reading from a channel specified in variable channel
//!
//! \param channel: decimal value from 0 to 3 
//!
//! \return 0 if failure, else the results from ADC reading
//
//****************************************************************************
unsigned short readADC_Breakout(unsigned int channel)
{
    if (channel > 3) return 0; // channel is from 0 to 3

    //Configurate default value
    uint16_t config =   ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                        ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                        ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                        ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                        ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                        ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    //Set voltage range to +- 6.144 V ()
    //config |= ADS1015_REG_CONFIG_PGA_6_144V;
    config |= ADS1015_REG_CONFIG_PGA_2_048V;

    // Set single-ended input channel
    switch (channel)
    {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(ADS1015_ADDRESS, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    delay(ADS1015_CONVERSIONDELAY);

    // Read the conversion results
    // Shift 12-bit results right 4 bits for the ADS1015
    return readRegister(ADS1015_ADDRESS, ADS1015_REG_POINTER_CONVERT) >> 4;  
}
//****************************************************************************
//**                       I2C CONNECTION HELPER FUNCTIONS                  **
//****************************************************************************
//****************************************************************************
//
//! Writes a 16-bits value to a specified register on the ADC board
//!
//! \param 
//          i2cAddress: specfies the i2c address of the ADC board
//          reg:    specifies the address of the register
//          value:  the value to write into the register
//
//!  \return 0: SUCCESS; >0 Failure
//****************************************************************************
static int writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value) 
{
    unsigned char DataBuf[3];
    int iRetVal;

    //Construct the Data buffer befor sending
    DataBuf[0] = (unsigned char)reg;
    DataBuf[1] = (unsigned char)(value>>8);
    DataBuf[2] = (unsigned char)(value & 0xFF);

    // Write the data to the specified address
    iRetVal = I2C_IF_Write((unsigned char)i2cAddress, DataBuf, 3, 1);

    if(iRetVal == SUCCESS)
    {
        #ifdef ADC_DEBUG
        //UART_PRINT("I2C Write complete\n\r");
        #endif
        return SUCCESS;
    }
    else
    {
        #ifdef ADC_DEBUG
        UART_PRINT("I2C Write failed\n\r");
        #endif
        return FAILURE;
    }
}
//****************************************************************************
//
//! Read a 16-bits value from a specified register on ADC board
//!
//! \param 
//          i2cAddress: specfies the i2c address of the ADC board
//          reg:    specifies the address of the register
//          value:  the value to write into the register
//
//!  \return 0: SUCCESS; >0 Failure
//****************************************************************************
static int16_t readRegister(uint8_t i2cAddress, uint8_t reg)
{
    unsigned char DataBuf[3];
    int iRetVal;
    //Construct the data buffer
    DataBuf[0] = (unsigned char) ADS1015_REG_POINTER_CONVERT;

    //Send command to the ADC board
    iRetVal = I2C_IF_Write((unsigned char)i2cAddress, DataBuf, 1, 1);
    if(iRetVal == FAILURE)
    {
        #ifdef ADC_DEBUG
        UART_PRINT("I2C Read failed\n\r");
        #endif
        return FAILURE;
    }
    //Read the value from the register
    iRetVal = I2C_IF_Read((unsigned char)i2cAddress, DataBuf, 2);
    if(iRetVal == SUCCESS)
    {
        #ifdef ADC_DEBUG
        //UART_PRINT("I2C Read complete\n\r");
        #endif 
        return (DataBuf[0] << 8 | DataBuf[1]);
    }
    else
    {
        #ifdef ADC_DEBUG
        UART_PRINT("I2C Read failed\n\r");
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
