#include "random.h"
#include "math.h"
#include <algorithm> // std::sort

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

	summa /= (float)A.size();
	return summa;
}


float floatabs(float arvo) {
	if (arvo > 0) return arvo;
	else return (arvo * (-1));
}

float VectorLength (std::vector<float> vec)
{
    float result = 0;
    for(int i = 0; i < vec.size(); i++)
    {
        result += vec[i] * vec[i];
    }
    return sqrt(result);
}


struct sortT{
	int key;
	float value;
};


bool sortCompareValues(sortT A, sortT B) {
	return(A.value < B.value);
}


bool sortCompareKeys(sortT A, sortT B) {
	return(A.key < B.key);
}



std::vector<int> jarjestaTaulukkoPienimmastaSuurimpaan(std::vector<float> taulukko)
{
	std::vector<int> tulos(taulukko.size());
	std::vector<sortT> sortti(taulukko.size());

	for(int i = 0; i < taulukko.size(); i++)
	{
		sortti[i].key = i;
		sortti[i].value = taulukko[i];
	}

	std::sort(sortti.begin(), sortti.end(), sortCompareValues );

	for(int i=0; i<sortti.size(); i++)
		tulos[i] = sortti[i].key;

	return tulos;
}
