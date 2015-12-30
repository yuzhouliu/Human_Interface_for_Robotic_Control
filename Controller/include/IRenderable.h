//*****************************************************************************
//
// IRenderable.h
//
// Class responsible for providing interface for objects renderable by a
// renderer.
//
// Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
// This code is licensed under BSD license (see LICENSE.txt for details)
//
// Created:
// December 28, 2015
//
// Modified:
// December 28, 2015
//
//*****************************************************************************
#ifndef _IRENDERABLE_H_
#define _IRENDERABLE_H_

class IRenderable
{
private:
    /* Fields */

public:
    /* Destructor */
    virtual ~IRenderable(){};

    /* Methods */
    virtual void onRender() = 0;
};

#endif /* _IRENDERABLE_H_ */
