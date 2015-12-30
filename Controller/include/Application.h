//*****************************************************************************
//
// Application.h
//
// Core class that contains the high level application logic.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 20, 2015
//
// Modified:
// December 30, 2015
//
//*****************************************************************************
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>

#include "Hand.h"   /* FingerPressureStruct */
#include "IObserver.h"

class Application : public IObserver
{
private:
    /* Fields */
    bool _exit;

    /* Methods */
    bool _initialize();
    void _terminate();
    bool _populateFingerPressureStruct(FingerPressureStruct &fingerPressures,
        unsigned char *buf, unsigned int buflen);

public:
    /* Constructor */
    Application();

    /* Destructor */
    ~Application();

    /* Methods */
    int run();

    /* IObserver virtual methods */
    void onNotify(int event);
};

#endif /* _APPLICATION_H_ */
