// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <glew.h>

// Include SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "objloader.hpp"

float randf(float min, float max) {
	return (((float)rand() / RAND_MAX)*(max - min) + min);
}

int main(int argc, char* argv[])
{
	//Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);

	//create SDL window and context
	SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 640, 480, SDL_WINDOW_OPENGL);
	if (window == NULL)		printf("Couldn't create window! %s\n", SDL_GetError());

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL)	printf("Error creating context! %s\n", SDL_GetError());

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		return -1;
	}

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	//VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Read our .obj file
	std::vector< glm::vec3 > vertices;
	std::vector<unsigned int> elements;
	std::vector< glm::vec3 > normals;

	bool res = loadOBJ("sphere.obj", vertices, elements, normals);
	printf("vertices: %i\n", vertices.size());

	/*
	std::vector< glm::vec3 > vertices = {
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, 1, -1),
		glm::vec3(1, 1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(-1, -1, 1),
		glm::vec3(-1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, -1, 1),
	};

	std::vector< int > elements = {
		0,1,2,
		0,2,3,
		0,1,4,
		1,4,5,
		2,1,5,
		2,5,6,
		2,6,3,
		3,6,7,
		3,7,0,
		7,0,4,
		7,4,5,
		7,5,6
	};

	std::vector< glm::vec3 > normals = {
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, 1, -1),
		glm::vec3(1, 1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(-1, -1, 1),
		glm::vec3(-1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, -1, 1),
	};
	*/
	//VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(glm::vec3), &elements[0], GL_DYNAMIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	//event for closing window
	SDL_Event windowEvent;

	//start!
	while(1){

		//handle window close event
		if (SDL_PollEvent(&windowEvent))
			if (windowEvent.type == SDL_QUIT) break;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

		// Camera matrix
		glm::mat4 ViewMatrix = glm::lookAt(
			glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);

		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 ModelMatrix = glm::mat4(1.0f);

		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		//2nd attribute buffer: normals
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);


		// Draw the triangle !
		//glDrawArrays(GL_TRIANGLES, 0, vertices.size() ); // 3 indices starting at 0 -> 1 triangle
		glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//show
		SDL_GL_SwapWindow(window);

		//alter the vertices
		/*
		for (int i = 0; i < vertices.size(); i++) {
			vertices[i] += glm::vec3(randf(-0.01, 0.01), randf(-0.01, 0.01), randf(-0.01, 0.01));
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		*/
	}
	// Cleanup
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalbuffer);

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	SDL_GL_DeleteContext(context);

	SDL_Quit();
	return 0;
}
