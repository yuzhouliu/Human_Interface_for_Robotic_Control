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
// March 19, 2016
//
//*****************************************************************************
#include "Panel.h"

#include <cassert>
#include <iostream>
#include <cmath>

#include "IPv4Address.h"
#include "LeapMotionManager.h"
#include "Window.h"

//*****************************************************************************
//
//! Constructor for Panel. Acquires resources for renderer and hand model.
//!
//! \param window SDL_Window that the application is running on.
//!
//! \return None.
//
//*****************************************************************************
Panel::Panel(Window *window, SDL_Window *sdlWindow)
    : _window(sdlWindow), _renderer(nullptr), _hand(nullptr),
    _connected(false), _mode(HIRCPPacket::FEEDBACK_MODE::NORMAL_MODE),
    _cachedFPS(0)
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

    //
    // Adds Window as observer to PlaybackStreamer
    //
    _playbackStreamer->addObserver(window);
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
    LeapMotionManager leap;
    LeapData leapData;
    FingerPressureStruct fingerPressures;

    //
    // Main panel logic
    //
    _fpsManager.setFPS(30);
    while (!Window::gExit)
    {
        //
        // Begins tracking fps
        //
        _fpsManager.beginFrame();

        //
        // Clears finger pressures
        //
        for (int i=0; i<NUM_FINGERS; i++)
        {
            fingerPressures.pressure[i] = 0;
        }

        //
        // Fetches relevent data from Leap Motion Controller
        //
        leap.processFrame(leapData);

        //
        // Record data (if applicable)
        //
        _playbackRecorder->update(leapData);

        //
        // Stream data (if applicable)
        //
        _playbackStreamer->update(leapData);

        //
        // Serialize data
        //
        leap.serialize(leapData, leapData.data, leapData._MAX_PAYLOAD);

        if (_connected)
        {
            //
            // Constructs a DATA packet to send over the network
            //
            HIRCPPacket sendPacket = HIRCPPacket::createDATAPacket(
                leapData.data, leapData._MAX_PAYLOAD);

            //
            // Sends DATA packet to remote host
            //
            if (!send(sendPacket))
            {
                _socket->close();
                _connected = false;
                notify(EVENT_DISCONNECTED);
                continue;
            }

            //
            // Constructs an empty packet to receive over the network
            //
            HIRCPPacket recvPacket = HIRCPPacket::createEmptyPacket();

            //
            // Receives DATA from remote host
            //
            if (!recv(recvPacket))
            {
                _socket->close();
                _connected = false;
                notify(EVENT_DISCONNECTED);
                continue;
            }

            //
            // Validate packet and take appropriate measures
            //
            if (!recvPacket.isValid() ||
                (recvPacket.getType() != HIRCPPacket::DACK))
            {
                //
                // TODO (Brandon): Create and send error packet
                //
            }

            //
            // Populates FingerPressureStruct with finger pressure information
            //
            _populateFingerPressureStruct(fingerPressures, recvPacket);
        }

        //
        // Updates model
        //
        _update(leapData, fingerPressures);

        //
        // Updates GUI
        //
        _render();

        //
        // Ends frame and blocks until FPS elapses
        //
        _fpsManager.endFrame();
    }
}

//*****************************************************************************
//
//! Connects to remote host. Synchronized by mutex.
//!
//! \param ipAddress the IPv4 address of the remote host.
//!
//! \return Returns \b true if the connection was established successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Panel::connect(char *ipAddressString)
{
    const int PORT = 5001;

    if (_connected)
    {
        std::cout << "Already connected." << std::endl;
        return false;
    }

    //
    // Constructs an IPv4 address from ip string
    //
    IPv4Address address(ipAddressString, PORT);

    //
    // Locks mutex protecting socket
    //
    std::lock_guard<std::mutex> lock(_socket_mutex);

    //
    // Opens socket
    //
    if (!_socket->open())
    {
        return false;
    }

    //
    // Connects to remote host
    //
    if (!_socket->connect(&address))
    {
        std::cout << "Unable to connect to " << ipAddressString << std::endl;
        return false;
    }
    std::cout << "Connected to " << ipAddressString << std::endl;

    //
    // Constructs a CRQ packet to send to remote host
    //
    HIRCPPacket crqPacket = HIRCPPacket::createCRQPacket(_mode);
    unsigned char message[HIRCPPacket::MAX_PACKET_SIZE];
    crqPacket.getData(message, HIRCPPacket::MAX_PACKET_SIZE);

    std::cout << "Sending packet: ";
    for (int i = 0; i < HIRCPPacket::MAX_PACKET_SIZE; i++)
    {
        std::cout << static_cast<unsigned int>(message[i]) << " ";
    }
    std::cout << std::endl;

    //
    // Sends packet to remote host
    //
    if (!_socket->send(message, HIRCPPacket::MAX_PACKET_SIZE))
    {
        std::cout << "[ERROR] Panel::connect(): Send failed." << std::endl;
        return false;
    }

    //
    // Creates an empty packet to store ACK received from remote host
    //
    HIRCPPacket ackPacket = HIRCPPacket::createEmptyPacket();

    //
    // Receives packet from remote host
    //
    if (!_socket->recv(message, HIRCPPacket::MAX_PACKET_SIZE))
    {
        std::cout << "[ERROR] Panel::connect(): Receive failed." << std::endl;
        return false;
    }

    std::cout << "Receiving packet: ";
    for (int i = 0; i < HIRCPPacket::MAX_PACKET_SIZE; i++)
    {
        std::cout << static_cast<unsigned int>(message[i]) << " ";
    }
    std::cout << std::endl;

    //
    // Populates packet with received data
    //
    ackPacket.populate(message, HIRCPPacket::MAX_PACKET_SIZE);

    if (!ackPacket.isValid() || (ackPacket.getType() != HIRCPPacket::ACK))
    {
        //
        // Packet received was not an ACK packet so terminate connection
        //
        _socket->close();

        std::cout << "Connection rejected." << std::endl;
        std::cout << "Disconnected from socket." << std::endl;
    }

    _connected = true;
    return true;
}

//*****************************************************************************
//
//! Terminates connection to remote host. Synchronized by mutex.
//!
//! \param None.
//!
//! \return Returns \b true if the connection was closed successfully and \b
//! false otherwise.
//
//*****************************************************************************
bool Panel::disconnect()
{
    if (_connected)
    {
        //
        // Locks mutex protecting socket
        //
        std::lock_guard<std::mutex> lock(_socket_mutex);

        //
        // Constructs a TRQ packet to send to remote host
        //
        HIRCPPacket trqPacket = HIRCPPacket::createTRQPacket();
        unsigned char message[HIRCPPacket::MAX_PACKET_SIZE];
        trqPacket.getData(message, HIRCPPacket::MAX_PACKET_SIZE);

        std::cout << "Sending packet: ";
        for (int i = 0; i < HIRCPPacket::MAX_PACKET_SIZE; i++)
        {
            std::cout << static_cast<unsigned int>(message[i]) << " ";
        }
        std::cout << std::endl;

        //
        // Sends packet to remote host
        //
        if (!_socket->send(message, HIRCPPacket::MAX_PACKET_SIZE))
        {
            std::cout << "[ERROR] Panel::disconnect(): Send failed." <<
                std::endl;
            return false;
        }

        //
        // Creates an empty packet to store ACK received from remote host
        //
        HIRCPPacket ackPacket = HIRCPPacket::createEmptyPacket();

        //
        // Receives packet from remote host
        //
        if (!_socket->recv(message, HIRCPPacket::MAX_PACKET_SIZE))
        {
            std::cout << "[ERROR] Panel::Disconnect(): Receive failed." <<
                std::endl;
            return false;
        }

        std::cout << "Receiving packet: ";
        for (int i = 0; i < HIRCPPacket::MAX_PACKET_SIZE; i++)
        {
            std::cout << static_cast<unsigned int>(message[i]) << " ";
        }
        std::cout << std::endl;

        //
        // Populates packet with received data
        //
        ackPacket.populate(message, HIRCPPacket::MAX_PACKET_SIZE);

        if (!ackPacket.isValid() || (ackPacket.getType() != HIRCPPacket::ACK))
        {
            //
            // Packet received was not an ACK packet so terminate connection
            //
            std::cout << "Safe disconnection failed... forcing disconnect" <<
                std::endl;
        }

        _socket->close();
        _connected = false;

        std::cout << "Disconnected from socket." << std::endl;

        return true;
    }
    std::cout << "Not connected." << std::endl;
    return false;
}

//*****************************************************************************
//
//! Sends data to remote host. Synchronized by mutex.
//!
//! \param packet HIRCP packet with data to send.
//!
//! \return Returns \b true if the packet was sent successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool Panel::send(HIRCPPacket &packet)
{
    if (_connected)
    {
        //
        // Locks mutex protecting socket
        //
        std::lock_guard<std::mutex> lock(_socket_mutex);
        
        //
        // Get packet data
        //
        unsigned char message[HIRCPPacket::MAX_PACKET_SIZE];
        packet.getData(message, HIRCPPacket::MAX_PACKET_SIZE);

        std::cout << "Sending packet: ";
        for (int i=0; i<HIRCPPacket::MAX_PACKET_SIZE; i++)
        {
            std::cout << static_cast<unsigned int>(message[i]) << " ";
        }
        std::cout << std::endl;

        //
        // Send packet data to remote host
        //
        if (!_socket->send(message, HIRCPPacket::MAX_PACKET_SIZE))
        {
            std::cout << "[ERROR] Panel::send(): Send failed." << std::endl;
            return false;
        }

        return true;
    }

    std::cout << "[WARNING] Panel::send(): Not connected." << std::endl;
    return false;
}

//*****************************************************************************
//
//! Receives data from remote host. Synchronized by mutex.
//!
//! \param packet HIRCP packet to hold received data.
//!
//! \return Returns \b true if the packet was received successfully and \b
//! false otherwise.
//
//*****************************************************************************
bool Panel::recv(HIRCPPacket &packet)
{
    if (_connected)
    {
        //
        // Locks mutex protecting socket
        //
        std::lock_guard<std::mutex> lock(_socket_mutex);

        //
        // Buffer to store packet data
        //
        unsigned char message[HIRCPPacket::MAX_PACKET_SIZE];

        //
        // Receives packet from remote host
        //
        if (!_socket->recv(message, HIRCPPacket::MAX_PACKET_SIZE))
        {
            std::cout << "[ERROR] Panel::recv(): Receive failed." << std::endl;
            return false;
        }

        //
        // Populates packet with received data
        //
        packet.populate(message, HIRCPPacket::MAX_PACKET_SIZE);

        std::cout << "Receiving packet: ";
        for (int i=0; i<HIRCPPacket::MAX_PACKET_SIZE; i++)
        {
            std::cout << static_cast<unsigned int>(message[i]) << " ";
        }
        std::cout << std::endl;

        return true;
    }

    std::cout << "[WARNING] Panel::recv(): Not connected." << std::endl;
    return false;
}

//*****************************************************************************
//
//! Starts recording data stream to file.
//!
//! \param filePath path to file to save data stream to.
//!
//! \return Returns \b true if recording started successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool Panel::startRecording(char *filePath)
{
    if (_playbackStreamer->isStreaming())
    {
        std::cout << "[ERROR] Panel::startRecording(): Cannot record while "\
            "streaming." << std::endl;
        return false;
    }

    if (!_playbackRecorder->startRecording(filePath, _fpsManager.getFPS()))
    {
        return false;
    }

    return true;
}

//*****************************************************************************
//
//! Stops recording data stream and save file.
//!
//! \param None.
//!
//! \return Returns \b true if recording stopped successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool Panel::stopRecording()
{
    if (_playbackStreamer->isStreaming())
    {
        std::cout << "[ERROR] Panel::stopRecording(): Recording not started."\
            << std::endl;
        return false;
    }

    if (!_playbackRecorder->stopRecording())
    {
        return false;
    }

    return true;
}

//*****************************************************************************
//
//! Starts streaming data from file.
//!
//! \param filePath path to file to stream data from.
//!
//! \return Returns \b true if streaming started successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool Panel::startStreaming(char *filePath)
{
    if (_playbackRecorder->isRecording())
    {
        std::cout << "[ERROR] Panel::startStreaming(): Cannot stream while "\
            "recording." << std::endl;
        return false;
    }

    //
    // Save the current FPS and start streaming with recorded FPS
    //
    _cachedFPS = _fpsManager.getFPS();
    if (!_playbackStreamer->startStreaming(filePath))
    {
        return false;
    }
    _fpsManager.setFPS(_playbackStreamer->getStreamingFPS());

    return true;
}

//*****************************************************************************
//
//! Stops streaming data.
//!
//! \param None.
//!
//! \return Returns \b true if streaming stopped successfully and \b false
//! otherwise.
//
//*****************************************************************************
bool Panel::stopStreaming()
{
    if (_playbackRecorder->isRecording())
    {
        std::cout << "[ERROR] Panel::stopStreaming(): Streaming not started."\
            << std::endl;
        return false;
    }

    if (!_playbackStreamer->stopStreaming())
    {
        return false;
    }

    //
    // Reset FPS to cached value
    //
    _fpsManager.setFPS(_cachedFPS);
    _cachedFPS = 0;

    return true;
}

//*****************************************************************************
//
//! Sets feedback mode.
//!
//! \param mode the FEEDBACK_MODE to set.
//!
//! \return Returns \b true if mode set successfully and \b false otherwise.
//
//*****************************************************************************
bool Panel::setFeedbackMode(HIRCPPacket::FEEDBACK_MODE mode)
{
    if (mode == HIRCPPacket::FEEDBACK_MODE::NORMAL_MODE
        && _mode != HIRCPPacket::FEEDBACK_MODE::NORMAL_MODE)
    {
        if (_connected)
        {
            HIRCPPacket modePacket = HIRCPPacket::createMODEPacket(mode);
            if (!send(modePacket))
            {
                _socket->close();
                _connected = false;
                notify(EVENT_DISCONNECTED);
                return false;
            }
        }
        _mode = HIRCPPacket::FEEDBACK_MODE::NORMAL_MODE;
        return true;
    }
    else if (mode == HIRCPPacket::FEEDBACK_MODE::CLOSED_LOOP_MODE
        && _mode != HIRCPPacket::FEEDBACK_MODE::CLOSED_LOOP_MODE)
    {
        if (_connected)
        {
            HIRCPPacket modePacket = HIRCPPacket::createMODEPacket(mode);
            if (!send(modePacket))
            {
                _socket->close();
                _connected = false;
                notify(EVENT_DISCONNECTED);
                return false;
            }
        }
        _mode = HIRCPPacket::FEEDBACK_MODE::CLOSED_LOOP_MODE;
        return true;
    }
    std::cout << "[ERROR] Panel::setFeedbackMode(): Feedback mode could not "\
        "be set!" << std::endl;
    return false;
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
    // Sets renderer to default to an Opaque black color on screen clear
    //
    iRetVal = SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00,
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
    _hand = std::unique_ptr<Hand>(new Hand(_renderer));

    //
    // Creates socket
    //
    _socket = std::unique_ptr<TCPSocket>(new TCPSocket());

    //
    // Creates playback recorder and streamr
    //
    _playbackStreamer = std::unique_ptr<PlaybackStreamer>(
        new PlaybackStreamer(_window));
    _playbackRecorder = std::unique_ptr<PlaybackRecorder>(
        new PlaybackRecorder(_window));

    return true;
}

//*****************************************************************************
//
//! Updates program logic. Called once per frame by run().
//!
//! \param leapData a structure containing leap data information.
//! \param fingerPressures a structure containing finger pressure information.
//!
//! \return None.
//
//*****************************************************************************
void Panel::_update(LeapData &leapData,
    FingerPressureStruct &fingerPressures)
{
    _hand->update(leapData, fingerPressures);
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
    _playbackRecorder->render();
    _playbackStreamer->render();

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
//! \param packet HIRCP packet with finger pressure data.
//!
//! \return Returns \b true if the structure was populated successfully and
//! \b false otherwise.
//
//*****************************************************************************
bool Panel::_populateFingerPressureStruct(FingerPressureStruct
    &fingerPressures, HIRCPPacket packet)
{
    const int BITS_PER_BYTE = 8;
    //const int MIN_ENCODED_PRESSURE = 800;
    const int MAX_ENCODED_PRESSURE[NUM_FINGERS] =
        { 1020, 800, 930, 1030, 990 };
    const float SENSOR_SCALING = 3.05;

    //
    // Buffer needs to be at least this size
    //
    int pressureSize = sizeof(unsigned short);
    const unsigned int buflen = packet.MAX_PAYLOAD_LEN;
    unsigned char buf[buflen];
    packet.getPayload(buf, buflen);
    assert(buflen >= (unsigned int)pressureSize*NUM_FINGERS);

    //
    // Parses through buf and populate structure
    //
    unsigned int bufIndex = 0;
    for (int i=0; i<NUM_FINGERS; i++)
    {
        unsigned short encodedPressure = 0;
        encodedPressure += buf[bufIndex++];
        encodedPressure <<= BITS_PER_BYTE;
        encodedPressure += buf[bufIndex++];
        assert((encodedPressure >= 0) && (encodedPressure <= 4096));
		
        // Setting encodedPressure if limit exceeded
        if (encodedPressure > MAX_ENCODED_PRESSURE[i])
        {
            encodedPressure = MAX_ENCODED_PRESSURE[i];
        }
        double multiplier = (double)(log10((double)(MAX_ENCODED_PRESSURE[i]
            - encodedPressure + 1))) / SENSOR_SCALING;

        fingerPressures.pressure[i] =
            static_cast<unsigned char>(multiplier*255);
        /*std::cout << "pressure = "
            << static_cast<unsigned int>(fingerPressures.pressure[i])
            << std::endl;*/
    }

    return true;
}
