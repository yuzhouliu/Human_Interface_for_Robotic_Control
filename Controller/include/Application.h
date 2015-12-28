//*****************************************************************************
//
// Application.h
//
// Core class that contains the high level application logic.
//
// Created:
// December 20, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>

#include "IObserver.h"
#include "Window.h"

class Application : public IObserver
{
private:
    /* Fields */
    Window *_window;
    bool _exit;

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

    /* IObserver virtual methods */
    void onNotify(int event);
};

#endif /* _APPLICATION_H_ */
