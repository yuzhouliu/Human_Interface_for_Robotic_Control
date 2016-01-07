//*****************************************************************************
//
// Panel.cpp
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
// January 5, 2016
//
//*****************************************************************************
#include "Panel.h"

#include <cassert>
#include <iostream>

#include "FrameRateManager.h"
#include "IPv4Address.h"
#include "LeapMotionManager.h"
#include "Network.h"
#include "TCPSocket.h"
#include "Window.h"

//*****************************************************************************
//
//! Constructor for Panel. Acquires resources for renderer and hand model.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Panel::Panel(SDL_Window *window)
    : _window(window), _renderer(nullptr)
{
    //
    // Initialize panel
    //
    if (!_initialize())
    {
        std::cerr << "[ERROR] Panel::Panel(): Failed to initialize." <<
            std::endl;
        return;
    }
}

//*****************************************************************************
//
//! Destructor for Panel. Releases resources used by renderer and hand model.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
Panel::~Panel()
{
    if (_renderer != nullptr)
    {
        SDL_DestroyRenderer(_renderer);
    }
}

//*****************************************************************************
//
//! Starts the panel. MUST BE CALLED FROM A DIFFERENT THREAD THAN THE WINDOW
//! THREAD.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Panel::run()
{
    FrameRateManager fpsManager;
    LeapMotionManager leap;
    const unsigned short _MAX_PAYLOAD = 256;
    unsigned char data[_MAX_PAYLOAD];
    FingerPressureStruct fingerPressures;

    //
    // Receive IPv4 address and port as input from user
    //
    std::cout << "Enter the target IPv4 address: ";
    char addressInput[16];
    std::cin >> addressInput;

    std::cout << "Enter the target port: ";
    unsigned short port;
    std::cin >> port;

    //
    // Constructs an IPv4 address using user input
    //
    IPv4Address address(addressInput, port);

    //
    // Constructs a TCP socket and connects to IPv4 address
    //
    TCPSocket tcpsocket;
    tcpsocket.open();
    tcpsocket.connect(&address);

    //
    // Main panel logic
    //
    fpsManager.setFPS(5);
    while (!Window::gExit)
    {
        //
        // Begins tracking fps
        //
        fpsManager.beginFrame();

        //
        // Fetches relevent data from Leap Motion Controller
        //
        leap.processFrame(data, _MAX_PAYLOAD);

        //
        // Send data to remote host
        //
        tcpsocket.send(data, _MAX_PAYLOAD);

        //
        // Receive data from remote host
        //
        tcpsocket.recv(data, _MAX_PAYLOAD);

        //
        // Populates FingerPressureStruct with finger pressure information
        // TODO (Brandon): As of now, this populates structure with angle
        // information. Must change to pressure information once we establish
        // communication to microcontroller and receive feedback from sensors.
        //
        _populateFingerPressureStruct(fingerPressures, data, _MAX_PAYLOAD);

        //
        // Updates model
        //
        _update(&fingerPressures);

        //
        // Updates GUI
        //
        _render();

        //
        // Ends frame and blocks until FPS elapses
        //
        fpsManager.endFrame();
    }
}

//*****************************************************************************
//
//! Initializes the panel.
//!
//! \param None.
//!
//! \return Returns \b true if the panel was initialized successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Panel::_initialize()
{
    int iRetVal = 0;

    //
    // Get the width and height 
    //
    int width, height;
    SDL_GetWindowSize(_window, &width, &height);

    //
    // Creates renderer
    //
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_TARGETTEXTURE);
    if (_renderer == nullptr)
    {
        std::cerr << "[ERROR] Panel::_initialize(): SDL_CreateRenderer() "\
            "failed. SDL Error " << SDL_GetError() << std::endl;
        return false;
    }

    //
    // Sets renderer to default to an Opqaue White color on screen clear
    //
    iRetVal = SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF,
        SDL_ALPHA_OPAQUE);
    if (iRetVal < 0)
    {
        std::cerr << "[ERROR] Panel::_initialize(): SDL_SetRenderDrawColor()"\
            " failed. SDL Error " << SDL_GetError() << std::endl;
        return false;
    }

    //
    // Sets a device independent resolution for rendering
    //
    iRetVal = SDL_RenderSetLogicalSize(_renderer, width, height);
    if (iRetVal < 0)
    {
        std::cerr << "[ERROR] Panel::_initialize(): "\
            "SDL_RenderSetLogicalSize() failed. SDL Error " << SDL_GetError()
            << std::endl;
        return false;
    }

    //
    // Creates hand model
    //
    _hand = std::unique_ptr<Hand>(new Hand(_renderer, width, height));

    return true;
}

//*****************************************************************************
//
//! Updates program logic. Called once per frame by run().
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Panel::_update(FingerPressureStruct *fingerPressures)
{
    _hand->update(fingerPressures);
}

//*****************************************************************************
//
//! Renders frame. Called once per frame by run().
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void Panel::_render()
{
    //
    // Clears screen
    //
    SDL_RenderClear(_renderer);

    //
    // Renders all images to screen
    //
    _hand->render();

    //
    // Updates screen (swaps screen buffers)
    //
    SDL_RenderPresent(_renderer);
}

//*****************************************************************************
//
//! Populates structure with pressure information decoded from buf.
//!
//! \param fingerPressures structure for storing results.
//! \param buf buffer with encoded pressure data.
//! \param buflen the size of the buffer.
//!
//! \return Returns \b true if the structure was populated successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Panel::_populateFingerPressureStruct(FingerPressureStruct
    &fingerPressures, unsigned char *buf, unsigned int buflen)
{
    //
    // Buffer needs to be at least this size
    //
    const int BITS_PER_BYTE = 8;
    //int pressureSize = sizeof(fingerPressures.pressure[0]);
    int pressureSize = sizeof(unsigned short);
    assert(buflen >= pressureSize*NUM_FINGERS);

    //
    // Parses through buf and populate structure
    //
    unsigned int bufIndex = 0;
    for (int i=0; i<NUM_FINGERS; i++)
    {
        //fingerPressures.pressure[i] = buf[bufIndex++];
        unsigned short encodedPressure = 0;
        encodedPressure += buf[bufIndex++];
        encodedPressure <<= BITS_PER_BYTE;
        encodedPressure += buf[bufIndex++];
        assert((encodedPressure >= 0) && (encodedPressure <= 4096));
        float multiplier = (float)(1) - (float)(encodedPressure)/4096;
        fingerPressures.pressure[i] = static_cast<unsigned char>(multiplier*255);
        std::cout << "pressure = " << static_cast<unsigned int>(fingerPressures.pressure[i]) << std::endl;
    }

    return true;
}
