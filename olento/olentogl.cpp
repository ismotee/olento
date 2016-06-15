#include "olentogl.h"
#include "oModificators.h"

//Muut kirjastot
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <algorithm> //copy

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.hpp"
#include "random.h"

using namespace std;

namespace olentogl {

string resourcesDir = "/media/olento/Uusi asema/ohjelmointi/c++/olento/olento/resources/";
string vertexShaderPath = resourcesDir + "shaders/standard.vertexshader";
string fragmentShaderPath = resourcesDir + "shaders/standard.fragmentshader";
string imgVertexShaderPath = resourcesDir + "shaders/images.vertexshader";
string imgFragmentShaderPath = resourcesDir + "shaders/images.fragmentshader";
//string backgroundPath = resourcesDir + "tausta.png";

vector<string> backgroundPaths = {
	resourcesDir + "tausta1.png",
	resourcesDir + "tausta2.png",
	resourcesDir + "tausta3.png"
};

string palettePath = resourcesDir + "palette.png";

string meshDir = resourcesDir + "meshes/";
vector<string> modDirs = {"ylos", "sivulle", "ulos", "kierteinen"};

glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

int screenWidth = 0;
int screenHeight = 0;

SDL_Window* window;
SDL_GLContext context;

GLuint programID;
GLuint imgProgramID;
GLuint vertexArrayID;

glm::vec3 cameraPosition;

vector<oObj> kappaleet;
oModificators mods;
xyPalette paletti;

oImage background;

int activeObject = 0;

bool running = false;


bool initialize(int window_w, int window_h, bool fullscreen) {

	/*** SDL-kontekstin luonti ***/
	screenWidth = window_w;
	screenHeight = window_h;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	//Asetetaan SDL videolle & versionumerot
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    //Multisamplen asetukset
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	GLenum err = glGetError();
	if(err!=0) cerr << "Set SDL attributes: " << err << "\n";

	Uint32 windowFlags = SDL_WINDOW_OPENGL;
	if(fullscreen) windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	//Luodaan ikkuna ja konteksti
	window = SDL_CreateWindow("OpenGL", 0, 0, screenWidth, screenHeight, windowFlags);
	if (window == NULL) {
		cerr << "Ikkunaa ei voitu luoda! " << SDL_GetError() << "\n";
		return false;
	}

	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		cerr << "Kontekstia ei voitu luoda! " << SDL_GetError() << "\n";
		return false;
	}

	err = glGetError(); 
	if(err!=0) cerr << "Create context: " << err << "\n";

	//GLEW'n asetukset. Ei käytetä Applen alustalla.
	#ifndef __APPLE__
		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK) {
			cerr << "Error initializing GLEW! " <<  glewGetErrorString(glewError) << "\n";
			return false;
		}
	#endif

	err = glGetError(); 
	if(err!=0) cerr << "GLEW: " << err << "\n";

	//Blendausasetuksia
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);


	//tarkistetaan errorit
	err = glGetError(); 
	if(err!=0) cerr << "Setup GL: " << err << "\n";

	/*** GL:n asetukset ***/

	//Luodaan VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	err = glGetError(); 
	if (err!=0) cerr << "Generate VAO: " << err << "\n";

	//taustaväri
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//shaderien lataaminen
	programID = LoadShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str() );
	imgProgramID = LoadShaders(imgVertexShaderPath.c_str(), imgFragmentShaderPath.c_str() );
	err = glGetError(); 
	if(err!=0) cerr << "Load shaders: " << err << "\n";

	createMaterials();
	setLight();

	//Luodaan kappaleet ja modifikaattorit
	kappaleet.resize(1);
	for(int i=0; i<kappaleet.size(); i++)
		kappaleet[i].create();

    mods.load(meshDir, "arkkityypit", modDirs);

    //luodaan taustakuva ja paletti
	background.create(backgroundPaths[0]);
	if (!paletti.loadFromFile(palettePath))
		std::cerr << "Palettia ei voitu ladata! (" << palettePath << ")\n";

	setMaterial(1);
	setColor(1,0,0);
	setView();

	//Jos tähän asti päästään, onnistuttiin.
	err = glGetError(); 
	if(err!=0) cerr << "Init: " << err << "\n";
	running = true;
	return true;
}


void show() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	background.show();

	glClear(GL_DEPTH_BUFFER_BIT);

	for(int i=0; i<kappaleet.size(); i++) {
		setPosition(kappaleet[i].position);
		kappaleet[i].show();
	}

	SDL_GL_SwapWindow(window);
	GLenum err = glGetError(); 
	if(err!=0) cerr << "Show: " << err << "\n";
}


void handleEvents() {
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		if(e.type == SDL_QUIT)
			running = false;
		else if(e.type == SDL_KEYDOWN) {
			if(e.key.keysym.sym == SDLK_F12)
				running = false;
		}
	}
}


void setLight() {
	glUseProgram(programID);
	GLuint LightA_ID = glGetUniformLocation(programID, "LightPosition_A_worldspace");
	GLuint LightB_ID = glGetUniformLocation(programID, "LightPosition_B_worldspace");

	GLuint LightPower_A_ID = glGetUniformLocation(programID, "lightPower_A"); 
	GLuint LightPower_B_ID = glGetUniformLocation(programID, "lightPower_B");

	GLuint AmbientPower_ID = glGetUniformLocation(programID, "ambientPower");

	//takavalo
	glm::vec3 lightPos_A = glm::vec3(-15, -1, -1);
	float lightPower_A = 200.0f;

	//etu
	glm::vec3 lightPos_B = glm::vec3(15, 1, 1);
	float lightPower_B = 150.0f;

	float ambientPower = 0.3f;

	glUniform3f(LightA_ID, lightPos_A.x, lightPos_A.y, lightPos_A.z);
	glUniform3f(LightB_ID, lightPos_B.x, lightPos_B.y, lightPos_B.z);
	glUniform1f(LightPower_A_ID, lightPower_A);
	glUniform1f(LightPower_B_ID, lightPower_B);
	glUniform1f(AmbientPower_ID, ambientPower);

	GLenum err = glGetError(); 
	if(err!=0) cerr << "SetLight: " << err << "\n";

}


void setView() {
	glUseProgram(programID);
	cameraPosition = glm::vec3(8, 3, 3);
	float aspectRatio = (float)screenWidth / (float)screenHeight;

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	
    // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
    
    // Camera matrix
    viewMatrix = glm::lookAt(
                                 cameraPosition,
                                 glm::vec3(0, 0, 0), // look at the origin
                                 glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
                                 );
    // Model matrix : an identity matrix (model will be at the origin)
    modelMatrix = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix; // Remember, matrix multiplication is the other way around

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

}


void setPosition(glm::vec3 pos) {
	glUseProgram(programID);
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    modelMatrix = glm::mat4(1.0f); //identity
    //TODO: transform!

    glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);

    GLenum err = glGetError(); 
	if(err!=0) cerr << "SetPosition: " << err << "\n";

}


void setMaterial(material mat) {
	//luodaan uniformien kahvat
	glUseProgram(programID);
	GLuint SpecularID = glGetUniformLocation(programID, "specularity");
	GLuint HardnessID = glGetUniformLocation(programID, "hardness");
	GLuint AlphaID = glGetUniformLocation(programID, "alpha");
	GLuint ambientID = glGetUniformLocation(programID, "ambientPower");

	glUniform1f(SpecularID, mat.specularity);
	glUniform1f(HardnessID, mat.hardness);
	glUniform1f(AlphaID, mat.alpha);
	glUniform1f(ambientID, mat.ambient);

	GLenum err = glGetError(); 
	if(err!=0) cerr << "SetMaterial: " << err << "\n";
}

void setMaterialTowards(material target, float amount) {
	static material mat;

	mat = mixMaterials(mat, target, amount);
	setMaterial(mat);
}



void setMaterial(float mat_n) {
	setMaterial(getMaterial(mat_n));
}


void setMaterialTowards(float target, float amount) {
	static float mat_n = 0.5;

	mat_n = glm::mix(mat_n, target, amount);

	setMaterial(getMaterial(mat_n));
}


void setColor(float r, float g, float b) {
	glUseProgram(programID);
	GLuint DiffuseID = glGetUniformLocation(programID, "diffuseColor");
	glUniform3f(DiffuseID, r, g, b);
	GLenum err = glGetError(); 
	if(err!=0) cerr << "SetColor: " << err << "\n";
}


void setColor(glm::vec3 color) {
	setColor(color.r, color.g, color.b);
}


void setColorTowards(glm::vec3 target, float amount) {
	static glm::vec3 color(0.5, 0.5, 0.5);

	color = glm::mix(color, target, amount);
	setColor(color);
}


bool isRunning() {
	return running;
}


void asetaMuoto(vector<float> arvot) {
	kappaleet[activeObject].asetaMuoto(arvot);
}


void valitseObjekti(int indeksi) {
	if(indeksi >=0 && indeksi < kappaleet.size() )
		activeObject = indeksi;
	else
		cerr << "Huono indeksi: " << indeksi << "! Kappaleita on " << kappaleet.size() << "\n";
}


void vaihdaKuva() {
	static int valittuKuva = 0;
	valittuKuva ++;
	if(valittuKuva >= backgroundPaths.size() )
		valittuKuva = 0;

	background.asetaKuva(backgroundPaths[valittuKuva] );
}


void oObj::updateBuffers() {

	obj.sortElementsByDistance(cameraPosition);
	obj.calculateAllNormals();

	//päivitä bufferit
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, obj.getVertexData().length, obj.getVertexData().data, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
	glBufferData(GL_ARRAY_BUFFER, obj.getNormalData().length, obj.getNormalData().data, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.getElementData().length, obj.getElementData().data, GL_STATIC_DRAW);

	GLenum err = glGetError(); 
	if(err!=0) cerr << "Update buffers: " << err << "\n";
}


void oObj::show() {

	glUseProgram(programID);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	//päivitetään verteksit ja normaalit bufferiin.
	updateBuffers();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	setMaterialTowards(mat, 0.1);
	setColorTowards(color, 0.1);

	glDrawElements(GL_TRIANGLES, obj.getElementData().length, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	GLenum err = glGetError(); 
	if(err!=0) cerr << "Show:draw: " << err << "\n";
}


bool oObj::create(string objfile) {

	mat = getMaterial(3);
	color = glm::vec3(0.5, 0.5, 0.5);
	position = glm::vec3(0, 0, 0);

	//luodaan GL-bufferit
	glGenBuffers(1, &vertexBufferID);
	glGenBuffers(1, &normalBufferID);
	glGenBuffers(1, &elementBufferID);

	glUseProgram(programID);

	// 1st attribute buffer : vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);


	// 2nd attribute buffer : normals
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	GLenum err = glGetError(); 
	if(err!=0) cerr << "oObj:create: " << err << "\n";

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	//ladataan objekti
	obj.loadFromFile(objfile);

	//työnnetään verteksit bufferiin
	updateBuffers();
}


void oObj::asetaMuoto(vector<float> values) {
  
  if (values.size() != 8) {
    std::cerr << "Varoitus: Annettiin muodolle " << values.size() << " arvoa\n";
    return;
  }
  
  //skaalaa luvut olennon käyttöön! Oletetaan että tulevat arvot ovat avoimella välillä 0...1
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

  //hae modifikaattorista uudet verteksit
  std::vector<glm::vec3> newVerts = mods.getShape(shapeValues);

  //aseta vertekstit kappaleeseen
  obj.changeVerticesTowards(newVerts, 0.01);

  //5: materiaali, 6 & 7: väri
  mat = getMaterial(values[5]);
  color = paletti.getColor(values[6], values[7] );

  //TODO: muoto ja väri

/*
  while (varattu)
	std::this_thread::yield();

  varattu = true;
  aimVerts = newAimVerts;
  mat_n = values[5];
  color_x = values[6];
  color_y = values[7];  
  varattu = false;
  */
}


bool oImage::create(string path) {
	//käytetään ohjelmaa
	glUseProgram(imgProgramID);

	//luodaan buffer ja tekstuuri
	glGenBuffers(1, &vertexBufferID);
	glGenTextures(1, &textureID);

	asetaKuva(path);

	//luodaan verteksit
	float newV[] = {
	-1, -1,  0,
	 1, -1,  0,
	-1,  1,  0,

	 1, -1,  0,
	-1,  1,  0,
	 1,  1,  0
	};

	copy(newV, newV+18, vertices);

	//laitetaan verteksit bufferiin
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(
		2,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDisableVertexAttribArray(2);

	GLenum err = glGetError(); 
	if(err!=0) cerr << "oImage:create: " << err << "\n";
}


void oImage::asetaKuva(string path) {

	glUseProgram(imgProgramID);

	//ladataan kuva tekstuuriin
	SDL_Surface* tmp = IMG_Load(path.c_str() );
	if (tmp == NULL) {
		std::cout << "Kuvaa ei voitu ladata: " << path.c_str() << " (" << SDL_GetError() << ")\n";
	}

	if(tmp->format->BytesPerPixel != 4)
		cerr << "Varoitus: BPP on " << (int)tmp->format->BytesPerPixel << "\n";

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_RGBA, 
			tmp->w, tmp->h, 
			0, 
			GL_RGBA, 
			GL_UNSIGNED_BYTE, 
			tmp->pixels
		);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(tmp);

}


void oImage::show() {
	glUseProgram(imgProgramID);
	//glDisable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

	GLenum err = glGetError(); 
	if(err!=0) cerr << "oImage:show:set attribute: " << err << "\n";

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, 18);

	glDisableVertexAttribArray(2);

	err = glGetError(); 
	if(err!=0) cerr << "oImage:show:Draw: " << err << "\n";
}



bool xyPalette::init() {

	if (!IMG_Init(IMG_INIT_PNG)) return false;
	else return true;
}


bool xyPalette::loadFromFile(std::string filename){
	
	if (!init()) return false;
	
	SDL_Surface* surface = IMG_Load(filename.c_str());

	if (surface == NULL) return false;

	w = surface->w;
	h = surface->h;
	size = w * h;

	colors.resize(size);

	for (int i = 0; i < size; i++) {
		//get pointer to pixel
		Uint8 *p = (Uint8 *)surface->pixels + i * surface->format->BytesPerPixel;
		
		//put color to vector

		float r = (float)p[0] / 256;
		float g = (float)p[1] / 256;
		float b = (float)p[2] / 256;

		colors[i] = glm::vec3(r,g,b);
	}

	std::cout << "Created palette, size: " << size << ", w: " << w << ", h: " << h << ", bpp: " << (int)surface->format->BytesPerPixel << "\n";
	if ((int)surface->format->BytesPerPixel != 4)
		std::cerr << "Varoitus! Paletin BPP pitäisi olla 4\n";
	SDL_FreeSurface(surface);
	return true;
}


glm::vec3 xyPalette::getColor(float x, float y) {
	int i_x = w*x;
	int i_y = h*y;

	int i = w*i_y + i_x;

	//std::cerr << "getColor " << i_x << ", " << i_y << " (" << i << ")\n";

	if (i<0 || i>size)
		std::cerr << "Huono väri-indeksi: " << i << " !\n";

	return colors[i];
}


void xyPalette::tulosta() {
	std::cout << "Paletti: \n";
	
	for (int i = 0; i < size; i++)
		std::cout << "r: " << colors[i].r << ", g: " << colors[i].g << ", b: " << colors[i].b << "\n";

	std::cout << size << " väriä\n";

}


};//end of namespace