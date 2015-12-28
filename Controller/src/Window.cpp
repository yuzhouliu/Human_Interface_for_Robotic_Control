//*****************************************************************************
//
// Window.cpp
//
// Class responsible for spawning, managing, and closing the OpenGL context.
//
// Created:
// December 27, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#include "Window.h"

#include <iostream>

#include <SDL.h>

//*****************************************************************************
//
//! Constructor for Window. Acquires resources for window and renderer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Window::Window()
    : _window(NULL), _renderer(NULL), _width(_DEFAULT_WIDTH),
      _height(_DEFAULT_HEIGHT)
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
//! Destructor for Window. Releases resources used by window and renderer.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Window::~Window()
{
    _terminate();
}

//*****************************************************************************
//
//! Updates the window. Called once every frame.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Window::update()
{
    _processInput();
    _update();
    _render();
}

//*****************************************************************************
//
//! Initializes the window.
//!
//! \param None.
//!
//! \return Returns \b true if the window was initialized successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Window::_initialize()
{
    int iRetVal = 0;

    //
    // Creates OpenGL context
    //
    _window = SDL_CreateWindow("HIRC Application", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_SHOWN);
    if (_window == NULL)
    {
        std::cerr << "[ERROR] Window::_initialize(): SDL_CreateWindow() "\
            "failed. SDL Error " << SDL_GetError() << std::endl;
        return false;
    }

    //
    // Creates renderer
    //
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_TARGETTEXTURE);
    if (_renderer == NULL)
    {
        std::cerr << "[ERROR] Window::_initialize(): SDL_CreateRenderer() "\
            "failed. SDL Error " << SDL_GetError() << std::endl;
        return false;
    }

    //
    // Sets renderer to default to an Opqaue White color on screen clear
    //
    iRetVal = SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF,
        SDL_ALPHA_OPAQUE);
    if (iRetVal < 0)
    {
        std::cerr << "[ERROR] Window::_initialize(): SDL_SetRenderDrawColor()"\
            " failed. SDL Error " << SDL_GetError() << std::endl;
        return false;
    }

    //
    // Sets a device independent resolution for rendering
    //
    iRetVal = SDL_RenderSetLogicalSize(_renderer, _width, _height);
    if (iRetVal < 0)
    {
        std::cerr << "[ERROR] Window::_initialize(): "\
            "SDL_RenderSetLogicalSize() failed. SDL Error " << SDL_GetError()
            << std::endl;
        return false;
    }

    return true;
}

//*****************************************************************************
//
//! Terminates the window.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Window::_terminate()
{
    if (_renderer != NULL)
    {
        SDL_DestroyRenderer(_renderer);
    }

    if (_window != NULL)
    {
        SDL_DestroyWindow(_window);
    }
}

//*****************************************************************************
//
//! Processes user input. Called once per frame by update().
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Window::_processInput()
{
    SDL_Event event;

    //
    // Polls the event queue for pending events
    //
    while (SDL_PollEvent(&event))
    {
        //
        // Lets the Application class know when the user wants to quit
        //
        if (event.type == SDL_QUIT)
        {
            notify(SDL_QUIT);
        }
    }
}

//*****************************************************************************
//
//! Updates program logic. Called once per frame by update().
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Window::_update()
{
    // TODO (Brandon): Implement
}

//*****************************************************************************
//
//! Renders frame. Called once per frame by update().
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Window::_render()
{
    //
    // Clears screen
    //
    SDL_RenderClear(_renderer);

    //
    // Renders all textures to screen
    //
    // TODO (Brandon): Implement

    //
    // Updates screen (swaps screen buffers)
    //
    SDL_RenderPresent(_renderer);
}
