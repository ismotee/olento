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
std::string palettePath = dirStr + "palette.png";

std::vector<glm::vec3> aimVerts;
float mat_n = 1;
float color_x = 0;
float color_y = 0;

oModificators mods(meshDir, "arkkityypit", modDirs);

xyPalette paletti;

dObject obj;



void setLight() {
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	glm::vec3 lightPos = glm::vec3(11, 6, 11);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
}


void initialize() {

	srand(time(NULL));

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
	palettePath = dirStr + "palette.png";

#endif

	//initialize window and context
	oWindow::init(width, height);

	if (!paletti.loadFromFile(palettePath))
		std::cerr << "Palettia ei voitu ladata! (" << palettePath << ")\n";
	else
		;//paletti.tulosta();

	glClearColor(0.7f, 0.9f, 1.0f, 0.0f);

	oBuffers::init();

	oDirectory dir(dirStr);

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

	//dObject obj = dObject(objectDir);
	obj.loadFromFile(objectDir);

	obj.sortElementsByDistance(oCamera::position);
	//set element data to be drawn
	oBuffers::setElements(obj);

	aimVerts = obj.vertices;

	setLight();

}


void setMaterial(float mat_n) {
	//luodaan uniformien kahvat
	GLuint SpecularID = glGetUniformLocation(programID, "specularity");
	GLuint HardnessID = glGetUniformLocation(programID, "hardness");
	GLuint AlphaID = glGetUniformLocation(programID, "alpha");

	material M = getMaterial(mat_n);

	glUniform1f(SpecularID, M.specularity);
	glUniform1f(HardnessID, M.hardness);
	glUniform1f(AlphaID, M.alpha);
}


void setColor(float x, float y) {
	GLuint DiffuseID = glGetUniformLocation(programID, "diffuseColor");
	glm::vec3 color = paletti.getColor(x, y);
	glUniform3f(DiffuseID, color.r, color.g, color.b);
}

void asetaMuoto(std::vector<float> values) {
  
  if (values.size() != 8) {
    std::cerr << "Varoitus: Annettiin muodolle " << values.size() << " arvoa\n";
    return;
  }
  
  //skaalaa luvut olennon k�ytt��n! Oletetaan ett� tulevat arvot ovat avoimella v�lill� 0...1
  values[0] *= 3;
  for (int i = 1; i < 5; i++) values[i] = values[i] * 2 - 1;
  values[5] *= 5;


  bound(values[0], 0.001, 2.999);
  for (int i = 1; i < 5; i++) bound(values[i], -0.999, 0.999);
  bound(values[5], 0, 4.999);
  bound(values[6], 0, 0.999);
  bound(values[7], 0, 0.999);
  
  std::vector<float> shapeValues = values;
  shapeValues.resize(5);
  
  dClock t;
  
  std::vector<glm::vec3> newAimVerts = mods.getShape(shapeValues);
   
  aimVerts = newAimVerts;
  mat_n = values[5];
  color_x = values[6];
  color_y = values[7];  
}


void updateGL() {

	//"p��luuppi alkaa"

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//muuta verteksit ja normaalit
	obj.changeVerticesTowards(aimVerts, 0.1f);
	//obj.changeVertices(aimVerts);

	setMaterial(mat_n);
	setColor(color_x, color_y);

	obj.calculateAllNormals();

	//J�rjest� elementit l�pin�kyvyytt� varten. T�ss� menee KAUAN (> 4 min)
	//obj.sortElementsByDistance(oCamera::position);

	//p�ivit� ja n�yt�
	oBuffers::updateBuffers(obj);
	oCamera::update();
	oBuffers::updateAttributes();
	GLenum err = glGetError();
	if (err != 0) std::cout << "Error: " << err << "\n";

	oWindow::show();

}


void suljeOlento() {
	oBuffers::close();
	glDeleteProgram(programID);

	oWindow::close();
}
