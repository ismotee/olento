//
//  oNet.cpp
//  opengltest
//
//  Created by Ismo Torvinen on 28.4.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#include <stdio.h>
#include "oNet.h"

oNet::oNet (): port_n(0)
{
    SDLNet_Init();

}

oNet::oNet (std::string hostname, int port): port_n(port)
{
    SDLNet_Init();
    
    if(SDLNet_ResolveHost(&ip, hostname.c_str(), port_n) == -1)
    {
        std::cout << "error: " << SDLNet_GetError() << "\n";
    }

    socket = SDLNet_TCP_Open(&ip);
    
}