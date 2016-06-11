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
#include <thread>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

// ----------------------------------------------------------------------------
// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle


//Ikkunan koko
int width = 1000;
int height =800;

GLuint programID;

std::string dirStr = "resources/";
std::string objectDir = dirStr + "meshes/olento_testi.obj";
std::string meshDir = dirStr + "meshes/";
std::vector<std::string> modDirs = {"ylos", "sivulle", "ulos", "kierteinen"};
std::string palettePath = dirStr + "palette.png";

std::vector<glm::vec3> aimVerts;
float mat_n = 1;
float color_x = 0;
float color_y = 0;

oModificators mods;

xyPalette paletti;

dObject obj;

std::atomic<bool> varattu;

void setLight() {
	GLuint LightA_ID = glGetUniformLocation(programID, "LightPosition_A_worldspace");
	GLuint LightB_ID = glGetUniformLocation(programID, "LightPosition_B_worldspace");

	GLuint LightPower_A_ID = glGetUniformLocation(programID, "lightPower_A"); 
	GLuint LightPower_B_ID = glGetUniformLocation(programID, "lightPower_B");

	GLuint AmbientPower_ID = glGetUniformLocation(programID, "ambientPower");

	//takavalo
	glm::vec3 lightPos_A = glm::vec3(-15, -1, -1);
	float lightPower_A = 150.0f;

	//etu
	glm::vec3 lightPos_B = glm::vec3(15, 1, 1);
	float lightPower_B = 50.0f;

	float ambientPower = 0.45f;

	glUniform3f(LightA_ID, lightPos_A.x, lightPos_A.y, lightPos_A.z);
	glUniform3f(LightB_ID, lightPos_B.x, lightPos_B.y, lightPos_B.z);
	glUniform1f(LightPower_A_ID, lightPower_A);
	glUniform1f(LightPower_B_ID, lightPower_B);
	glUniform1f(AmbientPower_ID, ambientPower);

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

    mods.load(meshDir, "arkkityypit", modDirs);
	std::string vertexShaderPath = dir.path + "shaders/multiLight.vertexshader";
	std::string fragmentShaderPath = dir.path + "shaders/multiLight.fragmentshader";

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
  
  //skaalaa luvut olennon k‰yttˆˆn! Oletetaan ett‰ tulevat arvot ovat avoimella v‰lill‰ 0...1
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
   
  while (varattu)
	std::this_thread::yield();

  varattu = true;
  aimVerts = newAimVerts;
  mat_n = values[5];
  color_x = values[6];
  color_y = values[7];  
  varattu = false;
}


void updateGL() {

	//"p‰‰luuppi alkaa"

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//muuta verteksit ja normaalit
	//obj.changeVerticesTowards(aimVerts, 0.1);
	while (varattu)
		std::this_thread::yield();
	obj.changeVertices(aimVerts);

	setMaterial(mat_n);
	setColor(color_x, color_y);

	obj.calculateAllNormals();

	//J‰rjest‰ elementit l‰pin‰kyvyytt‰ varten.
	//obj.sortElementsByDistance(oCamera::position);

	//p‰ivit‰ ja n‰yt‰
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
