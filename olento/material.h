#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

struct material{
	float alpha; //0...1
	float specularity; //0...1
	float hardness; // 1...7
	glm::vec3 diffuseColor; //r,g,b 0...1

};

// Random Float
float randf(float min, float max) {
	return ((float)rand() / RAND_MAX) * (max - min) + min;
}

glm::vec3 randvec(float min, float max) {
	return(glm::vec3(randf(min, max), randf(min, max), randf(min, max)));
}


material createRandomMaterial() {
	material result;
	float alphas[4] = { 0.2, 0.5, 1, 1};

	result.alpha = alphas[rand() % 4];
	result.specularity = randf(0, 1);
	result.hardness = randf(1, 7);
	result.diffuseColor = randvec(0, 1);

	return result;
}

#endif