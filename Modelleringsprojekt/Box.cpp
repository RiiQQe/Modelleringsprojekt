#include "Box.h"
#include <GLFW/glfw3.h>

Box::Box()
{
	boxX = 0;
	boxY = 0;
	rotAngle = 0;	
	lastFrameTime = 0;
}

void Box::DrawBox(int size, int colwidth)
{
	glPushMatrix();
	glBegin(GL_LINES);
    
	// Define grid by doing fancy iterations.
	glColor3f(0.2f, 0.2f, 0.2f);
   
    for (int i = 1; i < size / colwidth; i++) {
        
        // Side1
        
        glVertex3f(colwidth * i, 0, 0);
        glVertex3f(colwidth * i, size, 0);
        
        glVertex3f(0, colwidth * i, 0);
        glVertex3f(size, colwidth * i, 0);
        
        // Side2
        
        glVertex3f(0, 0, colwidth * i);
        glVertex3f(0, size, colwidth * i);
        
        glVertex3f(0, colwidth * i, 0);
        glVertex3f(0, colwidth * i, size);
        
        // Side3
        glVertex3f(colwidth * i, 0, size);
        glVertex3f(colwidth * i, size, size);
        
        glVertex3f(0, colwidth * i, size);
        glVertex3f(size, colwidth * i, size);
        
        // Side 4
        
        glVertex3f(size, 0, colwidth * i);
        glVertex3f(size, size, colwidth * i);
        
        glVertex3f(size, colwidth * i, 0);
        glVertex3f(size, colwidth * i, size);
        
        // Top
        
    /*    glVertex3f(0, size, colwidth * i);
        glVertex3f(size, size, colwidth * i);
        
        glVertex3f(colwidth * i, size, 0);
        glVertex3f(colwidth * i, size, size);*/
        
    }
	glEnd();

	glColor3f(1.0,0.0,0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(0,0,256);
	glVertex3f(256,0,256);
	glVertex3f(256, 0, 0);
	glVertex3f(0,0,0);
	glEnd();
	
	glPopMatrix();
	
}
