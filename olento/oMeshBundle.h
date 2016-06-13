#ifndef opengltest_oMeshBundle_h
#define opengltest_oMeshBundle_h

#include <vector>
#include "oMesh.h"
#include "oDirectory.h"

std::vector<glm::vec3> lerpVectors (std::vector<glm::vec3>& verts1, std::vector<glm::vec3>& verts2, float value);

class oMeshBundle {
public:
    std::vector<oMesh> meshes;
    
    oMeshBundle(oDirectory& dir);
    oMeshBundle(oDirectory& dir, oMeshBundle& archtypes);
    
    std::vector<glm::vec3> getShape(float value);
    std::vector<glm::vec3> getShape(float a_value,float m_value);
    
    //making easier and more intuitive to get data from vector
    oMesh& operator[](std::size_t idx)       { return meshes[idx]; };
    const oMesh& operator[](std::size_t idx) const { return meshes[idx]; };

};
#endif