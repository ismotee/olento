//olento.h
#ifndef OLENTO_H
#define OLENTO_H
/*
// m�kki includet
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#else // windows includet
#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

// Include GLM
*/

#include <vector>

void run();
void asetaMuoto(std::vector<float> arvot);

bool olentoPyorii();
void suljeOlento();

#endif