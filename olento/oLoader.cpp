#include "oLoader.h"
#include <iostream>
#include <string.h>

#include <stdio.h>

bool oLoader::loadOBJ(
	std::string path,
	std::vector<glm::vec3>& vertices)
{

	std::cout << "Loading OBJ file " << path.c_str() << "... ";

	FILE * file = fopen(path.c_str(), "r");
	if (file == NULL){
		printf("oLoader: Tiedostoa %s ei voitu avata!\n", path.c_str());
		getchar();
		return false;
	}

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){

			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);

		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}


	std::cout << "done!\n";
	return true;


}

bool oLoader::loadOBJ(
	std::string path,
	std::vector<glm::vec3>& vertices,
	std::vector<unsigned int>& elements,
	std::vector<dFace>& faces)
{

	//  std::cout << "Loading OBJ file " << path.c_str() << "...  " ;

	FILE * file = fopen(path.c_str(), "r");
	if (file == NULL){
		printf("oLoader: Tiedostoa %s ei voitu avata!\n", path.c_str());
		return false;
	}

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){

			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);

		}
		else if (strcmp(lineHeader, "f") == 0){

			unsigned int vertexIndex[3], normalIndex[3];

			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);

			if (matches != 6){

				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;

			}

			vertexIndex[0] -= 1;
			vertexIndex[1] -= 1;
			vertexIndex[2] -= 1;

			// use these to make Face
			faces.push_back(vertexIndex);

			elements.push_back(vertexIndex[0]);
			elements.push_back(vertexIndex[1]);
			elements.push_back(vertexIndex[2]);

		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	return true;
}

