#include "oMesh.h"

oMesh::oMesh(int size) {
    vertices.resize(size);


}

oMesh::oMesh (std::string path)
{
    oLoader::loadOBJ(path,vertices);
}


oMesh& oMesh::operator-=(const oMesh& rhs)
{
    std::cout << "rhs: " << rhs.vertices.size() << " target: " << vertices.size() << "\n";
    for(int i = 0; i < rhs.vertices.size();i++){
        vertices[i] = vertices[i] - rhs.vertices[i];
    }
    
    return *this; // return the result by reference
}