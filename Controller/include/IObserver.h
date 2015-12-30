//*****************************************************************************
//
// IObserver.h
//
// Class responsible for providing interface for the Observer in the Observer
// pattern.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 27, 2015
//
// Modified:
// December 28, 2015
//
//*****************************************************************************
#ifndef _IOBSERVER_H_
#define _IOBSERVER_H_

class IObserver
{
private:
    /* Fields */

public:
    /* Destructor */
    virtual ~IObserver(){};

    /* Methods */
    virtual void onNotify(int event) = 0;
};

#endif /* _IOBSERVER_H_ */
