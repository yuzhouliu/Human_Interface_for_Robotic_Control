//*****************************************************************************
//
// Application.cpp
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
#include "Application.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "FrameRateManager.h"
#include "IPv4Address.h"
#include "LeapMotionManager.h"
#include "Network.h"
#include "TCPSocket.h"
#include "Window.h"

//*****************************************************************************
//
//! Constructor for Image. Acquires resources for SDL and window.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Application::Application()
    : _exit(false)
{
    //
    // Initialize application
    //
    if (!_initialize())
    {
        std::cerr << "[ERROR] Application::Application(): Failed to "\
            "initialize." << std::endl;
        return;
    }
}

//*****************************************************************************
//
//! Destructor for Application. Releases resources used by SDL and window.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Application::~Application()
{
    _terminate();
}

//*****************************************************************************
//
//! Runs the application.
//!
//! \param None.
//!
//! \return Returns \b true if the application was run successfully and \b
//! false otherwise.
//
//*****************************************************************************
int Application::run()
{
    FrameRateManager fpsManager;
    LeapMotionManager leap;
    const unsigned short _MAX_PAYLOAD = 256;
    unsigned char data[_MAX_PAYLOAD];
    FingerPressureStruct fingerPressures;

    //
    // Receive IPv4 address and port as input from user
    //
    /*std::cout << "Enter the target IPv4 address: ";
    char addressInput[16];
    std::cin >> addressInput;

    std::cout << "Enter the target port: ";
    unsigned short port;
    std::cin >> port;

    //
    // Constructs an IPv4 address using user input
    //
    IPv4Address address(addressInput, port);

    //
    // Constructs a TCP socket and connects to IPv4 address
    //
    TCPSocket tcpsocket;
    tcpsocket.open();
    tcpsocket.connect(&address);*/

    //
    // Create window and register this object to be notified by window
    //
    Window window;
    window.addObserver(this);

    //
    // Main program logic
    //
    while (!_exit)
    {
        //
        // Begins tracking fps
        //
        fpsManager.beginFrame();

        //
        // Fetches relevent data from Leap Motion Controller
        //
        leap.processFrame(data, _MAX_PAYLOAD);

        //
        // Send data to remote host
        //
        /*tcpsocket.send(data, _MAX_PAYLOAD);*/

        //
        // Populates FingerPressureStruct with finger pressure information
        // TODO (Brandon): As of now, this populates structure with angle
        // information. Must change to pressure information once we establish
        // communication to microcontroller and receive feedback from sensors.
        //
        _populateFingerPressureStruct(fingerPressures, data, _MAX_PAYLOAD);

        //
        // Updates GUI
        //
        window.update(&fingerPressures);

        //
        // Ends frame and blocks until FPS elapses
        //
        fpsManager.endFrame();
    }

    return EXIT_SUCCESS;
}

//*****************************************************************************
//
//! Sets the exit flag if SDL_QUIT event is raised.
//!
//! \param event that was raised.
//!
//! \return None.
//
//*****************************************************************************
void Application::onNotify(int event)
{
    if (event == SDL_QUIT)
    {
        _exit = true;
    }
}

//*****************************************************************************
//
//! Initializes the application.
//!
//! \param None.
//!
//! \return Returns \b true if the application was initialized successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Application::_initialize()
{
    //
    // Initializes SDL
    //
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "[ERROR] Application::_initialize(): SDL_Init() failed. "\
            "SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    //
    // Initializes SDL_image
    //
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cerr << "[ERROR] Application::_initialize(): IMG_Init() failed. "\
            "SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    //
    // Initializes socket API
    //
    if (!network::initialize())
    {
        std::cerr << "[ERROR] Application::_initialize(): "\
            "network::initialize() failed." << std::endl;
        return false;
    }

    return true;
}

//*****************************************************************************
//
//! Terminates the application.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Application::_terminate()
{
    network::terminate();

    IMG_Quit();
    SDL_Quit();
}

//*****************************************************************************
//
//! Populates structure with pressure information decoded from buf.
//!
//! \param fingerPressures structure for storing results.
//! \param buf buffer with encoded pressure data.
//! \param buflen the size of the buffer.
//!
//! \return Returns \b true if the structure was populated successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Application::_populateFingerPressureStruct(FingerPressureStruct
    &fingerPressures, unsigned char *buf, unsigned int buflen)
{
    //
    // Buffer needs to be at least this size
    //
    const int BITS_PER_BYTE = 8;
    int pressureSize = sizeof(fingerPressures.pressure[0]);
    assert(buflen >= pressureSize*NUM_FINGERS);

    //
    // Parses through buf and populate structure
    //
    unsigned int bufIndex = 0;
    for (int i=0; i<NUM_FINGERS; i++)
    {
        fingerPressures.pressure[i] = buf[bufIndex++];
    }

    return true;
}

//*****************************************************************************
//
//! Creates and runs the application.
//!
//! \param None.
//!
//! \return Returns 0 on success, and a non-zero value otherwise.
//
//*****************************************************************************
int main(int argc, char *argv[])
{
    Application app;

    return app.run();
}

