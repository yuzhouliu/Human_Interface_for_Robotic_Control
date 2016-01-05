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
// January 4, 2016
//
//*****************************************************************************
#ifndef _PANEL_H_
#define _PANEL_H_

#include <memory>

#include <SDL.h>

#include "Hand.h"
#include "IObservable.h"

class Panel : public IObservable
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
};

#endif /* _PANEL_H_ */
