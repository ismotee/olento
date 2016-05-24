//oWindow.h
#ifndef OWINDOW_H
#define OWINDOW_H

//include GL
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <glew.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>

namespace oWindow{

	void init(int window_w = 500, int window_h = 500);
	void show();
	bool getCloseEvent();
	void close();
}

#endif