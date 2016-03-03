//*****************************************************************************
//
// Window.h
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
// March 3, 2016
//
//*****************************************************************************
#ifndef _WINDOW_H_
#define _WINDOW_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <vector>
#include <windows.h>

#include "Hand.h"
#include "Image.h"
#include "IObserver.h"
#include "Panel.h"

#define WINDOW_WIDTH 1100
#define WINDOW_HEIGHT 600
#define PRIMARY_VIEWPORT_X 0
#define PRIMARY_VIEWPORT_Y 0
#define PRIMARY_VIEWPORT_WIDTH 800
#define PRIMARY_VIEWPORT_HEIGHT 600
#define SECONDARY_VIEWPORT_X 850
#define SECONDARY_VIEWPORT_Y 25
#define SECONDARY_VIEWPORT_WIDTH 200
#define SECONDARY_VIEWPORT_HEIGHT 200
#define TERTIARY_VIEWPORT_X 850
#define TERTIARY_VIEWPORT_Y 250
#define TERTIARY_VIEWPORT_WIDTH 200
#define TERTIARY_VIEWPORT_HEIGHT 300

extern struct SDL_Window;
extern struct SDL_Renderer;

void panelTask(std::shared_ptr<Panel> panel);
void messageBoxTask(HWND windowHandle, std::string title, std::string message);
void CenterWindow(HWND hwnd);

enum WindowEvents
{
    EVENT_STOP_STREAMING = 1,
};

class Window : public IObserver
{
private:
    /* Fields */
    SDL_Window *_window;
    HWND _windowHandle;
    HMENU _menu;
    std::shared_ptr<Panel> _panel;
    unsigned short _width;
    unsigned short _height;
    const std::string _addressFilePath = "data/cache/IPAddresses.txt";

    /* Methods */
    bool _initialize();
    void _terminate();
    void _processInput();
    bool _saveIPAddress(std::string ipAddress);

public:
    /* Fields */
    static bool gExit;
    static SDL_Rect gPrimaryViewport;
    static SDL_Rect gSecondaryViewport;
    static SDL_Rect gTertiaryViewport;

    /* Constructor */
    Window();

    /* Destructor */
    ~Window();

    /* Methods */
    bool run();
    BOOL CALLBACK ConnectDlgProc(HWND hwnd, UINT msg, WPARAM wParam,
        LPARAM lParam);

    /* Static methods */
    static BOOL CALLBACK ConnectDlgProcRouter(HWND hwnd, UINT msg,
        WPARAM wParam, LPARAM lParam);

    /* IObserver methods */
    void onNotify(int event);
};

#endif /* _WINDOW_H_ */
