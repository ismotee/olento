//
//  oCamera.h
//  opengltest
//
//  Created by Ismo Torvinen on 25.3.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef __opengltest__oCam__
#define __opengltest__oCam__

#include "olento.h"

namespace oCamera {

	glm::vec3 position;

    GLuint MatrixID, ViewMatrixID, ModelMatrixID;
    
    void init(GLuint& programID, float aspectRatio = 1.0);
	void update(glm::vec3 lookAt = glm::vec3(0,0,0) );

    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
    glm::mat4 MVP;
    // later on more functions: like moving, tilting, panning, zooming etc..

};

void oCamera::init(GLuint& programID, float aspectRatio) {

	position = glm::vec3(8, 3, 3);

    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
	
    // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
    
    // Camera matrix
    View = glm::lookAt(
                                 position, // Camera is at (4,3,3), in World Space
                                 glm::vec3(0, 0, 0), // and looks at the origin
                                 glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
                                 );
    // Model matrix : an identity matrix (model will be at the origin)
    Model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
    
}

void oCamera::update(glm::vec3 lookAt) {

    // Camera matrix
    View = glm::lookAt(
                       position, 
                       lookAt,
                       glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    
    // Model matrix : an identity matrix (model will be at the origin)
    //Model = glm::mat4(1.0f);
    
    // Our ModelViewProjection : multiplication of our 3 matrices
    MVP = Projection * View * Model;
    
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

}


#endif /* defined(__opengltest__oCamera__) */
