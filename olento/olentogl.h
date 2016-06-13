﻿#ifndef OLENTOGL_H
#define OLENTOGL_H

//OpenGL-kirjastot
//Käytetään OpenGL3 Applelle ja GLEWiä muille alustoille
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif
#ifdef __linux__
#include <GL/glew.h>
#endif
#ifdef _WIN32
#include <glew.h>
#endif

#include <string>
#include "material.h"
#include "dobject.h"

#define DEFAULT_OBJ_PATH "/media/olento/Uusi asema/ohjelmointi/c++/GLalusta/GLalusta/resources/meshes/default.obj"

namespace olentogl {

	struct oObj{

		dObject obj;
		GLuint vertexBufferID;
		GLuint normalBufferID;
		GLuint elementBufferID;
		material mat;
		glm::vec3 color;
		void show();
		bool create(std::string objfile = DEFAULT_OBJ_PATH);
		void updateBuffers();
		void asetaMuoto(std::vector<float> values);
	};

	struct oImage {
		float vertices[18];

		GLuint vertexBufferID;
		GLuint elementBufferID;
		GLuint textureID;

		bool create();
		void show();

	};


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

	bool initialize(int w = 1920, int h = 1080);

	void show();
	void handleEvents();
	void setLight();
	bool isRunning();
	void setMaterial(float mat_n);
	void setMaterial(material mat);
	void setColor(float r, float g, float b);
	void setColor(glm::vec3 color);
	void setView();
	void asetaMuoto(std::vector<float> arvot);
};

#endif

/*

Yhteistä
	* shaderit
		- kappaleille
		- kuville
	* vao
	* valot
	* SDL-konteksti
	* xy-paletti

Kappale
	* verteksit
	* vbo
	* materiaali
	
Kuva
	* tekstuuri

*/