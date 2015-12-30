//*****************************************************************************
//
// Image.cpp
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
// December 28, 2015
//
//*****************************************************************************
#include "Image.h"

#include <iostream>
#include <SDL_image.h>

//*****************************************************************************
//
//! Empty constructor for Image.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Image::Image()
    : Image(nullptr, nullptr)
{

}

//*****************************************************************************
//
//! Constructor for Image. Loads a texture for renderer from path.
//!
//! \param renderer that the texture will be rendered on.
//! \param path to image on file system.
//!
//! \return None.
//
//*****************************************************************************
Image::Image(SDL_Renderer *renderer, std::string path)
    : _renderer(renderer), _texture(nullptr), _width(0), _height(0), _angle(0),
      _alpha(SDL_ALPHA_OPAQUE), _alphaEnabled(false)
{
    if ((renderer != nullptr) && !setTexture(path))
    {
        std::cerr << "[ERROR] Image::Image(): Texture was not set." <<
            std::endl;
        return;
    }
}

//*****************************************************************************
//
//! Destructor for Image. Releases resources used by texture.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Image::~Image()
{
    if (_texture != nullptr)
    {
        SDL_DestroyTexture(_texture);
    }
}

//*****************************************************************************
//
//! Sets the renderer that the class will use.
//!
//! \param path to image on file system.
//!
//! \return Returns \b true if the renderer was set successfully and \b false
//! otherwise.
//
//*****************************************************************************
void Image::setRenderer(SDL_Renderer *renderer)
{
    _renderer = renderer;
}

//*****************************************************************************
//
//! Sets the texture that the class will manage.
//!
//! \param path to image on file system.
//!
//! \return Returns \b true if the texture was loaded and set successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Image::setTexture(std::string path)
{
    if (_renderer == nullptr)
    {
        std::cerr << "[ERROR] Image::setTexture(): Renderer has not been set."\
            " Call setRenderer() first." << std::endl;
        return false;
    }

    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;

    //
    // Loads surface from path
    //
    surface = IMG_Load(path.c_str());
    if (surface == nullptr)
    {
        std::cerr << "[ERROR] Image::setTexture(): Unable to load image from "
            << path.c_str() << "! SDL_image Error: " << IMG_GetError() <<
            std::endl;
        return false;
    }

    //
    // Color key surface (Cyan: 0, 255, 255)
    //
    if (SDL_SetColorKey(surface, true, SDL_MapRGB(surface->format, 0, 0xFF,
        0xFF)) < 0)
    {
        std::cerr << "[ERROR] Image::setTexture(): Unable to color key image "\
            "from " << path.c_str() << "! SDL Error: " << SDL_GetError() <<
            std::endl;
        return false;
    }

    //
    // Creates texture from surface
    //
    texture = SDL_CreateTextureFromSurface(_renderer, surface);
    if (texture == nullptr)
    {
        std::cerr << "[ERROR] Image::setTexture(): Unable to create texture "\
            "from " << path.c_str() << "SDL Error: " << SDL_GetError() <<
            std::endl;
        return false;
    }
    SDL_FreeSurface(surface);

    return _setTexture(texture);
}

//*****************************************************************************
//
//! Gets the texture.
//!
//! \param None.
//!
//! \return Returns a pointer to the texture.
//
//*****************************************************************************
SDL_Texture *Image::getTexture()
{
    return _texture;
}

//*****************************************************************************
//
//! Gets the width.
//!
//! \param None.
//!
//! \return Returns the width.
//
//*****************************************************************************
unsigned short Image::getWidth()
{
    return _width;
}

//*****************************************************************************
//
//! Gets the height.
//!
//! \param None.
//!
//! \return Returns the height.
//
//*****************************************************************************
unsigned short Image::getHeight()
{
    return _height;
}

//*****************************************************************************
//
//! Sets the rectangle that dictates where the texture will be rendered.
//!
//! \param renderRect to set.
//!
//! \return None.
//
//*****************************************************************************
void Image::setRenderRect(SDL_Rect *renderRect)
{
    _renderRect = *renderRect;
}

//*****************************************************************************
//
//! Gets the rectangle that dictates where the texture will be rendered.
//!
//! \param None.
//!
//! \return Return the rectangle that dictates where the texture will be
//! rendered.
//
//*****************************************************************************
SDL_Rect Image::getRenderRect()
{
    return _renderRect;
}

//*****************************************************************************
//
//! Sets the angle in which the image will be rendered.
//!
//! \param angle to be set.
//!
//! \return None.
//
//*****************************************************************************
void Image::setAngle(unsigned short angle)
{
    if (angle > 360)
    {
        angle %= 360;
    }

    _angle = angle;
}

//*****************************************************************************
//
//! Gets the angle in which the image will be rendered.
//!
//! \param None.
//!
//! \return Returns the angle in which the image will be rendered.
//
//*****************************************************************************
unsigned short Image::getAngle()
{
    return _angle;
}

//*****************************************************************************
//
//! Enables alpha blending. Must be called before setAlphaBlend().
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Image::enableAlphaBlend()
{
    if (!_alphaEnabled)
    {
        SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
        _alphaEnabled = true;
    }
    else
    {
        std::cerr << "[WARNING] Image::enableAlphaBlend(): Alpha blending "\
            "already enabled." << std::endl;
    }
}

//*****************************************************************************
//
//! Disables alpha blending.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Image::disableAlphaBlend()
{
    if (_alphaEnabled)
    {
        SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_NONE);
        _alphaEnabled = false;
    }
    else
    {
        std::cerr << "[WARNING] Image::disableAlphaBlend(): Alpha blending "\
            "already disabled." << std::endl;
    }
}

//*****************************************************************************
//
//! Sets alpha blend value.
//!
//! \param alpha to set.
//!
//! \return None.
//
//*****************************************************************************
void Image::setAlphaBlend(unsigned char alpha)
{
    if (_alphaEnabled)
    {
        _alpha = alpha;
        SDL_SetTextureAlphaMod(_texture, alpha);
    }
    else
    {
        std::cerr << "[ERROR] Image::setAlphaBlend(): Alpha blending has not "\
            "been enabled. Call enableAlphaBlend() first." << std::endl;
    }
}

//*****************************************************************************
//
//! Gets alpha blend value.
//!
//! \param None.
//!
//! \return Returns alpha blend value.
//
//*****************************************************************************
unsigned char Image::getAlphaBlend()
{
    if (!_alphaEnabled)
    {
        std::cerr << "[WARNING] Image::getAlphaBlend(): Called before alpha "\
            "blending has been enabled." << std::endl;
    }

    return _alpha;
}

//*****************************************************************************
//
//! Renders image on screen.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Image::onRender()
{
    if (_renderer == nullptr)
    {
        std::cerr << "[WARNING] Image::onRender(): Image not rendered because"\
            " renderer was not set." << std::endl;
        return;
    }

    if (_texture == nullptr)
    {
        std::cerr << "[WARNING] Image::onRender(): Image not rendered because"\
            " texture was not set." << std::endl;
        return;
    }

    //
    // Renders image to buffered screen
    //
    SDL_RenderCopy(_renderer, _texture, nullptr, &_renderRect);
}

//*****************************************************************************
//
//! Centre the image on the screen.
//!
//! \param image the image to be centred.
//!
//! \return None.
//
//*****************************************************************************
void Image::centreImage(const std::unique_ptr<Image> &image,
    unsigned short screenWidth, unsigned short screenHeight)
{
    SDL_Rect centredRect;
    centredRect.w = image->getWidth();
    centredRect.h = image->getHeight();
    centredRect.x = (screenWidth - centredRect.w) / 2;
    centredRect.y = (screenHeight - centredRect.h) / 2;
    image->setRenderRect(&centredRect);
}

//*****************************************************************************
//
//! Sets the texture that the class will manage.
//!
//! \param texture to set.
//!
//! \return Returns \b true if the texture was loaded and set successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Image::_setTexture(SDL_Texture *texture)
{
    if (texture == nullptr)
    {
        return false;
    }

    //
    // Destroys old texture and reset variables
    //
    if (_texture != nullptr)
    {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
        _width = 0;
        _height = 0;
        _angle = 0;
        _alpha = SDL_ALPHA_OPAQUE;
        _alphaEnabled = false;
    }

    //
    // Sets new texture as texture
    //
    _texture = texture;

    //
    // Sets the width and height to the width and height of the new texture and
    // and sets the width and height of the rendering rectangle to the width
    // and height of the new texture.
    //
    int width, height;
    SDL_QueryTexture(_texture, nullptr, nullptr, &width, &height);
    _width = width;
    _height = height;
    _renderRect.x = 0;
    _renderRect.y = 0;
    _renderRect.w = width;
    _renderRect.h = height;

    return true;
}

