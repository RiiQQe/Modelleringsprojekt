#include "Box.h"
#include <GLFW/glfw3.h>


int Box::maxX = 512;

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
    
    GLfloat blue[] = {0.2, 0.6, 1.0, 1.0};
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 60.0);
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
        
        // Bottom to top
        
        for (int j = 0; j <= 256 / 16; j++){
        
            glVertex3f(0, 16.f * j, 16.f * i);
            glVertex3f(256, 16.f * j, 16.f * i);
        
            glVertex3f(16.f * i, 16.f * j, 0);
            glVertex3f(16.f * i, 16.f * j, 256);
        }
    
        
        // Top
        /*
        glVertex3f(0, 256, 16.f * i);
        glVertex3f(256, 256, 16.f * i);
        
        glVertex3f(16.f * i, 256, 0);
        glVertex3f(16.f * i, 256, 256);
        */
        
    }

    glEnd();
	
	glPopMatrix();
	
}
