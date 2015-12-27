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

class Application
{
private:
    /* Fields */

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
