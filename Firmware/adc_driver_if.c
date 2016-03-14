//*****************************************************************************
//
// adc_driver_if.c
//
// High-level ADC driver for CC3200-LAUNCHXL
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// This source file uses code snippets from Texas Instruments Incorporated's
// CC3200-LAUNCHXL sample projects. Copyright notice is moved to the end of
// this file.
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


// Standard includes
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Driverlib includes
#include "utils.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_adc.h"
#include "hw_ints.h"
#include "hw_gprcm.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "uart.h"
#include "pin.h"
#include "adc.h"

#include "pin_mux_config.h"
#include "adc_driver_if.h"
#include "adc_break_out_if.h"
#include "uart_if.h"

#define FOREVER 1

//****************************************************************************
// Initializes the Sensor ADCs for operation
//
// \param None.
//
// \return None.
//****************************************************************************
void InitSensorADC(void)
{
    // Enable ADC module
    MAP_ADCEnable(ADC_BASE);

    // Enable ADC channel
#ifndef DEBUG
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_0);    // NOTE: Cannot enabled concurrently with UART0 RX
#endif
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_1);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_2);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_3);
}

//****************************************************************************
// Disables the Sensor ADCs
//
// \param None.
//
// \return None.
//****************************************************************************
void DisableSensorADC(void)
{
    MAP_ADCChannelDisable(ADC_BASE, ADC_CH_0);
    MAP_ADCChannelDisable(ADC_BASE, ADC_CH_1);
    MAP_ADCChannelDisable(ADC_BASE, ADC_CH_2);
    MAP_ADCChannelDisable(ADC_BASE, ADC_CH_3);
}

//*****************************************************************************
// Gets the Sensor Reading from Finger-Tip sensors using ADC from CC3200
//
// \param eFinger -> finger type (ex: FINGER_THUMB, FINGER_INDEX, etc)
//
// \return Raw ADC value between 0 to 4096 (0 to 1.4V)
//*****************************************************************************
unsigned short GetSensorReading_CC3200(enum Fingertip_Sensor_Type eFingerSensor)
{
    unsigned int uiChannel;
    unsigned char ucCount;
    unsigned long ulSampleTotal;

#ifdef CC3200_ES_1_2_1
    // Enable ADC clocks.###IMPORTANT###Need to be removed for PG 1.32
    HWREG(GPRCM_BASE + GPRCM_O_ADC_CLK_CONFIG) = 0x00000043;
    HWREG(ADC_BASE + ADC_O_ADC_CTRL) = 0x00000004;
    HWREG(ADC_BASE + ADC_O_ADC_SPARE0) = 0x00000100;
    HWREG(ADC_BASE + ADC_O_ADC_SPARE1) = 0x0355AA00;
#endif

    // Convert pin number to channel number
    switch(eFingerSensor)
    {
        case SENSOR_FINGER_INDEX:
            uiChannel = ADC_CH_0;    // Pin_57
            break;
        case SENSOR_FINGER_THUMB:
            uiChannel = ADC_CH_1;    // Pin_58
            break;
        case SENSOR_FINGER_MIDDLE:
            uiChannel = ADC_CH_2;    // Pin_59
            break;
        case SENSOR_FINGER_RING:
            uiChannel = ADC_CH_3;    // Pin_60
            break;
        default:
            return (unsigned short) 0;
    }

    // Initialize Counter and Sum of Samples to 0
    ucCount = 0;
    ulSampleTotal = 0;

    while(ucCount < NO_OF_SAMPLES)
    {
        if(MAP_ADCFIFOLvlGet(ADC_BASE, uiChannel))
        {
            ulSampleTotal += (MAP_ADCFIFORead(ADC_BASE, uiChannel) >> 2) & 0x0FFF;
            ucCount++;
        }
    }

    return (unsigned short) ulSampleTotal/NO_OF_SAMPLES;
}

//*****************************************************************************
// Gets the Sensor Reading from Finger-Tip sensors using ADC
// Either from Breakout ADC board or CC3200 ADC
//
// \param eFinger -> finger type (ex: FINGER_THUMB, FINGER_INDEX, etc)
//
// \return Raw ADC value between 0 to 4096
//*****************************************************************************
unsigned short GetSensorReading(enum Fingertip_Sensor_Type eFingerSensor)
{
	switch(eFingerSensor)
	{
		case SENSOR_FINGER_THUMB:
            return readADC_Breakout(FINGER_THUMB_BREAKOUT);			
		case SENSOR_FINGER_INDEX:
			return GetSensorReading_CC3200(SENSOR_FINGER_INDEX);
		case SENSOR_FINGER_MIDDLE:
			return readADC_Breakout(FINGER_MIDDLE_BREAKOUT);
		case SENSOR_FINGER_RING:
			return readADC_Breakout(FINGER_RING_BREAKOUT);
		case SENSOR_FINGER_PINKY:
			return readADC_Breakout(FINGER_PINKY_BREAKOUT);
		default:
			return (unsigned short) 0;
	}
}

//*****************************************************************************
// Converts raw ADC value (0 to 4096) to the associated voltage (0 to 1.4V)
//
// \param input_raw_adc -> raw ADC value from 0 to 4096
//
// \return A float value ranging from 0 - 1.4V
//*****************************************************************************
float ConvertADCtoVolts(unsigned short input_raw_adc)
{
	return (float) (input_raw_adc*1.4)/4096;
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

//*****************************************************************************
//
// Application Name     - ADC
// Application Overview - The application is a reference to usage of ADC DriverLib 
//                        functions on CC3200. Developers/Users can refer to this 
//                        simple application and re-use the functions in 
//                        their applications.
//
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_ADC
// or
// docs\examples\CC32xx_ADC.pdf
//
//*****************************************************************************
