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
#include <string>

#include "glm/glm.hpp"

namespace oWindow{

	void init(int window_w = 500, int window_h = 500);
	void show();
	bool getCloseEvent();
	void close();
}

class xyPalette{
protected:
	std::vector<glm::vec3> colors;
	int w = 0;
	int h = 0;
	int size = 0;

	bool init();
public:
	bool loadFromFile(std::string filename);
	glm::vec3 getColor(float x, float y);
	void tulosta();
};


#endif