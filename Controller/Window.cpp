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
    : _window(NULL), _renderer(NULL), _width(0), _height(0)
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
    // TODO (Brandon): Implement
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
    // TODO (Brandon): Implement
    return false;
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
    // TODO (Brandon): Implement
}
