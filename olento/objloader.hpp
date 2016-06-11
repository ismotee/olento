//
//  objloader.h
//  opengltest
//
//  Created by Ismo Torvinen on 16.3.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "glm/glm.hpp"
#include <vector>

std::vector < glm::vec3 > loadVertices(const char* path);
	
std::vector <unsigned int> loadElements(const char* path);


bool loadOBJ(
             const char * path,
             std::vector<glm::vec3> & out_vertices,
             std::vector<unsigned int> & out_elements,
             std::vector<glm::vec3> & out_normals
             );



bool loadAssImp(
                const char * path,
                std::vector<unsigned short> & indices,
                std::vector<glm::vec3> & vertices,
                std::vector<glm::vec2> & uvs,
                std::vector<glm::vec3> & normals
                );

#endif