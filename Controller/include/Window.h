//*****************************************************************************
//
// Window.h
//
// Class responsible for spawning, managing, and closing the OpenGL context.
//
// Created:
// December 27, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <vector>

#include "Image.h"
#include "IObserver.h"

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
    std::vector<Image> _renderList;

    /* Methods */
    bool _initialize();
    void _terminate();

public:
    /* Constructor */
    Window();

    /* Destructor */
    ~Window();

    /* Methods */
    void update();
};

#endif /* _WINDOW_H_ */
