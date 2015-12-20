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
// December 20, 2015
//
//*****************************************************************************
#include "Application.h"

#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

Application::Application()
{

}

//*****************************************************************************
//
//! Runs the application.
//!
//! \param None.
//!
//! \return Returns \b true if the application was run successfully and \b
//! otherwise.
//
//*****************************************************************************
int Application::run()
{
    if (!initialize())
    {
        return EXIT_FAILURE;
    }

    while (true)
    {
        // TODO (Brandon): Main program logic
    }

    terminate();

    return EXIT_SUCCESS;
}

//*****************************************************************************
//
//! Initializes the application.
//!
//! \param None.
//!
//! \return Returns \b true if the application was initialized successfully and
//! \b otherwise.
//
//*****************************************************************************
bool Application::initialize()
{
    //
    // Initializes SDL
    //
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "SDL_Init() failed. SDL Error: " << SDL_GetError()
            << std::endl;
        return false;
    }

    //
    // Initialize SDL_image
    //
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cerr << "IMG_Init() failed. SDL_image Error: " << IMG_GetError()
            << std::endl;
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
void Application::terminate()
{
    IMG_Quit();
    SDL_Quit();
}

//*****************************************************************************
//
//! Creates and runs the application.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
int main(int argc, char *argv[])
{
    Application app;

    return app.run();
}
