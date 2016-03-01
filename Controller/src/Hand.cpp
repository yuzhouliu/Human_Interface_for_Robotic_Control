//*****************************************************************************
//
// Hand.cpp
//
// Class responsible for modelling a hand.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 29, 2015
//
// Modified:
// Feburary 29, 2016
//
//*****************************************************************************
#include "Hand.h"

#include <SDL_ttf.h>

#include "Window.h"

//*****************************************************************************
//
//! Constructor for Hand. Acquires resources for finger models.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Hand::Hand(SDL_Renderer *renderer)
    : _renderer(renderer)
{
    SDL_Rect renderRect;

    //
    // Creates base hand image
    //
    _staticImage = std::unique_ptr<Image>(new Image(renderer,
        "data/gfx/hand_right_base.png"));

    //
    // Creates and add fingers to list
    //
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::THUMB, renderer)));
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::INDEX, renderer)));
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::MIDDLE, renderer)));
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::RING, renderer)));
    _fingerList.push_back(std::unique_ptr<Finger>(
        new Finger(FingerType::PINKY, renderer)));

    //
    // Centre all images for aesthetic purposes
    //
    /*Image::centreImage(_staticImage, PRIMARY_VIEWPORT_WIDTH,
        PRIMARY_VIEWPORT_HEIGHT);
    for (auto it = _fingerList.begin(); it != _fingerList.end(); it++)
    {
        Image::centreImage((*it)->getStaticImage(), PRIMARY_VIEWPORT_WIDTH,
            PRIMARY_VIEWPORT_HEIGHT);
    }*/

    renderRect.x = 0; renderRect.y = 0;
    renderRect.w = SECONDARY_VIEWPORT_WIDTH;
    renderRect.h = SECONDARY_VIEWPORT_HEIGHT;
    _staticImage->setRenderRect(renderRect);
    for (auto it = _fingerList.begin(); it != _fingerList.end(); it++)
    {
        (*it)->getStaticImage()->setRenderRect(renderRect);
    }

    TTF_Font *font = TTF_OpenFont("data/font/kenvector_future_thin.ttf", 20);
    if (font == nullptr)
    {
        std::cout << "[ERROR] Hand::Hand(): Font could not be loaded." <<
            std::endl;
        return;
    }
    SDL_Color color = {0xFF, 0xFF, 0xFF};

    SDL_Surface *thumbTextSurface = TTF_RenderText_Blended(font, "Thumb =",
        color);
    SDL_Surface *indexTextSurface = TTF_RenderText_Blended(font, "Index =",
        color);
    SDL_Surface *middleTextSurface = TTF_RenderText_Blended(font, "Middle =",
        color);
    SDL_Surface *ringTextSurface = TTF_RenderText_Blended(font, "Ring =",
        color);
    SDL_Surface *pinkyTextSurface = TTF_RenderText_Blended(font, "Pinky =",
        color);
    SDL_Surface *wristTextSurface = TTF_RenderText_Blended(font, "Wrist =",
        color);
    if ((thumbTextSurface == nullptr) || (indexTextSurface == nullptr) ||
        (middleTextSurface == nullptr) || (ringTextSurface == nullptr) ||
        (pinkyTextSurface == nullptr) || (wristTextSurface == nullptr))
    {
        std::cout << "[ERROR] Hand::Hand(): Surface could not be created." <<
            std::endl;
        return;
    }

    _thumbText = std::unique_ptr<Image>(new Image());
    _thumbText->setRenderer(renderer);
    _thumbText->setTexture(thumbTextSurface);
    renderRect.x = 0; renderRect.y = 0; renderRect.w = 130; renderRect.h = 50;
    _thumbText->setRenderRect(renderRect);

    _indexText = std::unique_ptr<Image>(new Image());
    _indexText->setRenderer(renderer);
    _indexText->setTexture(indexTextSurface);
    renderRect.x = 0; renderRect.y = 50; renderRect.w = 130; renderRect.h = 50;
    _indexText->setRenderRect(renderRect);

    _middleText = std::unique_ptr<Image>(new Image());
    _middleText->setRenderer(renderer);
    _middleText->setTexture(middleTextSurface);
    renderRect.x = 0; renderRect.y = 100; renderRect.w = 130; renderRect.h = 50;
    _middleText->setRenderRect(renderRect);

    _ringText = std::unique_ptr<Image>(new Image());
    _ringText->setRenderer(renderer);
    _ringText->setTexture(ringTextSurface);
    renderRect.x = 0; renderRect.y = 150; renderRect.w = 130; renderRect.h = 50;
    _ringText->setRenderRect(renderRect);

    _pinkyText = std::unique_ptr<Image>(new Image());
    _pinkyText->setRenderer(renderer);
    _pinkyText->setTexture(pinkyTextSurface);
    renderRect.x = 0; renderRect.y = 200; renderRect.w = 130; renderRect.h = 50;
    _pinkyText->setRenderRect(renderRect);

    _wristText = std::unique_ptr<Image>(new Image());
    _wristText->setRenderer(renderer);
    _wristText->setTexture(wristTextSurface);
    renderRect.x = 0; renderRect.y = 250; renderRect.w = 130; renderRect.h = 50;
    _wristText->setRenderRect(renderRect);

    SDL_FreeSurface(thumbTextSurface);
    SDL_FreeSurface(indexTextSurface);
    SDL_FreeSurface(middleTextSurface);
    SDL_FreeSurface(ringTextSurface);
    SDL_FreeSurface(pinkyTextSurface);
    SDL_FreeSurface(wristTextSurface);

    for (int i = 0; i < 256; i++)
    {
        char buf[32 + 1];
        SDL_itoa(i, buf, 10);
        SDL_Surface *tempSurface = TTF_RenderText_Blended(font, buf, color);
        if (tempSurface == nullptr)
        {
            std::cout << "[ERROR] Hand::Hand(): Surface could not be created."
                << std::endl;
            return;
        }
        _numberTexts.push_back(new Image());
        _numberTexts[i]->setRenderer(renderer);
        _numberTexts[i]->setTexture(tempSurface);
        SDL_FreeSurface(tempSurface);
    }

    TTF_CloseFont(font);
}

//*****************************************************************************
//
//! Destructor for Hand.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Hand::~Hand()
{

}

//*****************************************************************************
//
//! Updates hand with finger pressure information.
//!
//! \param leapData a structure containing leap data information.
//! \param fingerPressures a structure containing finger pressure information.
//!
//! \return None.
//
//*****************************************************************************
void Hand::update(LeapData &leapData,
    FingerPressureStruct &fingerPressures)
{
    //
    // Updates hand image
    //
    if (leapData.imageAvailable)
    {
        SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
            (void*)(leapData.imageDataRGBA), leapData.imageWidth,
            leapData.imageHeight, leapData.imageDepth, leapData.imagePitch,
            0x00, 0x00, 0x00, 0x00);
        if (surface == nullptr)
        {
            std::cout << "[ERROR] Hand::update(): " << SDL_GetError()
                << std::endl;
            exit(-1);
        }

        _image = std::unique_ptr<Image>(new Image());
        _image->setRenderer(_renderer);
        _image->setTexture(surface);
        _image->setRenderRect(leapData.imageRenderRect);

        SDL_FreeSurface(surface);

        for (int i=0; i<NUM_FINGERS; i++)
        {
            _fingerList[i]->setRenderRect(leapData.fingerRects[i]);
        }
    }

    //
    // Updates finger pressures
    //
    for (int i=0; i<NUM_FINGERS; i++)
    {
        FingerType type = static_cast<FingerType>(i);
        auto pressureToSet = fingerPressures.pressure[i];
        _fingerList[+type]->setPressure(pressureToSet);
    }

    //
    // Updates recent angles
    //
    for (int i=0; i<NUM_FINGERS; i++)
    {
        _recentAngles[i] = leapData.totalAngle[i];
    }
    _recentAngles[NUM_FINGERS] = leapData.wristAngle;
}

//*****************************************************************************
//
//! Renders hand.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Hand::render()
{
    //
    // Renders all images to screen
    //
    SDL_RenderSetViewport(_renderer, &Window::gSecondaryViewport);
    _staticImage->onRender();

    if (_image != nullptr)
    {
        SDL_RenderSetViewport(_renderer, &Window::gPrimaryViewport);
        _image->onRender();
    }

    for (auto it = _fingerList.begin(); it != _fingerList.end(); it++)
    {
        (*it)->render();
    }

    SDL_Rect renderRect;
    SDL_RenderSetViewport(_renderer, &Window::gTertiaryViewport);

    _thumbText->onRender();
    renderRect.x = 130; renderRect.y = 0; renderRect.w = 50; renderRect.h = 50;
    _numberTexts[_recentAngles[0]]->setRenderRect(renderRect);
    _numberTexts[_recentAngles[0]]->onRender();

    _indexText->onRender();
    renderRect.x = 130; renderRect.y = 50; renderRect.w = 50; renderRect.h = 50;
    _numberTexts[_recentAngles[1]]->setRenderRect(renderRect);
    _numberTexts[_recentAngles[1]]->onRender();

    _middleText->onRender();
    renderRect.x = 130; renderRect.y = 100; renderRect.w = 50; renderRect.h = 50;
    _numberTexts[_recentAngles[2]]->setRenderRect(renderRect);
    _numberTexts[_recentAngles[2]]->onRender();

    _ringText->onRender();
    renderRect.x = 130; renderRect.y = 150; renderRect.w = 50; renderRect.h = 50;
    _numberTexts[_recentAngles[3]]->setRenderRect(renderRect);
    _numberTexts[_recentAngles[3]]->onRender();

    _pinkyText->onRender();
    renderRect.x = 130; renderRect.y = 200; renderRect.w = 50; renderRect.h = 50;
    _numberTexts[_recentAngles[4]]->setRenderRect(renderRect);
    _numberTexts[_recentAngles[4]]->onRender();

    _wristText->onRender();
    renderRect.x = 130; renderRect.y = 250; renderRect.w = 50; renderRect.h = 50;
    _numberTexts[_recentAngles[5]]->setRenderRect(renderRect);
    _numberTexts[_recentAngles[5]]->onRender();
}