#ifndef __SERVO_DRIVER_H__
#define __SERVO_DRIVER_H__

//*****************************************************************************
//
// servo_driver.h
//
// Header file for Servo driver for CC3200-LAUNCHXL as part of the robotic hand project
//
// Created:
// December 20, 2015
//
// Modified:
// December 20, 2015
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

// PWM register defines
#define PWM_PRESCALE 0x18
#define PWM_INTERVAL_RELOAD 0x6A00
#define PWM_0_DEGREES 0x9C40UL          // Actual may be 0xC000
#define PWM_180_DEGREES 0x30D54UL       // Actual may be 0x31400
#define PWM_MATCH_PER_DEGREE (0x30D54UL - 0x9C40UL)/180  // Equates to 0x379 or 889

//*****************************************************************************
// Sets up the Timer for PWM mode
//*****************************************************************************
void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert);

//*****************************************************************************
// Sets up the identified timers as PWM to drive the peripherals
//*****************************************************************************
void InitPWMModules();

//*****************************************************************************
// Disables Timer PWMs
//*****************************************************************************
void DeInitPWMModules();

//*****************************************************************************
// Updates the MatchSet value and PrescaleMatchSet value of the timer specified
//*****************************************************************************
void UpdatePWM_Match(unsigned long ulBase, unsigned long ulTimer,
                     uint16_t matchVal, uint8_t prescaleVal);

//*****************************************************************************
// Converts the input of degrees to a Match value and Prescale value
//*****************************************************************************
void Convert_Degrees_To_Match(unsigned short usDegrees, uint16_t *matchVal, 
                              uint8_t *prescaleVal);

#endif //  __SERVO_DRIVER_H__