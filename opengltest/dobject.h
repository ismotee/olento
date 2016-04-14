#ifndef opengltest_dobject_h
#define opengltest_dobject_h

#include <vector>
#include <iostream>
#include <time.h>
#include "oLoader.h"
#include "dFace.h"

class dFace;
class dFacesConnected;
class dObject;

class dClock{
public:
	clock_t lastTime;
	
	void reset() {lastTime = clock();	}
	
	float get() {
		clock_t time = clock() - lastTime;
		return ((float)time / CLOCKS_PER_SEC);
	}

	dClock(){
		reset();
	}

};



//===================================== dFacesConnected =================================//
//=======================================================================================//


class dFacesConnected {
    std::vector<unsigned int> faceIds;
    
public:
    
	void addFace(unsigned int Id) {
		faceIds.push_back(Id);
	}

    dFacesConnected(unsigned int vertexId,
                    std::vector<unsigned int> elements,
                    std::vector<dFace>& faces,
                    std::vector<glm::vec3> normals)
                    {   
        // get references from faces by elements and vertexId
        for(int i = 0; i < elements.size(); i++) {
            if (elements[i] == vertexId) {
                faceIds.push_back(i/3);
            }
        }
    }


	dFacesConnected() {}
    

	glm::vec3 calculateVertexNormal(std::vector<dFace>& allFaces) {
		glm::vec3 sum = glm::vec3(0, 0, 0);

		for (int i = 0; i < faceIds.size(); i++) 
			sum += allFaces[faceIds[i]].faceNormal;

		return glm::normalize(sum);
	}
};

//======================================== dObject =========================================//
//==========================================================================================//
//                                                                                          //
// dObject is object for openGl. The goal is to make class that is easy read and handle     //
// with main openGl's loop.                                                                 //
// dObject needs to be able to calculate normals on the go. That is made in the subclasses  //
// dFace and dFaceConnected. These two calsses are reference classes with one               //
// or two functions.                                                                        //
//                                                                                          //
//==========================================================================================//

class dObject : public oLoader {
public:
    dObject(std::string path);
    dObject(std::vector<glm::vec3> _vertices, std::vector<unsigned int> _elements);
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> elements;
    
    void calculateAllNormals();
    void changeVertices(std::vector<glm::vec3>& new_vertices);
    void changeVerticesTowards(std::vector<glm::vec3>& aim_vertices, float multi);
    
    std::vector<dFace> faces;
    std::vector<dFacesConnected> vFacesConnected;

private:
  
    void makeFaces();
    void makeFacesConnected();
    
};

dObject::dObject (std::string path)

{
    dClock c;
    oLoader::loadOBJ(path, vertices, elements, faces);
    normals = vertices;
    std::cout << "took " << c.get() << " s\n";
    
    //makeFaces();
    makeFacesConnected();
    
    calculateAllNormals();
    
}


dObject::dObject (std::vector<glm::vec3> _vertices, std::vector<unsigned int> _elements):
                    vertices(_vertices),
                    normals(_vertices),
                    elements(_elements)

{
    makeFaces();
    makeFacesConnected();
    
    calculateAllNormals();
    
}

void dObject::changeVertices(std::vector<glm::vec3>& new_vertices)
{
    vertices = new_vertices;
    calculateAllNormals();
}

void dObject::makeFaces() //t�t� pit�isi optimoida!
{
	dClock c;
	std::cout << "makeFaces...";

	for (std::vector<unsigned int>::iterator it = elements.begin();
			it != elements.end();
			it += 3) {
        
		std::vector<unsigned int> ids(it, it+3);
		faces.push_back(dFace(ids));
    }
    
	std::cout << " ok ("<<c.get()<< " s)\n";
}

void dObject::makeFacesConnected()
{
	dClock c;

	std::cout << "makeFacesConnected... ";
	vFacesConnected.resize(vertices.size());

	for (int i = 0; i < elements.size(); i++) {
		vFacesConnected[elements[i]].addFace(i / 3);
	}
	std::cout << "ok (" << c.get() << " s)\n";
}


void dObject::calculateAllNormals()
{
	//std::cout << "calculateAllNormals: vFacesConnected.size = " << vFacesConnected.size() << "\n";
	dClock c;
    for(int i = 0; i < faces.size(); i++) {
        faces[i].calculateFaceNormal(vertices);
    }
    
	//std::cout << "face normals done (took "<< c.get() <<" s)\n";
	c.reset();
    for(int i = 0; i < vFacesConnected.size() && i < normals.size(); i++) {
        normals[i] = vFacesConnected[i].calculateVertexNormal(faces);
    }

	//std::cout << "vertex normals done (took " << c.get() << " s)\n";
}

void dObject::changeVerticesTowards(std::vector<glm::vec3>& aim_vertices, float multi)
{
    if(multi > 1) multi = 1;
    if(multi < 0) multi = 0;
    
    for(int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = glm::mix(vertices[i],aim_vertices[i], multi);
    }
}


#endif
