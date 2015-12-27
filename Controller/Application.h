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
// December 20, 2015
//
//*****************************************************************************
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

class Application
{
private:
    /* Fields */

public:
    /* Constructor */
    Application();

    /* Destructor */
    ~Application();

    /* Methods */
    int run();
    bool initialize();
    void terminate();
};

#endif /* _APPLICATION_H_ */
