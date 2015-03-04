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
    
    for (int i = 1; i < 512 / 32; i++) {
        
        // Side1
        
        glVertex3f(32.f * i, 0, 0);
        glVertex3f(32.f * i, 512, 0);
        
        glVertex3f(0, 32.f * i, 0);
        glVertex3f(512, 32.f * i, 0);
        
        // Side2
        
        glVertex3f(0, 0, 32.f * i);
        glVertex3f(0, 512, 32.f * i);
        
        glVertex3f(0, 32.f * i, 0);
        glVertex3f(0, 32.f * i, 512);
        
        // Side3
        
        glVertex3f(32.f * i, 0, 512);
        glVertex3f(32.f * i, 512, 512);
        
        glVertex3f(0, 32.f * i, 512);
        glVertex3f(512, 32.f * i, 512);
        
        // Side 4
        
        glVertex3f(512, 0, 32.f * i);
        glVertex3f(512, 512, 32.f * i);
        
        glVertex3f(512, 32.f * i, 0);
        glVertex3f(512, 32.f * i, 512);
        
        // Bottom
        
        glVertex3f(0, 0, 32.f * i);
        glVertex3f(512, 0, 32.f * i);
        
        glVertex3f(32.f * i, 0, 0);
        glVertex3f(32.f * i, 0, 512);
        
        // Top
        
    /*    glVertex3f(0, 512, 32.f * i);
        glVertex3f(512, 512, 32.f * i);
        
        glVertex3f(32.f * i, 512, 0);
        glVertex3f(32.f * i, 512, 512);*/


        
    }

    glEnd();
	
	glPopMatrix();
	
}
