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

extern struct SDL_Window;
extern struct SDL_Renderer;

class Window
{
private:
    /* Fields */
    SDL_Window *window;
    SDL_Renderer *renderer;
    unsigned short width;
    unsigned short height;
    std::vector<Image> renderList;

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
