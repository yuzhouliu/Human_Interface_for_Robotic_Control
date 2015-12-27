//*****************************************************************************
//
// Image.h
//
// Class responsible for loading, managing, and closing textures.
//
// Created:
// December 27, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>

#include <SDL.h>

class Image
{
private:
    /* Fields */
    SDL_Texture *texture;
    SDL_Rect renderRect;
    unsigned short width;
    unsigned short height;
    unsigned short angle;
    unsigned char alpha;
    bool alphaEnabled;

public:
    /* Constructor */
    Image();

    /* Destructor */
    ~Image();

    /* Methods */
    bool setTexture(std::string path, SDL_Renderer *renderer);
    bool setTexture(SDL_Texture *texture);
    SDL_Texture *getTexture();
    unsigned short getWidth();
    unsigned short getHeight();
    void setRenderRect(SDL_Rect *renderRect);
    SDL_Rect getRenderRect();
    void setAngle(unsigned short angle);
    unsigned short getAngle();
    void enableAlphaBlend();
    void disableAlphaBlend();
    void setAlphaBlend(unsigned char alpha);
    unsigned char getAlphaBlend();
};

#endif /* _IMAGE_H_ */
