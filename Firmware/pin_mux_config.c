//*****************************************************************************
//
// pin_mux_config.c
//
// Pin mux configuration
//
// This source file was generated using TI's pin mux online tool:
// https://dev.ti.com/pinmux/app.html#/config/CC3200/ID_500/ID_0/null/null
// Copyright notice is moved to the end of this file.
//
// Created:
// February 19, 2015
//
// Modified:
// February 19, 2015
//
//*****************************************************************************

#include "pin_mux_config.h" 
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"

//*****************************************************************************
void PinMuxConfig(void)
{
    //
    // Enable Peripheral Clocks 
    //
    PRCMPeripheralClkEnable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_TIMERA1, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_ADC, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_01 for TimerPWM6 GT_PWM06
    //
    //PinTypeTimer(PIN_01, PIN_MODE_3);

    //
    // Configure PIN_64 for TimerPWM5 GT_PWM05
    //
    //PinTypeTimer(PIN_64, PIN_MODE_3);

    //
    // Configure PIN_02 for TimerPWM7 GT_PWM07
    //
    //PinTypeTimer(PIN_02, PIN_MODE_3);

    //
    // Configure PIN_21 for TimerPWM2 GT_PWM02
    //
    //PinTypeTimer(PIN_21, PIN_MODE_9);

#ifndef DEBUG
    // Pin 17/19 are muxed with JTAG, must flash with Uniflash
    // Configure PIN_19 for TimerPWM3 GT_PWM03
    //
    //PinTypeTimer(PIN_19, PIN_MODE_8);

    //
    // Configure PIN_17 for TimerPWM0 GT_PWM00
    //
    //PinTypeTimer(PIN_17, PIN_MODE_5);
#endif

#ifndef DEBUG
    // Pin 57 is muxed with UART0 RX, so cannot use UART to debug
    // Configure PIN_57 for ADC0 ADC_CH0
    //
    PinTypeADC(PIN_57, PIN_MODE_255);
#endif

    //
    // Configure PIN_58 for ADC0 ADC_CH1
    //
    PinTypeADC(PIN_58, PIN_MODE_255);

    //
    // Configure PIN_59 for ADC0 ADC_CH2
    //
    PinTypeADC(PIN_59, PIN_MODE_255);

    //
    // Configure PIN_60 for ADC0 ADC_CH3
    //
    PinTypeADC(PIN_60, PIN_MODE_255);

#ifdef DEBUG
    //
    // Configure PIN_52 for UART0 UART0_RTS
    //
    PinTypeUART(PIN_52, PIN_MODE_6);

    //
    // Configure PIN_50 for UART0 UART0_CTS
    //
    PinTypeUART(PIN_50, PIN_MODE_12);

    //
    // Configure PIN_03 for UART0 UART0_TX
    //
    PinTypeUART(PIN_03, PIN_MODE_7);

    //
    // Configure PIN_04 for UART0 UART0_RX
    //
    PinTypeUART(PIN_04, PIN_MODE_7);
#endif

    //
    // Configure PIN_05 for I2C0 I2C_SCL
    //
    //PinTypeI2C(PIN_05, PIN_MODE_5);

    //
    // Configure PIN_06 for I2C0 I2C_SDA
    //
    //PinTypeI2C(PIN_06, PIN_MODE_5);

    //
    // Configure PIN_01 for I2C0 I2C_SCL
    //
    PinTypeI2C(PIN_01, PIN_MODE_1);

    //
    // Configure PIN_02 for I2C0 I2C_SDA
    //
    PinTypeI2C(PIN_02, PIN_MODE_1);
}


//*****************************************************************************
// pin_mux_config.c
//
// configure the device pins for different signals
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

// This file was automatically generated on 2/17/2016 at 8:35:46 PM
// by TI PinMux version 
//
//*****************************************************************************
