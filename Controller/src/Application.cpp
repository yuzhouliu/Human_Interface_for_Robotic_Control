//*****************************************************************************
//
// Application.cpp
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
#include "Application.h"

#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "FrameRateManager.h"

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

    //
    // Main program logic
    //
    while (!_exit)
    {
        //
        // Begins tracking fps
        //
        fpsManager.beginFrame();

        // TODO (Brandon): Main program logic
        _window->update();

        //
        // Ends frame and blocks until FPS elapses
        //
        fpsManager.endFrame();
    }

    return EXIT_SUCCESS;
}

//*****************************************************************************
//
//! Sets the exit flag is SDL_QUIT event is raised.
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
    // Initialize SDL_image
    //
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cerr << "[ERROR] Application::_initialize(): IMG_Init() failed. "\
            "SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    //
    // Consructs a window to render images on
    //
    _window = new Window();

    //
    // Register this object to be notified by window
    //
    _window->addObserver(this);

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
    if (_window != NULL)
    {
        delete _window;
    }

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

