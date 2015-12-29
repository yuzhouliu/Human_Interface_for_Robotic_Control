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
// December 28, 2015
//
//*****************************************************************************
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <memory>
#include <vector>

#include "Finger.h"
#include "Image.h"
#include "IObservable.h"

extern struct SDL_Window;
extern struct SDL_Renderer;

class Window : public IObservable
{
private:
    /* Fields */
    const int _DEFAULT_WIDTH = 640;
    const int _DEFAULT_HEIGHT = 480;
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    unsigned short _width;
    unsigned short _height;
    std::unique_ptr<Image> _baseImage;
    std::vector<std::unique_ptr<Finger>> _fingerList;

    /* Methods */
    bool _initialize();
    void _terminate();
    void _processInput();
    void _update();
    void _render();
    void _centreImage(const std::unique_ptr<Image> &image);

public:
    /* Constructor */
    Window();

    /* Destructor */
    ~Window();

    /* Methods */
    void update();
};

#endif /* _WINDOW_H_ */
