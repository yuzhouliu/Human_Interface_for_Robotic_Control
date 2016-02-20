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
// Feburary 19, 2016
//
//*****************************************************************************
#include "Window.h"

#include <Commdlg.h> /* OPENFILENAME */
#include <fstream>
#include <iostream>
#include <thread>

#include <SDL.h>
#include <SDL_syswm.h>

#include "IPv4Address.h"

#include "resource.h"

//
// Enabling Visual Styles (for combo box)
//
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <Commctrl.h>

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
                    DialogBoxParam(GetModuleHandle(NULL),
                        MAKEINTRESOURCE(IDD_CONNECT), NULL,
                        Window::ConnectDlgProcRouter, (LPARAM)(this));
                    break;
                case ID_FILE_DISCONNECT:
                    //
                    // File -> Disconnect
                    //
                    _panel->disconnect();
                    EnableMenuItem(_menu, ID_FILE_DISCONNECT, MF_GRAYED);
                    EnableMenuItem(_menu, ID_FILE_CONNECT, MF_ENABLED);
                    break;
                case ID_FILE_QUIT:
                    //
                    // File -> Quit
                    //
                    gExit = true;
                    break;
                case ID_OPTIONS_STARTRECORDING:
                {
                    //
                    // Options -> Start Recording
                    //
                    OPENFILENAME openFileName;
                    char filePathBuf[MAX_PATH] = "";

                    ZeroMemory(&openFileName, sizeof(openFileName));

                    openFileName.lStructSize = sizeof(openFileName);
                    openFileName.hwndOwner = _windowHandle;
                    openFileName.lpstrFilter = "HIRC Files (*.hirc)\0*.hirc\0"\
                        "All Files (*.*)\0*.*\0";
                    openFileName.lpstrFile = filePathBuf;
                    openFileName.nMaxFile = MAX_PATH;
                    openFileName.lpstrDefExt = "hirc";
                    openFileName.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST |
                        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

                    if (GetSaveFileName(&openFileName))
                    {
                        _panel->startRecording(filePathBuf);
                    }

                    EnableMenuItem(_menu, ID_OPTIONS_STARTRECORDING, MF_GRAYED);
                    EnableMenuItem(_menu, ID_OPTIONS_STOPRECORDING, MF_ENABLED);
                    break;
                }
                case ID_OPTIONS_STOPRECORDING:
                    //
                    // Options -> Stop Recording
                    //
                    _panel->stopRecording();
                    EnableMenuItem(_menu, ID_OPTIONS_STARTRECORDING, MF_ENABLED);
                    EnableMenuItem(_menu, ID_OPTIONS_STOPRECORDING, MF_GRAYED);
                    /*MessageBox(_windowHandle, "Not implemented",
                        "Not implemented", MB_ICONINFORMATION | MB_OK);*/
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
//! Save IP address to file
//!
//! \param ipAddress IP address to save.
//!
//! \return Returns \b true if the address was saved successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool Window::_saveIPAddress(std::string ipAddress)
{
    //
    // Opens a file stream to save IP address to file
    //
    std::ofstream addressFile;
    addressFile.open(_addressFilePath, std::ios::app);
    if (!addressFile.is_open())
    {
        std::cerr << "[ERROR] Panel::_saveIPAddress(): Unable to open file."
            << std::endl;
        return false;
    }

    //
    // Save IP address to file
    //
    addressFile << ipAddress << std::endl;

    addressFile.close();
    return true;
}

//*****************************************************************************
//
//! Thread function responsible for running the panel concurrently with the
//! application in order to bypass the modal windows event loop.
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

//*****************************************************************************
//
//! Moves window to center of screen.
//!
//! \param hwnd handle to the window.
//!
//! \return None.
//
//*****************************************************************************
void CenterWindow(HWND hwnd)
{
     RECT rectWindow, rectDesktop;
 
     //
     // Make the window relative to its Desktop
     //
     GetWindowRect(hwnd, &rectWindow);
     GetWindowRect(GetDesktopWindow(), &rectDesktop);

     int nWidth = rectWindow.right - rectWindow.left;
     int nHeight = rectWindow.bottom - rectWindow.top;

     int nX = ((rectDesktop.right - rectDesktop.left) - nWidth) / 2
         + rectDesktop.left;
     int nY = ((rectDesktop.bottom - rectDesktop.top) - nHeight) / 2
         + rectDesktop.top;

     int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
     int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

     //
     // Make sure that the dialog box never moves outside of the screen
     //
     if (nX < 0) nX = 0;
     if (nY < 0) nY = 0;
     if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
     if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

     MoveWindow(hwnd, nX, nY, nWidth, nHeight, FALSE);
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
BOOL CALLBACK Window::ConnectDlgProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        //
        // Opens file stream to read saved IP addresses
        //
        std::string ipAddress;
        std::ifstream addressFile;
        addressFile.open(_addressFilePath);
        if (!addressFile.is_open())
        {
            std::cerr << "[ERROR] Panel::ConnectDlgProc(): Unable to open "\
                "file." << std::endl;
            return FALSE;
        }

        //
        // Populates combo box with list of saved IP addresses
        //
        while (getline(addressFile, ipAddress))
        {
            SendDlgItemMessage(hwnd, IDC_COMBO, CB_ADDSTRING, 0,
                (LPARAM)ipAddress.c_str());
        }
        addressFile.close();

        //
        // Sets minimum drop down size to 5 elements and centers window
        //
        SendDlgItemMessage(hwnd, IDC_COMBO, CB_SETMINVISIBLE, 5, 0);
        CenterWindow(hwnd);
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CONNECT:
        {
            //
            // Get IP address that user inputted
            //
            HWND hComboBox = GetDlgItem(hwnd, IDC_COMBO);
            char addressInput[IPv4Address::MAX_IP_ADDR_BUF_LEN];
            int len = GetWindowTextLength(hComboBox);
            if ((len >= IPv4Address::MIN_IP_ADDR_LEN) &&
                (len <= IPv4Address::MAX_IP_ADDR_LEN))
            {
                GetDlgItemText(hwnd, IDC_COMBO, addressInput,
                    IPv4Address::MAX_IP_ADDR_LEN);
                if (IPv4Address::validateIPAddress(addressInput))
                {
                    //
                    // Connect to remote host
                    //
                    //_panel->connect(addressInput);

                    //
                    // Insert IP address into combo box and save IP address if
                    // it does not exist in combo box already
                    //
                    if (SendDlgItemMessage(hwnd, IDC_COMBO, CB_FINDSTRINGEXACT,
                        -1, (LPARAM)addressInput) == CB_ERR)
                    {
                        _saveIPAddress(std::string(addressInput));
                        SendDlgItemMessage(hwnd, IDC_COMBO, CB_ADDSTRING, 0,
                            (LPARAM)addressInput);
                    }
                    EnableMenuItem(_menu, ID_FILE_DISCONNECT, MF_ENABLED);
                    EnableMenuItem(_menu, ID_FILE_CONNECT, MF_GRAYED);
                    EndDialog(hwnd, 0);
                }
                else
                {
                    MessageBox(hwnd, "Invalid IP address",
                        "Invalid IP address", MB_ICONINFORMATION | MB_OK);
                }
            }
            else
            {
                MessageBox(hwnd, "Invalid IP address",
                    "Invalid IP address", MB_ICONINFORMATION | MB_OK);
            }
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
//! Static DlgProc router function for File->Connect, that routes the event to
//! the correct DlgProc for further processing.
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
BOOL CALLBACK Window::ConnectDlgProcRouter(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    Window *window;

    //
    // The pointer to the Window instance is passed in lParam on dialog
    // initialization
    //
    if (msg == WM_INITDIALOG)
    {
        window = reinterpret_cast<Window*>(lParam);

        //
        // Store the pointer to the Window instance as user data in the dialog
        // box
        //
        SetWindowLong(hwnd, GWL_USERDATA,
            reinterpret_cast<LONG_PTR>(window));
    }

    //
    // Fetch the pointer to the Window instance from the dialog box
    //
    window = reinterpret_cast<Window*>(GetWindowLong(hwnd, GWL_USERDATA));

    //
    // Delegate the call of the DlgProc function to the correct instance
    //
    if (window)
    {
        return window->ConnectDlgProc(hwnd, msg, wParam, lParam);
    }

    return FALSE;
}
