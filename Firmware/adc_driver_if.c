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
// December 28, 2015
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
#include "pinmux.h"
#include "pin.h"
#include "adc.h"

#include "adc_driver_if.h"
#include "uart_if.h"

#define FOREVER            1
#define NO_OF_SAMPLES       1

unsigned long pulAdcSamples[4096];

//*****************************************************************************
//
//
// \return None.
//*****************************************************************************
void GetADC(enum Finger_Type eFinger)
{
    unsigned int  uiChannel;
    unsigned int  uiIndex=0;
    unsigned long ulSample;

    //
    // Initialize Array index for multiple execution
    //
    uiIndex=0;

#ifdef CC3200_ES_1_2_1
    //
    // Enable ADC clocks.###IMPORTANT###Need to be removed for PG 1.32
    //
    HWREG(GPRCM_BASE + GPRCM_O_ADC_CLK_CONFIG) = 0x00000043;
    HWREG(ADC_BASE + ADC_O_ADC_CTRL) = 0x00000004;
    HWREG(ADC_BASE + ADC_O_ADC_SPARE0) = 0x00000100;
    HWREG(ADC_BASE + ADC_O_ADC_SPARE1) = 0x0355AA00;
#endif
    //
    // Pinmux for the selected ADC input pin
    //
    MAP_PinTypeADC(uiAdcInputPin,PIN_MODE_255);

    //
    // Convert pin number to channel number
    //
    switch(uiAdcInputPin)
    {
        case PIN_58:
            uiChannel = ADC_CH_1;
            break;
        case PIN_59:
            uiChannel = ADC_CH_2;
            break;
        case PIN_60:
            uiChannel = ADC_CH_3;
            break;
        default:
            break;
    }

    //
    // Configure ADC timer which is used to timestamp the ADC data samples
    //
    //MAP_ADCTimerConfig(ADC_BASE,2^17);

    //
    // Enable ADC timer which is used to timestamp the ADC data samples
    //
    //MAP_ADCTimerEnable(ADC_BASE);

    //
    // Enable ADC module
    //
    MAP_ADCEnable(ADC_BASE);

    //
    // Enable ADC channel
    //
    MAP_ADCChannelEnable(ADC_BASE, uiChannel);

    while(uiIndex < NO_OF_SAMPLES + 4)
    {
        if(MAP_ADCFIFOLvlGet(ADC_BASE, uiChannel))
        {
            ulSample = MAP_ADCFIFORead(ADC_BASE, uiChannel);
            pulAdcSamples[uiIndex++] = ulSample;
        }


    }

    MAP_ADCChannelDisable(ADC_BASE, uiChannel);

    uiIndex = 0;

    //
    // Print out ADC samples
    //
    while(uiIndex < NO_OF_SAMPLES)
    {
        UART_PRINT("\n\rVoltage is %f\n\r",(((float)((pulAdcSamples[4+uiIndex] >> 2 ) & 0x0FFF))*1.4)/4096);
        uiIndex++;
    }

    UART_PRINT("\n\r");

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
