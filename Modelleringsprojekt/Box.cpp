/*
 *  Box.cpp
 *  Graphics Assignment Four
 *
 *  Created by Karl Grogan on 27/04/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Box.h"
#include <GLFW/glfw3.h>

// Moves the box 512 pixels every second. When it reaches teh other side of the window
// it goes back to the start.

Box::Box()
{
	boxX = 0;
	boxY = 0;
	rotAngle = 0;	
	lastFrameTime = 0;
}

void Box::DrawBox()
{
	glPushMatrix();
	//glTranslatef(64, 64, 0);
	// Changes the position of the box x, y, or z directions. 
	//glTranslatef(boxX, boxY, 0.0f);
	// Rotates the box about the x,y or z axis depending where the '1' is specified in the parameters.
	// The first parameter is the rotation angle.
	//glRotatef(rotAngle, 0, 0, 1);
	
	glBegin(GL_LINES);
		
    // Defines each line

	// Define grid by doing fancy iterations.
	glColor3f(0.2f, 0.2f, 0.2f);
    
    for (int i = 1; i < 256 / 16; i++) {
        
        // Side1
        
        glVertex3f(16.f * i, 0, 0);
        glVertex3f(16.f * i, 256, 0);
        
        glVertex3f(0, 16.f * i, 0);
        glVertex3f(256, 16.f * i, 0);
        
        // Side2
        
        glVertex3f(0, 0, 16.f * i);
        glVertex3f(0, 256, 16.f * i);
        
        glVertex3f(0, 16.f * i, 0);
        glVertex3f(0, 16.f * i, 256);
        
        // Side3
        
        glVertex3f(16.f * i, 0, 256);
        glVertex3f(16.f * i, 256, 256);
        
        glVertex3f(0, 16.f * i, 256);
        glVertex3f(256, 16.f * i, 256);
        
        // Side 4
        
        glVertex3f(256, 0, 16.f * i);
        glVertex3f(256, 256, 16.f * i);
        
        glVertex3f(256, 16.f * i, 0);
        glVertex3f(256, 16.f * i, 256);
        
        // Bottom
        
        glVertex3f(0, 0, 16.f * i);
        glVertex3f(256, 0, 16.f * i);
        
        glVertex3f(16.f * i, 0, 0);
        glVertex3f(16.f * i, 0, 256);

        
    }
/*
    glVertex2f(0.0f, 64.0f);
    glVertex2f(512.0f, 64.0f);
    
    glVertex2f(10.0f, 10.0f);
    glVertex2f(502.0f, 10.0f);
    
    glVertex2f(502.0f, 10.0f);	//bottom right
    glVertex2f(502.0f, 502.0f ); //top right
    
    glVertex2f(502.0f, 502.0f);
    glVertex2f(10.0f, 500.0f);
	*/

    glEnd();
	
	glPopMatrix();
	
}
