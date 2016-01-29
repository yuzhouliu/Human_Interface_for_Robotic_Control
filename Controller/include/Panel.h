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
// January 29, 2016
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

#include "Hand.h"
#include "IObservable.h"
#include "Network.h"
#include "TCPSocket.h"

class Panel
{
private:
    /* Fields */
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    std::unique_ptr<Hand> _hand;
    std::unique_ptr<TCPSocket> _socket;
    std::mutex _socket_mutex;
    bool _connected;
    const std::string _addressFilePath = "data/cache/IPAddresses.txt";

    /* Methods */
    bool _initialize();
    void _update(FingerPressureStruct *fingerPressures);
    void _render();
    bool _populateFingerPressureStruct(FingerPressureStruct &fingerPressures,
        unsigned char *buf, unsigned int buflen);
    bool _saveIPAddress(std::string ipAddress);

public:
    /* Constructor */
    Panel(SDL_Window *window);

    /* Destructor */
    ~Panel();

    /* Methods */
    void run();
    bool connect(char *ipAddressString);
    bool disconnect();
    bool send(unsigned char *message, unsigned short len);
    bool recv(unsigned char *message, unsigned short len);
    BOOL CALLBACK ConnectDlgProc(HWND hwnd, UINT msg, WPARAM wParam,
        LPARAM lParam);

    /* Static methods */
    static BOOL CALLBACK DlgProcRouter(HWND hwnd, UINT msg, WPARAM wParam,
        LPARAM lParam);
};

#endif /* _PANEL_H_ */
