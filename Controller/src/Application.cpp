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
// December 28, 2015
//
//*****************************************************************************
#include "Application.h"

#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "FrameRateManager.h"
#include "LeapMotionManager.h"
#include "Network.h"
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
    Window window;
    const unsigned short _MAX_PAYLOAD = 256;
    char data[_MAX_PAYLOAD];

    //
    // Register this object to be notified by window
    //
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
        // TODO (Brandon): Send data to remote host
        //


        //
        // Updates GUI
        //
        window.update();

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

