//
//  dynamicVertex.h
//  opengltest
//
//  Created by Ismo Torvinen on 17.3.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//
/*
#ifndef opengltest_dynamicVertex_h
#define opengltest_dynamicVertex_h


#include <vector>
#include "glm/glm.hpp"

class dFace;
class dObject;

class dFace {
    // dFace heavily depends on parent dObject. All the vertexes and normals should be there
    // in dObject lists. dFace class exists just because it is easier to calculate vertex normals
    // when all the information can be found in the same class.
    // NOTE: Because of the references, vace is highly dependent of the parents life cycle.
    // PLEASE DO NOT USE dFace OUTSIDE THE dObject!!!!!
    std::vector<unsigned int> vertsIds;
    std::vector<glm::vec3>& verts;

public:
    glm::vec3 faceNormals;
    
    dFace (std::vector<unsigned int> _vertsIds, std::vector<glm::vec3>& _vertsList):
                vertsIds(_vertsIds),  // verts ids to find correct verts from a list. Same places for the normals.
                verts(_vertsList) // reference for verts
    {
    
    }
    
    void calculateFaceNormals () {
        
        glm::vec3 vect1;
        glm::vec3 vect2;
        
        vect1 = verts[vertsIds[1]]-verts[vertsIds[0]];
        vect2 = verts[vertsIds[2]]-verts[vertsIds[0]];
        
        faceNormals = glm::cross(vect1,vect2);
        
    }
    
};

class dObject
{

    std::vector<glm::vec3> vertices;
    std::vector<std::vector<unsigned int> > facesConnectedToVertices;
    std::vector<dFace> faces;
    std::vector<glm::vec3> normals;
    
public:
    dObject (std::vector<glm::vec3> _vertices, std::vector<unsigned int> _elements): vertices(_vertices), faces(),normals()
    {
        // make faces from elements
        int i = 0;
        while (i < _elements.size()) {
        
            std::vector<unsigned int> ids;
            ids.push_back(_elements[i++]);
            ids.push_back(_elements[i++]);
            ids.push_back(_elements[i++]);
            faces.push_back(dFace(ids,vertices));
        }
        
        
        //find connected faces per vertex
        
        for(int j = 0; j < vertices.size(); j++)
        {
            std::vector<unsigned int> connectedFaces;

            
            for(int k = 0; k < _elements.size();k++)
            {
                if(_elements[k] == j) {
                    connectedFaces.push_back(k/3);
                }
            }
            
            facesConnectedToVertices[j] = connectedFaces;
            
        }
        
        normals = vertices;
        
        calculateAllNormals();

        
 
        
    }

    void calculateAllNormals ()
    {
        //calculate faceNormals
        for (int j = 0; j < faces.size();j++)
        {
            faces[j].calculateFaceNormals();
        }
        
        // calculate vertex normals
        for (int j = 0; j < facesConnectedToVertices.size() ;j++)
        {
            glm::vec3 sumOfNormals = glm::vec3(0,0,0);
            
            for (int k = 0; k < facesConnectedToVertices[j].size(); k++)
            {
                unsigned int faceId = facesConnectedToVertices[j][k];
                
                sumOfNormals = sumOfNormals + faces[faceId].faceNormals;
                
            }
            
            normals[j] = glm::normalize(sumOfNormals);
        }

    
    }
    
    
};



class dVertex {
    
public:
    unsigned int vertexId;
    glm::vec3 vertex;
    glm::vec3 normal;
    std::vector<unsigned int> connectedFacesIndices;
    //dObject& parent;
    
    dVertex(unsigned int Id, glm::vec3 vert, dObject& _parent);
 
    
    
    
};
*/



/*
class dObject {

public:
    dObject(std::vector<glm::vec3> vertices, std::vector<unsigned int> elements) {
    
        for(int i = 0; i < vertices.size(); i++)
            dVertices.push_back(dVertex(i,vertices[i],*this));
        
        faces = elements;
        
        CalculateNormals();
        
    }
    
    std::vector<dVertex> dVertices;
    
    std::vector<unsigned int> faces;
    // faces consists a list of indices to vertexes, every face is triangle. Take note
    // when searching connected faces or get information with "facesId"
    // facesId needs to be multiplied by 3 in order to get the right vertices.
    // when writing facesId naturally id will be divided by 3.
    
    std::vector<glm::vec3> AllVertices () {
    
        std::vector<glm::vec3> result;
        for(int i = 0; i < dVertices.size();i++)
            result.push_back(dVertices[i].vertex);
        return result;
    }

    
    std::vector<glm::vec3> AllNormals () {
        
        std::vector<glm::vec3> result;
        for(int i = 0; i < dVertices.size();i++)
            result.push_back(dVertices[i].normal);
        return result;
    }

    
    std::vector<unsigned int> getConnectedFaces (unsigned int Id) {
    
        std::vector<unsigned int> result;
        
        //go through all the vertex indices and mark all faces that consist id
        for(int i = 0; i < faces.size();i ++) {
            if(faces[i] == Id) {
                 result.push_back(i/3); // i is divided by 3 to get the "facesId"
            }
        }
    return result;
    }
                 
    void CalculateNormals() {
        for(int i = 0; i < dVertices.size();i++)
            dVertices[i].normal = calculateVertexNormal(i);
    }
                 
    glm::vec3 calculateVertexNormal (unsigned int vertId) {
    
        glm::vec3 FaceNormalsSum(0,0,0);
        
        for(int i = 0; i < dVertices[vertId].connectedFacesIndices.size(); i++)
            FaceNormalsSum += calculateFaceNormal(dVertices[vertId].connectedFacesIndices[i]);
                    // sorry for that
    
        return glm::normalize(FaceNormalsSum);
        
    }
                 
    glm::vec3 calculateVertexNormal (std::vector<unsigned int> facesIds) {
                     
            glm::vec3 FaceNormalsSum(0,0,0);
        
            for(int i = 0; i < facesIds.size(); i++)
                FaceNormalsSum += calculateFaceNormal(facesIds[i]);
                     // much better!!
        
        glm::vec3 N = glm::normalize(FaceNormalsSum);
        
        N.x *= -1;
        N.y *= -1;
        N.z *= -1;
        return N;
    }

            
    glm::vec3 calculateFaceNormal (unsigned int facesId) {
        glm::vec3 f;
        glm::vec3 s;
        glm::vec3 t;
        
        // again, facesId needs to be multiplied by 3 to get vertices
        f = dVertices[facesId * 3].vertex;
        s = dVertices[facesId * 3 + 1].vertex;
        t = dVertices[facesId * 3 + 2].vertex;
        
        glm::vec3 N = glm::cross(s - f,t - f);
        
        return N;
    }

};
            

dVertex::dVertex(unsigned int Id, glm::vec3 vert, dObject& _parent) :
vertexId(Id),
vertex(vert),
parent(_parent)
{
    connectedFacesIndices = parent.getConnectedFaces(vertexId);
}


#endif
*/