//oBuffers.h
#ifndef OBUFFERS_H
#define OBUFFERS_H
#include <vector>
#include "olento.h"
#include "dobject.h"

namespace oBuffers
{
	GLuint vertexbuffer, normalbuffer, elementbuffer;
	GLuint VertexArrayID;

	int element_n;

	void init();
	void setElements(dObject& obj);
	void close();

	void updateBuffers(dObject& obj);
	void updateAttributes();
};

void oBuffers::init() {

	// VAO
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &normalbuffer);
	glGenBuffers(1, &elementbuffer);

}


void oBuffers::setElements(dObject& obj) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.getElementData().length, obj.getElementData().data, GL_STATIC_DRAW);

	element_n = obj.getElementData().length;
}



void oBuffers::updateBuffers(dObject& obj)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj.getVertexData().length, obj.getVertexData().data, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj.getNormalData().length, obj.getNormalData().data, GL_DYNAMIC_DRAW);
}


void oBuffers::updateAttributes() {
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);


	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	// 2nd attribute buffer : normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	glDrawElements(GL_TRIANGLES, element_n, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


void oBuffers::close() {
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}

#endif

