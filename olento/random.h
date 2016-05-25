#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <stdlib.h>

float randf(float min, float max);
glm::vec3 randvec(float min, float max);
void bound(float& value, float min, float max);
void wrap(float& value, float min, float max);