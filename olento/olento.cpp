#include "olento.h"
#include "random.h"

#include "oWindow.h"
#include "shader.hpp"
#include "objloader.hpp"
#include "dobject.h"
#include "oBuffers.h"
#include "oCamera.h"
#include "oDirectory.h"
#include "oMeshBundle.h"
#include "oModificators.h"
#include "material.h"
#include "dClock.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <iostream>
#include <atomic>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

// ----------------------------------------------------------------------------
// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle


//Ikkunan koko
int width = 1000;
int height = 800;

GLuint programID;


std::string dirStr = "olento/resources/";
std::string objectDir = dirStr + "meshes/olento_testi.obj";
std::string meshDir = dirStr + "meshes/";
std::vector<std::string> modDirs = {"ylos", "sivulle", "ulos", "kierteinen"};

float muodonArvot[7];

std::atomic<bool> isRunning = false;
std::atomic<bool> verteksitVarattu = false;

std::vector<glm::vec3> aimVerts;

oModificators mods(meshDir, "arkkityypit", modDirs);


void asetaMuoto(std::vector<float> values) {
	
	if (values.size() != 7) std::cerr << "Varoitus: Annettiin muodolle " << values.size() << " arvoa\n";
	/*
	for (int i = 0; i < values.size() && i < 7; i++) {
		muodonArvot[i] = values[i];
	}

	bound(muodonArvot[0], 0.001, 3.999);
	bound(muodonArvot[1], -0.999, 0.999);
	bound(muodonArvot[2], -0.999, 0.999);
	bound(muodonArvot[3], -0.999, 0.999);
	bound(muodonArvot[4], -0.999, 0.999);

	//HUOM! turha vektorimuunnos, getShape voisi vastaanottaa arrayn!
	std::vector<float> values(muodonArvot, muodonArvot + 5);
	aimVerts = mods.getShape(values);*/

	values.resize(5);

	bound(values[0], 0.001, 2.999);
	bound(values[1], -0.999, 0.999);
	bound(values[2], -0.999, 0.999);
	bound(values[3], -0.999, 0.999);
	bound(values[4], -0.999, 0.999);

	dClock t;

	std::vector<glm::vec3> newAimVerts = mods.getShape(values);

	//odota lupaa kirjoittaa
	while (verteksitVarattu)
		t.delay(10000);
	
	verteksitVarattu = true;
	aimVerts = newAimVerts;
	verteksitVarattu = false;

}

void initialize() {

	srand(time(NULL));

	//initialize window and context
	oWindow::init(width, height);

	glClearColor(0.7f, 0.9f, 1.0f, 0.0f);

	oBuffers::init();

	oDirectory dir(dirStr);
	/*
	modDirs[0] = "ylos";
	modDirs[1] = "sivulle";
	modDirs[2] = "ulos";
	modDirs[3] = "kierteinen";
	*/
	std::string vertexShaderPath = dir.path + "shaders/StandardShading.vertexshader";
	std::string fragmentShaderPath = dir.path + "shaders/StandardShading.fragmentshader";
	programID = LoadShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

	//init camera
	oCamera::init(programID, (float)width / height);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	createMaterials();

	glUseProgram(programID);

}


void setMaterial(float mat_n) {
	//luodaan uniformien kahvat
	GLuint DiffuseID = glGetUniformLocation(programID, "diffuseColor");
	GLuint SpecularID = glGetUniformLocation(programID, "specularity");
	GLuint HardnessID = glGetUniformLocation(programID, "hardness");
	GLuint AlphaID = glGetUniformLocation(programID, "alpha");

	material M = getMaterial(mat_n);

	glUniform3f(DiffuseID, M.diffuseColor.r, M.diffuseColor.g, M.diffuseColor.b);
	glUniform1f(SpecularID, M.specularity);
	glUniform1f(HardnessID, M.hardness);
	glUniform1f(AlphaID, M.alpha);
}


void setLight() {
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	glm::vec3 lightPos = glm::vec3(11, 6, 11);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
}


void run() {

	isRunning = true;

#ifdef __APPLE__
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
	{
		// error!
	}
	CFRelease(resourcesURL);

	chdir(path);
	std::cout << "Current Path: " << path << std::endl;
	std::string pathStr = path;
	dirStr = pathStr + "/resources/";
	objectDir = dirStr + "meshes/olento_testi.obj";
	meshDir = dirStr + "meshes/";
#endif



	initialize();

	// Create and compile our GLSL program from the shaders

	dObject obj = dObject(objectDir);
	std::cout << "loading mods\n";

	obj.sortElementsByDistance(oCamera::position);
	//set element data to be drawn
	oBuffers::setElements(obj);

	int loop = 101;

	dClock t;

	setLight();

	//std::vector<float> values;
	//values.resize(5);

	float mat_i = 0;

	//pääluuppi alkaa
	while (oWindow::getCloseEvent() == false){

		//ota aikaa
		t.reset();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (loop++ > 10) {

			/*
			values[0] += randf(-0.1, 0.1);
			values[1] += randf(-0.1, 0.1);
			values[2] += randf(-0.1, 0.1);
			values[3] += randf(-0.1, 0.1);
			values[4] += randf(-0.1, 0.1);

			bound(values[0], 0.001, 3.999);
			bound(values[1], -0.999, 0.999);
			bound(values[2], -0.999, 0.999);
			bound(values[3], -0.999, 0.999);
			bound(values[4], -0.999, 0.999);
			*/

			/*
			//HUOM! turha vektorimuunnos, getShape voisi vastaanottaa arrayn!
			std::vector<float> values(muodonArvot, muodonArvot + 5);
			aimVerts = mods.getShape(values);
			*/
			//obj.sortElementsByDistance(oCamera::position);
			//oBuffers::setElements(obj.elements);

			loop = 0;
		}

		//muuta materiaalia
		mat_i += 0.02;
		wrap(mat_i, 0, 4.99);
		setMaterial(mat_i);

		//muuta verteksit ja normaalit
		if (!verteksitVarattu) {
			verteksitVarattu = true;
			obj.changeVerticesTowards(aimVerts, 0.1f);
			//obj.changeVertices(aimVerts);
			verteksitVarattu = false;

			obj.calculateAllNormals();
		}
		else std::cerr << "Ei voitu muuttaa verteksejä koska varattu\n";
		//Järjestä elementit läpinäkyvyyttä varten. Tässä menee KAUAN (> 4 min)
		//obj.sortElementsByDistance(oCamera::position);

		//päivitä ja näytä
		oBuffers::updateBuffers(obj);
		oCamera::update();
		oBuffers::updateAttributes();

		GLenum err = glGetError();
		if (err != 0) std::cout << "Error: " << err << "\n";

		oWindow::show();

		//ajasta 30 FPS:ään
		t.delay(30);

	}

	oBuffers::close();
	glDeleteProgram(programID);

	oWindow::close();

	isRunning = false;
}


bool olentoPyorii() {
	return isRunning;
}