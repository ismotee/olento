#include "random.h"

float randf(float min, float max) {
	return ((float)rand() / RAND_MAX) * (max - min) + min;
}

glm::vec3 randvec(float min, float max) {
	return(glm::vec3(randf(min, max), randf(min, max), randf(min, max)));
}


void bound(float& value, float min, float max) {
	if (value < min) value = min;
	if (value > max) value = max;
}


void wrap(float& value, float min, float max) {
	float w = max - min;
	while (value < min) value += w;
	while (value > max) value -= w;
}


float vektorienEro(std::vector<float> A, std::vector<float> B) {

	if (A.size() != B.size() || A.empty() || B.empty())
		return -1;

	float summa = 0;

	for (int i = 0; i < A.size(); i++) {
		summa += fabs(A[i] - B[i]);
	}

	//summa /= (float)A.size();
	return summa;
}


float floatabs(float arvo) {
	if (arvo > 0) return arvo;
	else return (arvo * (-1));
}
