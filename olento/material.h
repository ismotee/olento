#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <stdlib.h>

struct material{
	float alpha; //0...1
	float specularity; //0...1
	float hardness; // 1...7
	glm::vec3 diffuseColor; //r,g,b 0...1

};

void createMaterials();

material getMaterial(int n);

// Random Float
float randf(float min, float max);

glm::vec3 randvec(float min, float max);


material createRandomMaterial();

#endif