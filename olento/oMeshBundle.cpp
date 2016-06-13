#include "oMeshBundle.h"


std::vector<glm::vec3> lerpVectors (std::vector<glm::vec3>& verts1, std::vector<glm::vec3>& verts2, float value) {

	int size = verts1.size();
	if (size > verts2.size()) size = verts2.size();
    std::vector<glm::vec3> result(size);
    
    for(int i = 0; i < size;i++) {
    
        result[i] = glm::mix(verts1[i],verts2[i],value);
    
    }

    return result;
    
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
        }
        meshes.push_back(oMesh(dir.files[i]));
        meshes[meshes.size() - 1] -= archtypes[i/4];
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
