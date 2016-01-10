
//*****************************************************************************
//
// servo_driver.c
//
// Low-level Servo driver for CC3200-LAUNCHXL
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
// December 20, 2015
//
// Modified:
// January 4, 2016
//
//*****************************************************************************

//*****************************************************************************
// The PWM works based on the following settings:
//     Timer reload interval -> determines the time period of one cycle
//     Timer match value -> determines the duty cycle
// The computation for PWM is as described below:
// System frequency = 80 Mhz.
// For a time period of 20 ms (Servo), 
//    Timer reload value = 80,000,000/(1/20ms) = 1,600,000 cycles
//    Cannot store this in 16-bit reload register, so must use prescaler
//    1,600,000 = 0x186A00. Store lower 16 bits into Load, higher 8 bits into prescale
//    PRESCALE = 0x18, RELOAD = 0x6A00
// Servos rotate using duty cycle, 0.5ms for 0 degrees, 2.5ms for 180 degrees.
//    Timer Match for 0.5ms: 80,000,000/(1/0.5ms) = 40,000 cycles
//      40,000 = 0x9C40, MatchPrescale = 0x0, MatchReload = 0x9C40
//    Timer Match for 2.5ms: 80,000m000/(1/2.5ms) = 200,000 cycles
//      200,000 = 0x30D40, MatchPrescale = 0x3, MatchReload = 0x0D40
//
//*****************************************************************************

// Standard includes
#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "rom.h"
#include "rom_map.h"
#include "timer.h"
#include "utils.h"
#include "prcm.h"

#include "pin_mux_config.h"
#include "servo_driver.h"


//*****************************************************************************
// Sets up the Timer for PWM mode
// \param ulBase is the base address of the timer to be configured
// \param ulTimer is the timer to be setup (TIMER_A or  TIMER_B)
// \param ulConfig is the timer configuration setting
// \param ucInvert is to select the inversion of the output
//
// \return None.
//*****************************************************************************
void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert)
{
    // Set GPT - Configured Timer in PWM mode.
    MAP_TimerConfigure(ulBase,ulConfig);

    // Inverting the timer output if required
    MAP_TimerControlLevel(ulBase,ulTimer,ucInvert);

    // Set the prescaler and load to obtain 20ms
    MAP_TimerPrescaleSet(ulBase,ulTimer,PWM_PRESCALE);
    MAP_TimerLoadSet(ulBase,ulTimer,PWM_INTERVAL_RELOAD);

    // Match value set so as to output level 0
    MAP_TimerPrescaleMatchSet(ulBase,ulTimer,PWM_PRESCALE);
    MAP_TimerMatchSet(ulBase,ulTimer,PWM_INTERVAL_RELOAD);
}

//*****************************************************************************
//
// Sets up the identified timers as PWM to drive the finger servos
//
// \param none
//
// This function sets up the folowing
//    1. TIMERA2 (TIMER B) as Index finger (also RED of RGB light)
//    2. TIMERA3 (TIMER A) as Thumb (also YELLOW of RGB light)
//    3. TIMERA3 (TIMER B) as Middle finger (also GREEN of RGB light)
//
// \return None.
//*****************************************************************************
void InitPWMModules()
{
    // TIMERA2 (TIMER B), Pin 64 as Index finger
    // Also as RED of RGB light. GPIO 9 --> PWM_5
    SetupTimerPWMMode(TIMERA2_BASE, TIMER_B,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM), 1);

    // TIMERA3 (TIMER B), Pin 01 as Thumb
    // Also as YELLOW of RGB light. GPIO 10 --> PWM_6
    SetupTimerPWMMode(TIMERA3_BASE, TIMER_A,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);

    // TIMERA3 (TIMER A), Pin 02 as Middle finger
    // Also as GREEN of RGB light. GPIO 11 --> PWM_7
    SetupTimerPWMMode(TIMERA3_BASE, TIMER_B,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);

    // TIMERA1 (TIMER A), Pin 21 as Ring
    // SOP2 --> PWM_2
    SetupTimerPWMMode(TIMERA1_BASE, TIMER_A,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);

    // NOTE: Enabling the following two will diable JTAG (debug).
    //  	 Must use Uniflash instead
    // TIMERA1 (TIMER B), Pin 19 as Pinky
    // TCK --> PWM_3
    SetupTimerPWMMode(TIMERA1_BASE, TIMER_B,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);

    // TIMERA0 (TIMER B), Pin 17 as Wrist
    // TDO --> PWM_0
    SetupTimerPWMMode(TIMERA0_BASE, TIMER_A,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);

    // Enable the timers
    MAP_TimerEnable(TIMERA2_BASE,TIMER_B);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_A);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_B);
    MAP_TimerEnable(TIMERA1_BASE,TIMER_A);
    MAP_TimerEnable(TIMERA1_BASE,TIMER_B);
    MAP_TimerEnable(TIMERA0_BASE,TIMER_A);
}

//****************************************************************************
//
// Disables the timer PWMs
//
// \param none
//
// \return None.
//
//****************************************************************************
void DeInitPWMModules()
{
    // Disable the peripherals
    MAP_TimerDisable(TIMERA2_BASE, TIMER_B);
    MAP_TimerDisable(TIMERA3_BASE, TIMER_A);
    MAP_TimerDisable(TIMERA3_BASE, TIMER_B);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
}

//****************************************************************************
//
// Updates the TimerMatch and PrescaleMatch for specified Timer PWM
//
// \param ulBase is the base address of the timer to be configured
// \param ulTimer is the timer to be setup (TIMER_A or  TIMER_B)
//
// \return None.
//
//****************************************************************************
void UpdatePWM_DutyCycle(unsigned long ulBase, unsigned long ulTimer,
                         unsigned short usMatchVal, unsigned char ucPrescaleVal)
{
    MAP_TimerMatchSet(ulBase, ulTimer, usMatchVal);
    MAP_TimerPrescaleMatchSet(ulBase, ulTimer, ucPrescaleVal);
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
// Application Name     - PWM Application
// Application Overview - The general purpose timers (GPTs) support a 16-bit 
//                        pulse-width modulation (PWM) mode with software-
//                        programmable output inversion of the PWM signal.
//                        The brightness of the LEDs are varied from Off to On 
//                        as PWM output varies.
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_PWM
// or
// docs\examples\CC32xx_PWM.pdf
//
//*****************************************************************************
