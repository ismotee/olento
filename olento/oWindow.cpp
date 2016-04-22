#include "olento.h"
#include "oWindow.h"
#include <iostream>
SDL_Window* window;
SDL_GLContext context;

void oWindow::init(int window_w, int window_h) {

	//setup SDL context:  initialize SDL for video
	SDL_Init(SDL_INIT_VIDEO);
    
#ifdef __APPLE__
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
#endif
	//Multisample
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	//create SDL window and context
	window = SDL_CreateWindow("OpenGL", 100, 100, window_w, window_h, SDL_WINDOW_OPENGL);
	if (window == NULL)        printf("Couldn't create window! %s\n", SDL_GetError());

	context = SDL_GL_CreateContext(window);
	if (context == NULL)    printf("Error creating context! %s\n", SDL_GetError());

	//windows initti
#ifndef __APPLE__
	//setup glew
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));

#endif

	glEnable(GL_MULTISAMPLE);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	std::cout << "Window init " << SDL_GetError() << "\n";

}


void oWindow::show() {
	SDL_GL_SwapWindow(window);
}



bool oWindow::getCloseEvent() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			return true;
	}

	return false;
}


void oWindow::close() {
	SDL_GL_DeleteContext(context);
	SDL_Quit();
}

