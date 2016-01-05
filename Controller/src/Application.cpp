//*****************************************************************************
//
// Application.cpp
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
#include "Application.h"

#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "Network.h"
#include "Window.h"

//*****************************************************************************
//
//! Constructor for Application. Acquires resources for SDL and window.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Application::Application()
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
    //
    // Creates and starts window
    //
    Window window;

    return window.run();
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

