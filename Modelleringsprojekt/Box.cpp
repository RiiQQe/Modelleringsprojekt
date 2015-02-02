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
    glVertex2f(10.0f, 500.0f);
    glVertex2f(10.0f, 10.0f);
    
    glVertex2f(10.0f, 10.0f);
    glVertex2f(502.0f, 10.0f);
    
    glVertex2f(502.0f, 10.0f);	//bottom right
    glVertex2f(502.0f, 502.0f ); //top right
    
    glVertex2f(502.0f, 502.0f);
    glVertex2f(10.0f, 500.0f);
	
    glEnd();
	
	glPopMatrix();
	
}
