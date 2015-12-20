
//*****************************************************************************
//
// servo_driver.c
//
// Low-level Servo driver for CC3200-LAUNCHXL
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
// December 20, 2015
//
//*****************************************************************************

//*****************************************************************************
// Includes
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

#include "servo_driver.h"

// Pinmux configuration include
#include "pin_mux_config.h"

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

    //TODO setup the rest of the PWM modules!!!

    // Enable the timers
    MAP_TimerEnable(TIMERA2_BASE,TIMER_B);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_A);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_B);
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
void UpdatePWM_Match(unsigned long ulBase, unsigned long ulTimer,
                     unsigned short usMatchVal, unsigned char ucPrescaleVal)
{
    MAP_TimerMatchSet(ulBase, ulTimer, usMatchVal);
    MAP_TimerPrescaleMatchSet(ulBase, ulTimer, ucPrescaleVal);
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
void Convert_Degrees_To_Match(unsigned short usDegrees, unsigned short *usMatchVal,
                              unsigned char *ucPrescaleVal)
{
    unsigned int uiDegreesToMatch;
    uiDegreesToMatch = PWM_0_DEGREES + usDegrees * PWM_MATCH_PER_DEGREE;

    *usMatchVal = uiDegreesToMatch & 0xFFFF;
    *ucPrescaleVal = (uiDegreesToMatch & 0xFF0000) >> 16;

    return;
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
