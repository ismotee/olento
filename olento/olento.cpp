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

float muodonArvot[8];

std::atomic<bool> isRunning(false);
std::atomic<bool> verteksitVarattu(false);
std::atomic<bool> ready(false);

std::vector<glm::vec3> aimVerts;
float mat_n = 1;
float color_x = 0;
float color_y = 0;

oModificators mods(meshDir, "arkkityypit", modDirs);

xyPalette paletti;

std::vector<float> desiredOutput(8,0.5);
bool koulutetaan  = false;
bool katsellaan = false;

void initialize() {
  
  srand(time(NULL));
  
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
  glUniform3f(DiffuseID, color.r,color.g, color.b);
}

void asetaMuoto(std::vector<float> values) {
  
  if (!ready) return;
  
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
  
  //odota lupaa kirjoittaa
  while (verteksitVarattu)
    t.delay(10000);
  
  verteksitVarattu = true;
  aimVerts = newAimVerts;
  mat_n = values[5];
  color_x = values[6];
  color_y = values[7];
  verteksitVarattu = false;
  
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
  
  aimVerts = obj.vertices;
  
  int loop = 101;
  
  dClock t;
  
  setLight();
  
  ready = true;
  std::cerr << "run!\n";
  
  //p��luuppi alkaa
  while (oWindow::getCloseEvent() == false && isRunning == true){
    
    //ota aikaa
    t.reset();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //muuta verteksit ja normaalit
    if (!verteksitVarattu) {
      verteksitVarattu = true;
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
      
      verteksitVarattu = false;
      
    }
    //else std::cerr << "Ei voitu muuttaa verteksej� koska varattu\n";
    
	handleEvent();

    //ajasta 30 FPS:��n
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


void suljeOlento() {
  isRunning = false;
}


bool handleEvent() {
	//return true if there was event
	const float koulutus_up = 0.02;
	const float koulutus_down = 0.02;
	bool result = false;

	if (desiredOutput.size() != 8)
		std::cerr << "desiredOutput on " << desiredOutput.size() << "!\n";

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			result = true;
			isRunning = false;
		}

		else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){

			result = true;

			switch (e.key.keysym.sym) {
			case SDLK_q:
				if (e.type == SDL_KEYDOWN) desiredOutput[0] += koulutus_up;
				//else desiredOutput[0] = 0.5;
				break;
			case SDLK_a:
				if (e.type == SDL_KEYDOWN) desiredOutput[0] -= koulutus_down;
				//else desiredOutput[0] = 0.5;
				break;
			case SDLK_w:
				if (e.type == SDL_KEYDOWN) desiredOutput[1] += koulutus_up;
				//else desiredOutput[1] = 0.5;
				break;
			case SDLK_s:
				if (e.type == SDL_KEYDOWN) desiredOutput[1] -= koulutus_down;
				//else desiredOutput[1] = 0.5;
				break;
			case SDLK_e:
				if (e.type == SDL_KEYDOWN) desiredOutput[2] += koulutus_up;
				//else desiredOutput[2] = 0.5;
				break;
			case SDLK_d:
				if (e.type == SDL_KEYDOWN) desiredOutput[2] -= koulutus_down;
				//else desiredOutput[2] = 0.5;
				break;
			case SDLK_r:
				if (e.type == SDL_KEYDOWN) desiredOutput[3] += koulutus_up;
				//else desiredOutput[3] = 0.5;
				break;
			case SDLK_f:
				if (e.type == SDL_KEYDOWN) desiredOutput[3] -= koulutus_down;
				//else desiredOutput[3] = 0.5;
				break;
			case SDLK_t:
				if (e.type == SDL_KEYDOWN) desiredOutput[4] += koulutus_up;
				//else desiredOutput[4] = 0.5;
				break;
			case SDLK_g:
				if (e.type == SDL_KEYDOWN) desiredOutput[4] -= koulutus_down;
				//else desiredOutput[4] = 0.5;
				break;
			case SDLK_y:
				if (e.type == SDL_KEYDOWN) desiredOutput[5] += koulutus_up;
				//else desiredOutput[5] = 0.5;
				break;
			case SDLK_h:
				if (e.type == SDL_KEYDOWN) desiredOutput[5] -= koulutus_down;
				//else desiredOutput[5] = 0.5;
				break;
			case SDLK_u:
				if (e.type == SDL_KEYDOWN) desiredOutput[6] += koulutus_up;
				//else desiredOutput[6] = 0.5;
				break;
			case SDLK_j:
				if (e.type == SDL_KEYDOWN) desiredOutput[6] -= koulutus_down;
				//else desiredOutput[6] = 0.5;
				break;
			case SDLK_i:
				if (e.type == SDL_KEYDOWN) desiredOutput[7] += koulutus_up;
				//else desiredOutput[7] = 0.5;
				break;
			case SDLK_k:
				if (e.type == SDL_KEYDOWN) desiredOutput[7] -= koulutus_down;
				//else desiredOutput[7] = 0.5;
				break;
			case SDLK_RETURN:
				if (e.type == SDL_KEYDOWN) koulutetaan = !koulutetaan;
				if (koulutetaan) std::cout << "koulutetaan\n";
				else std::cout << "ei en�� kouluteta\n";
				break;
			case SDLK_SPACE:
				for (int i = 0; i < desiredOutput.size(); i++)
					desiredOutput[i] = 0.5;
				break;
			case SDLK_TAB:
				if (e.type == SDL_KEYDOWN) {
					katsellaan = !katsellaan;
					if (katsellaan) std::cout << "katsellaan\n";
					else std::cout << "muokataan\n";
				}
				break;
			}

			for (int i = 0; i < desiredOutput.size(); i++)
				bound(desiredOutput[i], 0, 1);

		}
	}

	return result;
}


bool koulutetaanko() {	return koulutetaan;}
bool katsellaanko() { return katsellaan; }

std::vector<float> haeKoulutusarvot() {
	return desiredOutput;
}