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


// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <iostream>

//using namespace glm;

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

// ----------------------------------------------------------------------------
// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle


// Random Float
float randf(float min, float max) {
	return ((float)rand() / RAND_MAX) * (max - min) + min;
}

void initialize() {
	srand(time(NULL));

	//initialize window and context
	oWindow::init();

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
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

	oBuffers::init();

	// Create and compile our GLSL program from the shaders
	std::string dirStr = "/Users/ismotorvinen/Documents/3d/olento/olento/resources/";
    oDirectory dir(dirStr);
    std::string meshDir(dirStr + "meshes/");
    
    std::vector<std::string> modDirs;
    //modDirs.push_back("arkkityypit");
    modDirs.push_back("ylos");
    modDirs.push_back("sivulle");
    modDirs.push_back("ulos");
    modDirs.push_back("kierteinen");
    
    
    
    std::string vertexShaderPath = dir.path + "shaders/StandardShading.vertexshader";
	std::string fragmentShaderPath = dir.path + "shaders/StandardShading.fragmentshader";
    std::string objectDir = dirStr + "meshes/olento_testi.obj";

    oModificators mods(meshDir, "arkkityypit", modDirs);
    
	GLuint programID = LoadShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    dObject obj = dObject(objectDir);

    std::cout << "vertices: " << obj.vertices.size() << "\n";
    std::cout << "faces: " << obj.faces.size() << "\n";
    std::cout << "normals: " << obj.normals.size() << "\n";
    std::cout << "elements: " << obj.elements.size() << "\n";

    oBuffers::setElements(obj.elements);
    
    oCamera::init(programID);
    

	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    int loop = 101;
    float a = 0.0f;
    float b = 0.0;
    float c = 0.0;
    float d = 0.0;
    float e = 0.0;
    std::vector<glm::vec3> aimVerts;

	dClock t;

	//pääluuppi alkaa
	while (oWindow::getCloseEvent() == false){

		t.reset();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(loop++ > 5) {
            
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
            
            std::cout << values[0] << " : " << values[1] << " : " << values[2] << ":" << values[3] << ":" << values[4] << "\n";
            
            aimVerts = mods.getShape(values);
            
            loop = 0;
        }
        
        obj.changeVerticesTowards(aimVerts, 0.1f);
        obj.calculateAllNormals();


		glUseProgram(programID);

		oBuffers::updateBuffers(obj);
        
        oCamera::update();
        
		//set light
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		//update vertices & normals
		oBuffers::updateAttributes();

		// Draw
		//glDrawElements(GL_TRIANGLES, obj.elements.size(), GL_UNSIGNED_INT, 0);

		GLenum err = glGetError();
		if (err != 0) std::cout << "Error: " << err << "\n";

		//show
		oWindow::show();

		//alter vertices & normals
		//for (int i = 0; i < obj.vertices.size(); i++)
		//	obj.vertices[i] = obj.vertices[i] + glm::vec3(randf(-0.01f, 0.01f), randf(-0.01f, 0.01f), randf(-0.01f, 0.01f));
		
		//std::cout << "fps: " << 1/t.get() << "\n";

	}

	oBuffers::close();
	glDeleteProgram(programID);

	oWindow::close();
	return 0;
}