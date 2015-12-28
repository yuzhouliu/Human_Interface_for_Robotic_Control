//*****************************************************************************
//
// IObserver.h
//
// Class responsible for providing interface to implement Observer pattern.
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
#ifndef _IOBSERVER_H_
#define _IOBSERVER_H_

#include <vector>

class IObserver
{
private:
    /* Fields */

public:
    /* Destructor */
    virtual ~IObserver(){};

    /* Methods */
    virtual void onNotify(int event) = 0;
};

class IObservable
{
private:
    /* Fields */
    std::vector<IObserver*> _observers;

public:
    /* Destructor */
    virtual ~IObservable(){};

    /* Methods */
    virtual void addObserver(IObserver *observer)
    {
        _observers.push_back(observer);
    };

    virtual void removeObserver(IObserver *observer)
    {
        _observers.erase(std::find(_observers.begin(), _observers.end(),
            observer));
    };

protected:
    /* Methods */
    virtual void notify(int event)
    {
        for (auto iterator = _observers.begin(); iterator != _observers.end();
            iterator++)
        {
            (*iterator)->onNotify(event);
        }
            
    };
};

#endif /* _IOBSERVER_H_ */
