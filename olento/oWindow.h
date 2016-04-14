//oWindow.h
#ifndef OWINDOW_H
#define OWINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>

namespace oWindow{

	void init();
	void show();
	bool getCloseEvent();
	void close();
}

#endif