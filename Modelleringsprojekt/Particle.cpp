#include <iostream>
#include "Particle.h"
#include <stdlib.h>
#include <GLUT/glut.h>
#include <GLFW/glfw3.h>

// Constructor for a particle.
void Particle::CreateParticle()
{
    xpos= 100 + (rand() % (int)(400 - 99));
	ypos= 256.0;
	zpos= 0.5;
	xspeed = 2-(int)rand() % 5 ;
	yspeed = 2-(int)rand() % 5  ;
	zspeed = 2-(int)rand() % 5  ;
}
	

//Evolves the particle parameters over time.
//This method changes the vertical and horizontal poition of the particle

void Particle::EvolveParticle()
{
    if(xpos > 10 && xpos < 500 && ypos > 10 && ypos < 500){
        
        //std::cout << (int)rand() % 5 << "   ";
    
		xpos+= 2 - (int)rand()  % 5 ;
		ypos+= 2 - (int)rand()  % 5 ;
        //ypos -= 9.82*glfwGetTime();;
		//yspeed+= 2 - (int)rand()  % 5 ;
        
    }

    
}

//Draw all the particles
void Particle::DrawObjects()
{

    glColor3f(1,1,1);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0,1.0); glVertex3f(xpos+3, ypos+3,zpos);     // top    right
    glTexCoord2f(0.0,1.0); glVertex3f(xpos-3, ypos+3,zpos);     // top    left
    glTexCoord2f(0.0,1.0); glVertex3f(xpos+3, ypos-3,zpos);     // bottom right
    glTexCoord2f(0.0,1.0); glVertex3f(xpos-3, ypos-3,zpos);     // bottom left
    
    glEnd();
	
}


float Particle::GetXPos()
{
	return xpos;
}


float Particle::GetYPos()
{
    return ypos;
}


void Particle::SetXPos(float xPos)
{
	xpos = xPos;
}


void Particle::SetYPos(float yPos)
{
	xpos = yPos;
}
