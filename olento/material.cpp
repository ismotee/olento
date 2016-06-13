#include "material.h"
#include "random.h"

material materials[5];
//lasimainen
//osittain läpinäkyvä matta
//matta opaque
//kovakiiltoinen opaque

void createMaterials() {
	materials[0] = material(0.1, 1, 10, 0.4);
	materials[1] = material(0.2, 0.05, 1, 0.5);
	materials[2] = material(1, 0.1, 1, 0.4);
	materials[3] = material(1, 1, 15, 0.2);
}

material::material(float alpha_, float specularity_, float hardness_, float ambient_) : 
					alpha(alpha_), 
					specularity(specularity_), 
					hardness(hardness_),
					ambient(ambient_) 
{}


material mixMaterials(material A, material B, float amount) {
	material result;

	result.alpha = glm::mix(A.alpha, B.alpha, amount);
	result.specularity = glm::mix(A.specularity, B.specularity, amount);
	result.hardness = glm::mix(A.hardness, B.hardness, amount);
	result.ambient = glm::mix(A.ambient, B.ambient, amount);

	return result;
}


material getMaterial(float n) {
	
	bound(n, 0, 3);
	
	int prev_n = (int)n;
	
	//Jos luku on kokonainen, palauta suoraan se materiaali
	if (prev_n == n)
		return materials[prev_n];

	//sekoita materiaalit
	int next_n = prev_n + 1;
	float mix = n - prev_n;

	return mixMaterials(materials[prev_n], materials[next_n], mix);

}

material createRandomMaterial() {
	material result;
	float alphas[4] = { 0.3, 0.7, 1, 1 };

	result.alpha = alphas[rand() % 4];
	result.specularity = randf(0, 1);
	result.hardness = randf(1, 7);
	result.ambient = randf(0.1, 0.6);

	return result;
}