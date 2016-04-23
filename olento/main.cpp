//main.cpp
#include "olento.h"

#include "shader.hpp"
#include "objloader.hpp"
#include "oWindow.h"
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
std::vector<std::string> modDirs(4);

void initialize() {

    srand(time(NULL));

	//initialize window and context
	oWindow::init(width, height);

	// Dark blue background
	glClearColor(0.7f, 0.9f, 1.0f, 0.0f);

	oBuffers::init();

	oDirectory dir(dirStr);

	modDirs[0] = "ylos";
	modDirs[1] = "sivulle";
	modDirs[2] = "ulos";
	modDirs[3] = "kierteinen";

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


void setMaterial(int mat_n) {
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
	glm::vec3 lightPos = glm::vec3(6, 6, 6);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
}


int main(int argc, char* argv[]) {
    
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
#endif

	initialize();

	// Create and compile our GLSL program from the shaders
    
	dObject obj = dObject(objectDir);
	oModificators mods(meshDir, "arkkityypit", modDirs);

    std::cout << "vertices: " << obj.vertices.size() << "\n";
    std::cout << "faces: " << obj.faces.size() << "\n";
    std::cout << "normals: " << obj.normals.size() << "\n";
    std::cout << "elements: " << obj.elements.size() << "\n";

	//Järjestä elementit läpinäkyvyyttä varten.
	obj.sortElementsByDistance(oCamera::position);

	//set element data to be drawn
	oBuffers::setElements(obj.elements);

    int loop = 101;
    float a = 1.0f;
    float b = 0.0;
    float c = 0.0;
    float d = 0.0;
    float e = 0.0;
    std::vector<glm::vec3> aimVerts;

	dClock t;

	setLight();

	//pääluuppi alkaa
	while (oWindow::getCloseEvent() == false){

		//ota aikaa
		t.reset();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(loop++ > 10) {
            
            std::vector<float> values(5);
        
          //  a += 0.06f;
            e+= 0.3f;
  
            if(e >= 1.0f) {
                e = -0.9;
                d += 0.3f;
            }
            if(d >= 1.0f) {
                d = -0.9;
                c += 0.3f;
            }
            if(c >= 1.0f) {
                c = -0.9;
                b += 0.3f;
            }
            if(b >= 1.0f) {
                b = -0.9;
                a += 0.999f;
            }
            if(a >= 3) {
                a = 0;
            }
            
            values[0] = a;
            values[1] = b;
            values[2] = c;
            values[3] = d;
            values[4] = e;
            
          //  std::cout << values[0] << " : " << values[1] << " : " << values[2] << ":" << values[3] << ":" << values[4] << "\n";
            
            aimVerts = mods.getShape(values);
			setMaterial(rand() % 5);

			obj.sortElementsByDistance(oCamera::position);

            loop = 0;
        }

		//muuta verteksit ja normaalit
        obj.changeVerticesTowards(aimVerts, 0.1f);
        obj.calculateAllNormals();

		//päivitä ja näytä
		oBuffers::updateBuffers(obj);
        oCamera::update();
		oBuffers::updateAttributes();

		GLenum err = glGetError();
		if (err != 0) std::cout << "Error: " << err << "\n";

		oWindow::show();

		t.delay(30);

	}

	oBuffers::close();
	glDeleteProgram(programID);

	oWindow::close();
	return 0;
}