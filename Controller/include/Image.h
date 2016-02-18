//*****************************************************************************
//
// Image.h
//
// Class responsible for loading, managing, and closing textures.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 27, 2015
//
// Modified:
// Feburary 18, 2016
//
//*****************************************************************************
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <memory>
#include <string>
#include <SDL.h>

#include "IRenderable.h"

class Image : public IRenderable
{
private:
    /* Fields */
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    SDL_Rect _renderRect;
    unsigned short _width;
    unsigned short _height;
    unsigned short _angle;
    unsigned char _alpha;
    bool _alphaEnabled;

    /* Methods */
    bool _setTexture(SDL_Texture *texture);

public:
    /* Constructors */
    Image();
    Image(SDL_Renderer *renderer, std::string path);

    /* Destructor */
    ~Image();

    /* Methods */
    void setRenderer(SDL_Renderer *renderer);
    bool setTexture(std::string path);
    bool setTexture(SDL_Surface *surface);
    SDL_Texture *getTexture();
    unsigned short getWidth();
    unsigned short getHeight();
    void setRenderRect(SDL_Rect &renderRect);
    SDL_Rect getRenderRect();
    void setAngle(unsigned short angle);
    unsigned short getAngle();
    void enableAlphaBlend();
    void disableAlphaBlend();
    void setAlphaBlend(unsigned char alpha);
    unsigned char getAlphaBlend();

    /* IRenderable virtual methods */
    void onRender();

    /* Static methods */
    static void centreImage(const std::unique_ptr<Image> &image,
        unsigned short screenWidth, unsigned short screenHeight);
};

#endif /* _IMAGE_H_ */
