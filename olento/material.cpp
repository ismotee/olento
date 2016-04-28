#include "material.h"

material materials[5];


float randf(float min, float max) {
	return ((float)rand() / RAND_MAX) * (max - min) + min;
}

glm::vec3 randvec(float min, float max) {
	return(glm::vec3(randf(min, max), randf(min, max), randf(min, max)));
}

void createMaterials() {
	for (int i = 0; i < 5; i++)
		materials[i] = createRandomMaterial();
}


material getMaterial(int n) {
	if (n<0 || n>5)
		n = 0;

	return materials[n];
}

material createRandomMaterial() {
	material result;
	float alphas[4] = { 0.3, 0.7, 1, 1 };

	result.alpha = alphas[rand() % 4];
	result.specularity = randf(0, 1);
	result.hardness = randf(1, 7);
	result.diffuseColor = randvec(0, 1);

	return result;
}