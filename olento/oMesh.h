#ifndef OMESH_H
#define OMESH_H

#include "oLoader.h"
#include <vector>
#include <iostream>


class oMesh : public oLoader {
public:
    
    std::vector<glm::vec3> vertices;

    oMesh (std::string path);
    oMesh (int size);
    
    oMesh& operator-=(const oMesh& rhs);

};

#endif
