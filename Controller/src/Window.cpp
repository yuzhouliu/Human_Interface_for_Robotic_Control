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
// January 7, 2016
//
//*****************************************************************************
#include "Window.h"

#include <iostream>
#include <thread>

#include <SDL.h>
#include <SDL_syswm.h>

#include "resource.h"

//
// Initialize static variable
//
bool Window::gExit = false;

//*****************************************************************************
//
//! Constructor for Window. Acquires resources for window.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Window::Window()
    : _window(nullptr), _menu(nullptr), _width(_DEFAULT_WIDTH),
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
}

//*****************************************************************************
//
//! Destructor for Window. Releases resources used by window.
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
//! Starts the window.
//!
//! \param None.
//!
//! \return Returns \b true if the window was run successfully and \b
//! false otherwise.
//
//*****************************************************************************
bool Window::run()
{
    //
    // Creates panel
    //
    _panel = std::shared_ptr<Panel>(new Panel(_window));

    //
    // Creates thread
    //
    std::thread panelThread(panelTask, _panel);

    //
    // Process inputs until user quits
    //
    while (!gExit)
    {
        _processInput();
    }

    //
    // Wait for thread to finish
    //
    panelThread.join();

    return true;
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
    _menu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
    if (_menu == nullptr)
    {
        std::cerr << "[ERROR] Window::_initialize(): Could not create menu. "\
            "SDL Error " << SDL_GetError() << std::endl;
        return false;
    }
    SetMenu(_windowHandle, _menu);

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
            // User wants to quit
            //
            gExit = true;
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
                case ID_FILE_CONNECT:
                    //
                    // File -> Connect
                    //
                    DialogBox(GetModuleHandle(NULL), 
                        MAKEINTRESOURCE(IDD_CONNECT), NULL, ConnectDlgProc);
                    break;
                case ID_FILE_DISCONNECT:
                    //
                    // File -> Disconnect
                    //
                    MessageBox(_windowHandle, "Not implemented",
                        "Not implemented", MB_ICONINFORMATION | MB_OK);
                    break;
                case ID_FILE_QUIT:
                    //
                    // File -> Quit
                    //
                    gExit = true;
                    break;
                case ID_OPTIONS_STARTRECORDING:
                    //
                    // Options -> Start Recording
                    //
                    MessageBox(_windowHandle, "Not implemented",
                        "Not implemented", MB_ICONINFORMATION | MB_OK);
                    break;
                case ID_OPTIONS_STOPRECORDING:
                    //
                    // Options -> Stop Recording
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
//! Dialog function for File->Connect dialog box.
//!
//! \param hwnd handle to the dialog box.
//! \param msg the message command received.
//! \param wParam notification message in high byte and control identifier of
//!     the control that sent the message in low byte.
//! \param lParam window handle to the control that sent the message.
//!
//! \return Returns 1 if the dialog box exited successfully and 0 otherwise.
//
//*****************************************************************************
BOOL CALLBACK ConnectDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CONNECT:
        {
            HWND hComboBox = GetDlgItem(hwnd, IDC_COMBO);
            int len = GetWindowTextLength(hComboBox);
            std::cout << len << std::endl;
            EndDialog(hwnd, 0);
            break;
        }
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

//*****************************************************************************
//
//! Thread function responsible for running the panel concurrently with the
//! application to bypass the modal windows event loop.
//!
//! \param panel the Panel that this thread function will run.
//!
//! \return None.
//
//*****************************************************************************
void panelTask(std::shared_ptr<Panel> panel)
{
    panel->run();
}
