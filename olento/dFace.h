#include <vector>

#ifndef DFACE_H
#define DFACE_H

#include "glm/glm.hpp"

class dFace 
{
public:

	std::vector<unsigned int> vertsIds; //facen verteksit indekseinä dObjectin verteksidataan
    
    glm::vec3 faceNormal;
    
    dFace(std::vector<unsigned int> _vertsIds) : vertsIds(_vertsIds) {}
    
	dFace(unsigned int* _vertsIds) : vertsIds(_vertsIds,_vertsIds + 3) {}
    

    void calculateFaceNormal(std::vector<glm::vec3>& allVerts)
    {
        glm::vec3 verts[3] = { allVerts[vertsIds[0]], allVerts[vertsIds[1]], allVerts[vertsIds[2]] };
        
        glm::vec3 vect1;
        glm::vec3 vect2;
        
        vect1 = verts[1] - verts[0];
        vect2 = verts[2] - verts[0];
        
        faceNormal = glm::cross(vect1, vect2);
    }
};


#endif
