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
// January 3, 2016
//
//*****************************************************************************
#include "Window.h"

#include <iostream>

#include <SDL.h>
#include <SDL_syswm.h>

#include "res.h"

//*****************************************************************************
//
//! Constructor for Window. Acquires resources for window, renderer, and hand
//! model
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Window::Window()
    : _window(nullptr), _menu(nullptr), _renderer(nullptr),
      _width(_DEFAULT_WIDTH), _height(_DEFAULT_HEIGHT)
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
    // Creates hand model
    //
    _hand = std::unique_ptr<Hand>(new Hand(_renderer, _width, _height));
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
void Window::update(FingerPressureStruct *fingerPressures)
{
    _processInput();
    _update(fingerPressures);
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
    if (_window == nullptr)
    {
        std::cerr << "[ERROR] Window::_initialize(): Could not create window "\
            ". SDL Error " << SDL_GetError() << std::endl;
        return false;
    }

    //
    // Receive HWND information from window in order to create menu bar
    //
    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version);
    if (!SDL_GetWindowWMInfo(_window, &windowInfo))
    {
        std::cerr << "[ERROR] Window::_initialize(): Could not query window "\
            "information. SDL Error " << SDL_GetError() << std::endl;
        return false;
    }
    _windowHandle = windowInfo.info.win.window;

    //
    // Creates and attaches menu bar to window
    //
    _menu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU));
    if (_menu == nullptr)
    {
        std::cerr << "[ERROR] Window::_initialize(): Could not create menu. "\
            "SDL Error " << SDL_GetError() << std::endl;
        return false;
    }
    SetMenu(_windowHandle, _menu);

    //
    // Creates renderer
    //
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_TARGETTEXTURE);
    if (_renderer == nullptr)
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

    //
    // The system window manager event contains a pointer to system-specific
    // information about unknown window manager events. If you enable this
    // event using SDL_EventState(), it will be generated whenever unhandled
    // events are received from the window manager.
    //
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

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
    if (_renderer != nullptr)
    {
        SDL_DestroyRenderer(_renderer);
    }

    if (_menu != nullptr)
    {
        DestroyMenu(_menu);
    }

    if (_window != nullptr)
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
        switch (event.type)
        {
        case SDL_QUIT:
            //
            // Notifies the Application class that the user wants to quit
            //
            notify(SDL_QUIT);
            break;

        case SDL_SYSWMEVENT:
            //
            // Native Win32 system event
            //
            switch (event.syswm.msg->msg.win.msg)
            {
            case WM_COMMAND:
                //
                // Menu bar event
                //
                switch (LOWORD(event.syswm.msg->msg.win.wParam))
                {
                case IDM_CONNECT:
                    //
                    // File -> Connect
                    //
                    MessageBox(_windowHandle, "Not implemented",
                        "Not implemented", MB_ICONINFORMATION | MB_OK);
                    break;
                case IDM_DISCONNECT:
                    //
                    // File -> Disconnect
                    //
                    MessageBox(_windowHandle, "Not implemented",
                        "Not implemented", MB_ICONINFORMATION | MB_OK);
                    break;
                case IDM_QUIT:
                    //
                    // File -> Quit
                    //
                    notify(SDL_QUIT);
                    break;
                case IDM_RECORD:
                    //
                    // Options -> Record
                    //
                    MessageBox(_windowHandle, "Not implemented",
                        "Not implemented", MB_ICONINFORMATION | MB_OK);
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        default:
            break;
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
void Window::_update(FingerPressureStruct *fingerPressures)
{
    _hand->update(fingerPressures);
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
    _hand->render();

    //
    // Updates screen (swaps screen buffers)
    //
    SDL_RenderPresent(_renderer);
}
