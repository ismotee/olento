//
//  oMeshBundle.h
//  opengltest
//
//  Created by Ismo Torvinen on 27.3.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef opengltest_oMeshBundle_h
#define opengltest_oMeshBundle_h

#include "olento.h"
#include <vector>
#include "oMesh.h"
#include "oDirectory.h"

std::vector<glm::vec3> lerpVectors (std::vector<glm::vec3>& verts1, std::vector<glm::vec3>& verts2, float value) {

    std::vector<glm::vec3> result(verts1.size());
    
    for(int i = 0; i < result.size();i++) {
    
        result[i] = glm::mix(verts1[i],verts2[i],value);
    
    }

    return result;
    
};

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

oMeshBundle::oMeshBundle (oDirectory& dir)
{
    for(int i = 0; i < dir.files.size(); i++)
        meshes.push_back(oMesh(dir.files[i]));
}



oMeshBundle::oMeshBundle (oDirectory& dir, oMeshBundle& archtypes) {
    int j = 1;
    
    for(int i = 0; i < dir.files.size(); i++) {
        if (++j % 4 == 0) {
            meshes.push_back(archtypes[0].vertices.size());
            std::cout << "writing 0mesh\n";
        }
        std::cout << "push_back...\n";
        meshes.push_back(oMesh(dir.files[i]));
        std::cout << "minus archtype...  ";
        meshes[meshes.size() - 1] -= archtypes[i/4];
        std::cout << "ok!\n";
    }
}

std::vector<glm::vec3> oMeshBundle::getShape (float value) {

    int base = (int) value;
    
    /*
    std::vector<glm::vec3> result = meshes[base].vertices;
    
    
        for(int i = 0; i < meshes[base].vertices.size();i++) {
        result[i] = glm::mix(meshes[base].vertices[i],meshes[upper].vertices[i], value - base);
    }
    */
    
    return lerpVectors(meshes[base].vertices, meshes[base + 1].vertices,value - base);
}

std::vector<glm::vec3> oMeshBundle::getShape (float a_value,float m_value) {
    
    int a_multiplier = (int) a_value;
    
    float m_valueScaled = (m_value + 1) * 2;
    int m_valueScaledInt = (int) m_valueScaled;
    
    std::vector<glm::vec3> mesh1 = lerpVectors(meshes[5 * a_multiplier + (int)m_valueScaled].vertices,
                                               meshes[5 * (a_multiplier+1) + (int)m_valueScaled].vertices,
                                               (float)a_value - a_multiplier);
    
    std::vector<glm::vec3> mesh2 = lerpVectors(meshes[5 * a_multiplier + (int)m_valueScaled + 1].vertices,
                                            meshes[5 * (a_multiplier+1) + (int)m_valueScaled + 1].vertices,
                                               (float)a_value - a_multiplier);
    
    return lerpVectors(mesh1,mesh2,m_valueScaled - m_valueScaledInt);
}

#endif
