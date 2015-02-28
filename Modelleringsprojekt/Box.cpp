#include "Box.h"
#include <GLFW/glfw3.h>

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
	glBegin(GL_LINES);
    
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
        
        // Top
        
    /*    glVertex3f(0, 256, 16.f * i);
        glVertex3f(256, 256, 16.f * i);
        
        glVertex3f(16.f * i, 256, 0);
        glVertex3f(16.f * i, 256, 256);*/


        
    }

    glEnd();
	
	glPopMatrix();
	
}
