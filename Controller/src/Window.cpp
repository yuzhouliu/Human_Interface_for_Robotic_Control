//*****************************************************************************
//
// Window.cpp
//
// Class responsible for spawning, managing, and closing the OpenGL context.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 27, 2015
//
// Modified:
// December 28, 2015
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
    // Initialize window
    //
    if (!_initialize())
    {
        std::cerr << "[ERROR] Window::Window(): Failed to initialize." <<
            std::endl;
        return;
    }

    //
    // Creates base hand image
    //
    _baseImage = std::unique_ptr<Image>(new Image(_renderer, "data/gfx/hand_base.png"));

    //
    // Creates and add images to finger image list
    //
    _fingerImageList.push_back(std::unique_ptr<Image>(new Image(_renderer, "data/gfx/hand_right_thumb.png")));
    _fingerImageList.push_back(std::unique_ptr<Image>(new Image(_renderer, "data/gfx/hand_right_index.png")));
    _fingerImageList.push_back(std::unique_ptr<Image>(new Image(_renderer, "data/gfx/hand_right_middle.png")));
    _fingerImageList.push_back(std::unique_ptr<Image>(new Image(_renderer, "data/gfx/hand_right_ring.png")));
    _fingerImageList.push_back(std::unique_ptr<Image>(new Image(_renderer, "data/gfx/hand_right_pinky.png")));

    //
    // Centre all images for aesthetic purposes
    //
    _centreImage(_baseImage);
    for (auto it = _fingerImageList.begin(); it != _fingerImageList.end(); it++)
    {
        _centreImage(*it);
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
    _window = SDL_CreateWindow("Human Interface for Robotic Control",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height,
        SDL_WINDOW_SHOWN);
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
        // Notifies the Application class that the user wants to quit
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
    // Renders all images to screen
    //
    _baseImage->onRender();
    for (auto it = _fingerImageList.begin(); it != _fingerImageList.end();
        it++)
    {
        (*it)->onRender();
    }

    //
    // Updates screen (swaps screen buffers)
    //
    SDL_RenderPresent(_renderer);
}

//*****************************************************************************
//
//! Centre the image on the screen.
//!
//! \param image the image to be centred.
//!
//! \return None.
//
//*****************************************************************************
void Window::_centreImage(const std::unique_ptr<Image> &image)
{
    SDL_Rect centredRect;
    centredRect.w = image->getWidth();
    centredRect.h = image->getHeight();
    centredRect.x = (_width - centredRect.w) / 2;
    centredRect.y = (_height - centredRect.h) / 2;
    image->setRenderRect(&centredRect);
}
