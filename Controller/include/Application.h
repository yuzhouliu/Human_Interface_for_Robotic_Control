//*****************************************************************************
//
// Application.h
//
// Class responsible for initializing components and integrating them together.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 20, 2015
//
// Modified:
// January 4, 2016
//
//*****************************************************************************
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>

class Application
{
private:
    /* Methods */
    bool _initialize();
    void _terminate();

public:
    /* Constructor */
    Application();

    /* Destructor */
    ~Application();

    /* Methods */
    int run();
};

#endif /* _APPLICATION_H_ */
