//
//  oNet.h
//  opengltest
//
//  Created by Ismo Torvinen on 28.4.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef opengltest_oNet_h
#define opengltest_oNet_h

#include <iostream>
#include "olento.h"


//tcp client
#ifdef __APPLE__
#include <SDL2/SDL_net.h>
#else

#endif

class oNet
{
protected:
    char buffer[256];
    IPaddress ip;
    TCPsocket socket;
    int port_n;
    
public:
    oNet ();
    oNet (std::string hostname, int port);
    
};


#endif
