#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <stdlib.h>

struct material{
	float alpha; //0...1
	float specularity; //0...1
	float hardness; // 1...7
	float ambient; //0...1
	material(){}
	material(float, float, float, float);

};

void createMaterials();

material getMaterial(float n);
material mixMaterials(material A, material B, float amount);

// Random Float
float randf(float min, float max);

glm::vec3 randvec(float min, float max);


material createRandomMaterial();

#endif