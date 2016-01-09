//*****************************************************************************
//
// Panel.h
//
// Class responsible for updating and rendering on drawing panel.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// January 3, 2016
//
// Modified:
// January 8, 2016
//
//*****************************************************************************
#ifndef _PANEL_H_
#define _PANEL_H_

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <windows.h>

#include <SDL.h>

#include "Hand.h"
#include "IObservable.h"

class Panel
{
private:
    /* Fields */
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    std::unique_ptr<Hand> _hand;

    /* Methods */
    bool _initialize();
    void _update(FingerPressureStruct *fingerPressures);
    void _render();
    bool _populateFingerPressureStruct(FingerPressureStruct &fingerPressures,
        unsigned char *buf, unsigned int buflen);

public:
    /* Constructor */
    Panel(SDL_Window *window);

    /* Destructor */
    ~Panel();

    /* Methods */
    void run();
    BOOL CALLBACK ConnectDlgProc(HWND hwnd, UINT msg, WPARAM wParam,
        LPARAM lParam);

    /* Static methods */
    static BOOL CALLBACK DlgProcRouter(HWND hwnd, UINT msg, WPARAM wParam,
        LPARAM lParam);
};

#endif /* _PANEL_H_ */
