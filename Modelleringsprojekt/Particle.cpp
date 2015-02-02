#include <iostream>
#include "Particle.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Constructor for a particle.
void Particle::CreateParticle()
{

    pos = glm::vec2(100 + (rand() % (int)(400-99)), rand() % 512);
    vel = glm::vec2(2-(int)rand() % 5, 2-(int)rand() % 5);
    
    mass = 1.0f;
    gravity = 9.82f;
    density = 0;
    
}

void Particle::EvolveParticle()
{
    // Check boundaries
    if(pos.x > 10 && pos.x < 500 && pos.y > 10 && pos.y < 500){
        
        //std::cout << (int)rand() % 5 << "   ";
    
		//pos.x+= 2 - (int)rand()  % 5 ;
		//pos.y+= 2 - (int)rand()  % 5 ;
        
        vel.y -= gravity * 0.01f;
        pos.y += vel.y;
        pos.x += vel.x;
        
    }

    
}

//Draw all the particles
void Particle::DrawObjects()
{

    glColor3f(1,1,1);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0,1.0); glVertex3f(pos.x+3, pos.y+3, 0.5);     // top    right
    glTexCoord2f(0.0,1.0); glVertex3f(pos.x-3, pos.y+3, 0.5);     // top    left
    glTexCoord2f(0.0,1.0); glVertex3f(pos.x+3, pos.y-3, 0.5);     // bottom right
    glTexCoord2f(0.0,1.0); glVertex3f(pos.x-3, pos.y-3, 0.5);     // bottom left
    
    glEnd();
	
}


glm::vec2 Particle::getPos()
{
    return pos;
}

void Particle::setPos(float xPos, float yPos){

    pos.x = xPos;
    pos.y = yPos;

}

void setPressure(float value){

    pressure = value;
}