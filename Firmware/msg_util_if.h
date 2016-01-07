#ifndef __MSG_UTIL_IF_H__
#define __MSG_UTIL_IF_H__

//*****************************************************************************
//
// msg_util_if.h
//
// Header file for functions converting various types to proper message buffer format
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Project: Human Interface for Robotic Control
//
// Created:
// January 6, 2015
//
// Modified:
// January 6, 2015
//
//*****************************************************************************

//****************************************************************************
// Converts 16-bit unsigned short to 2 8-bit unsigned chars
//****************************************************************************
void UnsignedShort_to_UnsignedChar(unsigned short input_16_bits, unsigned char *highByte, unsigned char *lowByte);

#endif //  __MSG_UTIL_IF_H__
