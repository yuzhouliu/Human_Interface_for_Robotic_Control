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
// March 5, 2016
//
//*****************************************************************************
#include "Window.h"

#include <Commdlg.h> /* OPENFILENAME */
#include <cassert>
#include <fstream>
#include <iostream>
#include <thread>

#include <SDL.h>
#include <SDL_syswm.h>

#include "IPv4Address.h"
#include "TCPSocket.h"

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
SDL_Rect Window::gPrimaryViewport = {PRIMARY_VIEWPORT_X, PRIMARY_VIEWPORT_Y,
    PRIMARY_VIEWPORT_WIDTH, PRIMARY_VIEWPORT_HEIGHT};
SDL_Rect Window::gSecondaryViewport = {SECONDARY_VIEWPORT_X, SECONDARY_VIEWPORT_Y,
    SECONDARY_VIEWPORT_WIDTH, SECONDARY_VIEWPORT_HEIGHT};
SDL_Rect Window::gTertiaryViewport = {TERTIARY_VIEWPORT_X, TERTIARY_VIEWPORT_Y,
    TERTIARY_VIEWPORT_WIDTH, TERTIARY_VIEWPORT_HEIGHT};

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
    : _window(nullptr), _menu(nullptr), _width(WINDOW_WIDTH),
      _height(WINDOW_HEIGHT)
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
    _panel = std::shared_ptr<Panel>(new Panel(this, _window));
    _panel->addObserver(this);

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
                case ID_FILE_AUTO:
                {
                    //
                    // File -> Auto-connect
                    //
                    char ipAddress[IPv4Address::MAX_IP_ADDR_BUF_LEN];

                    // Fetch IP address from server
                    if (!_getRemoteIPAddress(ipAddress,
                        IPv4Address::MAX_IP_ADDR_BUF_LEN))
                    {
                        //
                        // Creates daemon thread
                        //
                        std::thread messageBoxThread(messageBoxTask,
                            _windowHandle,
                            "Unable to fetch IP address from server",
                            "Auto-connect failed");
                        messageBoxThread.detach();
                        break;
                    }

                    if (!IPv4Address::validateIPAddress(ipAddress))
                    {
                        //
                        // Creates daemon thread
                        //
                        std::thread messageBoxThread(messageBoxTask,
                            _windowHandle,
                            "Invalid IP address found from server",
                            "Auto-connect failed");
                        messageBoxThread.detach();
                        break;
                    }

                    //
                    // Connect to remote host
                    //
                    if (!_panel->connect(ipAddress))
                    {
                        //
                        // Creates daemon thread
                        //
                        std::thread messageBoxThread(messageBoxTask,
                            _windowHandle,
                            "Unable to auto-connect to remote host",
                            "Auto-connect failed");
                        messageBoxThread.detach();
                        break;
                    }

                    EnableMenuItem(_menu, ID_FILE_DISCONNECT, MF_ENABLED);
                    EnableMenuItem(_menu, ID_FILE_CONNECT, MF_GRAYED);
                    EnableMenuItem(_menu, ID_FILE_AUTO, MF_GRAYED);
                    EndDialog(_windowHandle, 0);
                    break;
                }
                case ID_FILE_DISCONNECT:
                    //
                    // File -> Disconnect
                    //
                    _panel->disconnect();
                    EnableMenuItem(_menu, ID_FILE_DISCONNECT, MF_GRAYED);
                    EnableMenuItem(_menu, ID_FILE_CONNECT, MF_ENABLED);
                    EnableMenuItem(_menu, ID_FILE_AUTO, MF_ENABLED);
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
                        EnableMenuItem(_menu, ID_OPTIONS_STARTRECORDING,
                            MF_GRAYED);
                        EnableMenuItem(_menu, ID_OPTIONS_STOPRECORDING,
                            MF_ENABLED);
                        EnableMenuItem(_menu, ID_OPTIONS_STARTPLAYBACK,
                            MF_GRAYED);
                        EnableMenuItem(_menu, ID_OPTIONS_STOPPLAYBACK,
                            MF_GRAYED);
                    }

                    break;
                }
                case ID_OPTIONS_STOPRECORDING:
                    //
                    // Options -> Stop Recording
                    //
                    _panel->stopRecording();
                    EnableMenuItem(_menu, ID_OPTIONS_STARTRECORDING,
                        MF_ENABLED);
                    EnableMenuItem(_menu, ID_OPTIONS_STOPRECORDING, MF_GRAYED);
                    EnableMenuItem(_menu, ID_OPTIONS_STARTPLAYBACK,
                        MF_ENABLED);
                    EnableMenuItem(_menu, ID_OPTIONS_STOPPLAYBACK, MF_GRAYED);
                    /*MessageBox(_windowHandle, "Not implemented",
                        "Not implemented", MB_ICONINFORMATION | MB_OK);*/
                    break;
                case ID_OPTIONS_STARTPLAYBACK:
                {
                    //
                    // Options -> Start Playback
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
                        OFN_HIDEREADONLY;

                    if (GetOpenFileName(&openFileName))
                    {
                        _panel->startStreaming(filePathBuf);
                        EnableMenuItem(_menu, ID_OPTIONS_STARTRECORDING,
                            MF_GRAYED);
                        EnableMenuItem(_menu, ID_OPTIONS_STOPRECORDING,
                            MF_GRAYED);
                        EnableMenuItem(_menu, ID_OPTIONS_STARTPLAYBACK,
                            MF_GRAYED);
                        EnableMenuItem(_menu, ID_OPTIONS_STOPPLAYBACK,
                            MF_ENABLED);
                    }

                    break;
                }
                case ID_OPTIONS_STOPPLAYBACK:
                    //
                    // Options -> Stop Playback
                    //
                    _panel->stopStreaming();
                    EnableMenuItem(_menu, ID_OPTIONS_STARTRECORDING,
                        MF_ENABLED);
                    EnableMenuItem(_menu, ID_OPTIONS_STOPRECORDING, MF_GRAYED);
                    EnableMenuItem(_menu, ID_OPTIONS_STARTPLAYBACK,
                        MF_ENABLED);
                    EnableMenuItem(_menu, ID_OPTIONS_STOPPLAYBACK, MF_GRAYED);
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
//! Save IP address to file.
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
//! Fetches IP address of remote host.
//!
//! \param buf buffer to store the fetched IP address.
//! \param buflen size of buffer.
//!
//! \return Returns \b true if the address was fetched successfully and \b
//! false otherwise.
//
//*****************************************************************************
bool Window::_getRemoteIPAddress(char *buf, int buflen)
{
    assert(buflen >= IPv4Address::MAX_IP_ADDR_BUF_LEN);
    struct hostent *he;
    struct in_addr **addr_list;
    char *hostName = "robotarm.net23.net";
    char *http_request_format = "GET /fetch_ip.php?id=1 HTTP/1.1\r\n"
                                "Host: %s\r\n"
                                "\r\n";

    //
    // Populate hostent structure with IP address information
    //
    if ((he = gethostbyname(hostName)) == NULL) 
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Could not locate"\
            " host." << std::endl;
        return false;
    }
    addr_list = (struct in_addr**)he->h_addr_list;
     
    //
    // Iterate through list of IP addresses and use first one found
    //
    char hostAddress[IPv4Address::MAX_IP_ADDR_BUF_LEN];
    bool hostAddressFound = false;
    for(int i=0; addr_list[i]!=NULL; i++) 
    {
        //The first IP address that matches;
        strncpy_s(hostAddress, inet_ntoa(*addr_list[i]),
            IPv4Address::MAX_IP_ADDR_BUF_LEN);
        hostAddressFound = true;
    }
    //std::cout << "Host address = " << hostAddress << std::endl;

    //
    // Check if an IP address was found
    //
    if (!hostAddressFound)
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Host address not"\
            " found." << std::endl;
        return false;
    }

    //
    // Validate IP address
    //
    if (!IPv4Address::validateIPAddress(hostAddress))
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Invalid host "\
            "address found." << std::endl;
        return false;
    }

    //
    // Creates and opens socket
    //
    TCPSocket socket;
    if (!socket.open())
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Could not open "\
            "socket." << std::endl;
        return false;
    }

    //
    // Connects to host
    //
    const int HTTP_PORT = 80;
    IPv4Address ipAddress(hostAddress, HTTP_PORT);
    if (!socket.connect(&ipAddress))
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Could not "\
            "connect to host." << std::endl;
        return false;
    }

    //
    // Send HTTP request to host
    //
    const int MAX_HTTP_BUF_LEN = 256;
    char http_request[MAX_HTTP_BUF_LEN];
    sprintf_s(http_request, http_request_format, hostName);
    //std::cout << "HTTP REQUEST\n" << http_request << std::endl;
    if (!socket.send((unsigned char*)http_request, MAX_HTTP_BUF_LEN))
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Could not "\
            "send HTTP request to host." << std::endl;
        return false;
    }

    //
    // Receive HTTP response from host
    //
    char http_response[MAX_HTTP_BUF_LEN];
    if (!socket.recv((unsigned char*)http_response, MAX_HTTP_BUF_LEN))
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Could not "\
            "receive HTTP response from host." << std::endl;
        return false;
    }
    //std::cout << "HTTP RESPONSE\n" << http_response << std::endl;

    //
    // Find Content-Length header
    //
    int pos = 0;
    std::string httpResponseString(http_response);
    std::string contentLengthHeader("Content-Length: ");
    if ((pos = httpResponseString.find(contentLengthHeader)) ==
        std::string::npos)
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Content-Length "\
            "header not present in HTTP response." << std::endl;
        return false;
    }
    httpResponseString.erase(0, pos+contentLengthHeader.length());

    //
    // Get Content-Length value
    //
    std::string CRLF("\r\n");
    if ((pos = httpResponseString.find(CRLF)) == std::string::npos)
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Content-Length "\
            "value not present in HTTP response." << std::endl;
        return false;
    }
    std::string contentLengthString = httpResponseString.substr(0, pos);
    int contentLength = atoi(contentLengthString.c_str());

    //
    // Get HTTP response body
    //
    std::string httpHeaderBodyDelimiter("\r\n\r\n");
    if ((pos = httpResponseString.find(httpHeaderBodyDelimiter)) ==
        std::string::npos)
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): Body not present"\
            " in HTTP response." << std::endl;
        return false;
    }
    httpResponseString.erase(0, pos+httpHeaderBodyDelimiter.length());
    std::string httpResponseBody = httpResponseString.substr(0, contentLength);
    std::cout << "HTTP RESPONSE BODY\n" << httpResponseBody << std::endl;

    //
    // Get IP address from HTTP response body
    //
    std::string colonDelimiter(":");
    if ((pos = httpResponseBody.find(colonDelimiter)) == std::string::npos)
    {
        std::cout << "[ERROR] Window::_getRemoteIPAddress(): HTTP response "\
            "body mal-formed." << std::endl;
        return false;
    }
    std::string ipAddressString = httpResponseBody.substr(0, pos);
    strncpy_s(buf, buflen, ipAddressString.c_str(), buflen);

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
//! Thread function responsible for opening a message box with specified
//! message.
//!
//! \param windowHandle handle to the parent window.
//! \param title the title to display.
//! \param message the message to display.
//!
//! \return None.
//
//*****************************************************************************
void messageBoxTask(HWND windowHandle, std::string title, std::string message)
{
    MessageBox(windowHandle, title.c_str(),
        message.c_str(), MB_ICONINFORMATION | MB_OK);
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
                    if (!_panel->connect(addressInput))
                    {
                        //
                        // Creates daemon thread
                        //
                        std::thread messageBoxThread(messageBoxTask,
                            _windowHandle,
                            "Unable to connect to remote host",
                            "Connect failed");
                        messageBoxThread.detach();

                        EndDialog(hwnd, 0);
                        break;
                    }

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

//*****************************************************************************
//
//! Event handler for events that Window is listening for. (Virtual method of
//! IObserver interface)
//!
//! \param event the identifier of the event.
//!
//! \return None.
//
//*****************************************************************************
void Window::onNotify(int event)
{
    switch (event)
    {
    case EVENT_DISCONNECTED:
    {
        //
        // Event occurs when PlaybackStreamer reaches end of file
        //
        EnableMenuItem(_menu, ID_FILE_DISCONNECT, MF_GRAYED);
        EnableMenuItem(_menu, ID_FILE_CONNECT, MF_ENABLED);
        EnableMenuItem(_menu, ID_FILE_AUTO, MF_ENABLED);

        //
        // Creates daemon thread
        //
        std::thread messageBoxThread(messageBoxTask, _windowHandle,
            "Connection to InMoov lost", "Disconnected");
        messageBoxThread.detach();

        break;
    }
    case EVENT_STOP_STREAMING:
    {
        //
        // Event occurs when PlaybackStreamer reaches end of file
        //
        _panel->stopStreaming();
        EnableMenuItem(_menu, ID_OPTIONS_STARTRECORDING, MF_ENABLED);
        EnableMenuItem(_menu, ID_OPTIONS_STOPRECORDING, MF_GRAYED);
        EnableMenuItem(_menu, ID_OPTIONS_STARTPLAYBACK, MF_ENABLED);
        EnableMenuItem(_menu, ID_OPTIONS_STOPPLAYBACK, MF_GRAYED);

        //
        // Creates daemon thread
        //
        std::thread messageBoxThread(messageBoxTask, _windowHandle,
            "Playback reached end of file", "Playback Complete");
        messageBoxThread.detach();

        break;
    }
    default:
        break;
    }
}
