#ifndef RANDOM_H
#define RANDOM_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <stdlib.h>
#include <vector>

float randf(float min, float max);
glm::vec3 randvec(float min, float max);
void bound(float& value, float min, float max);
void wrap(float& value, float min, float max);
float vektorienEro(std::vector<float> A, std::vector<float> B);
float floatabs(float arvo);
float VectorLength (std::vector<float> vec);

#endif