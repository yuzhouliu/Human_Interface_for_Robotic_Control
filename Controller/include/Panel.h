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
// March 3, 2016
//
//*****************************************************************************
#ifndef _PANEL_H_
#define _PANEL_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <mutex>
#include <windows.h>

#include <SDL.h>

#include "FrameRateManager.h"
#include "Hand.h"
#include "HIRCP.h"
#include "IObservable.h"
#include "Network.h"
#include "PlaybackRecorder.h"
#include "PlaybackStreamer.h"
#include "TCPSocket.h"

class Window;

class Panel
{
private:
    /* Fields */
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    std::unique_ptr<Hand> _hand;
    std::unique_ptr<TCPSocket> _socket;
    std::mutex _socket_mutex;
    FrameRateManager _fpsManager;
    std::unique_ptr<PlaybackRecorder> _playbackRecorder;
    std::unique_ptr<PlaybackStreamer> _playbackStreamer;
    bool _connected;
    int _cachedFPS;

    /* Methods */
    bool _initialize();
    void _update(LeapData &leapData,
        FingerPressureStruct &fingerPressures);
    void _render();
    bool _populateFingerPressureStruct(FingerPressureStruct &fingerPressures,
        HIRCPPacket packet);

public:
    /* Constructor */
    Panel(Window *window, SDL_Window *sdlWindow);

    /* Destructor */
    ~Panel();

    /* Methods */
    void run();
    bool connect(char *ipAddressString);
    bool disconnect();
    bool send(HIRCPPacket &packet);
    bool recv(HIRCPPacket &packet);
    bool startRecording(char *filePath);
    bool stopRecording();
    bool startStreaming(char *filePath);
    bool stopStreaming();
};

#endif /* _PANEL_H_ */
