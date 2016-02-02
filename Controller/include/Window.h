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
// Feburary 1, 2016
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
#include "Panel.h"

extern struct SDL_Window;
extern struct SDL_Renderer;

void panelTask(std::shared_ptr<Panel> panel);
void CenterWindow(HWND hwnd);

class Window
{
private:
    /* Fields */
    const int _DEFAULT_WIDTH = 640;
    const int _DEFAULT_HEIGHT = 480;
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
};

#endif /* _WINDOW_H_ */
