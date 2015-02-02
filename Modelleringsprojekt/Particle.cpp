#include <iostream>
#include "Particle.h"
#include <stdlib.h>
#include <GLUT/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

// Constructor for a particle.
void Particle::CreateParticle()
{
	pos = vec3(rand() % 512, rand() % 512, 0.5);
	vel = vec3(2 - (int)rand() % 5, 2 - (int)rand() % 5, 2 - (int)rand() % 5 );

	mass = 1.0f;
	gravity = 9.82f;
	radius = 3.0f;
	special = false;
}
	

//Evolves the particle parameters over time.
//This method changes the vertical and horizontal poition of the particle

void Particle::EvolveParticle()
{
	vel[1] -= gravity * 0.01f;
	pos[1] += vel[1];

	if (pos[0] <  0 || pos[0] > 512) {
		vel[0] = -vel[0];
	}
	if (pos[1] < 0 || pos[1] > 512) {
		vel[1] = -vel[1];// * 0.7;
		pos[1] = pos[1] < 0 ? 0 : pos[1];
	}

	pos[0] += vel[0];
}

//Draw all the particles
void Particle::DrawObjects()
{

    !special ? glColor3f(1,0,0) :  glColor3f(1,1,1);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+3, pos[1]+3,pos[2]);     // top    right
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-3, pos[1]+3,pos[2]);     // top    left
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+3, pos[1]-3,pos[2]);     // bottom right
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-3, pos[1]-3,pos[2]);     // bottom left
    
    glEnd();
	
}

glm::vec3 Particle::getPos() {
	return pos;
}	


void Particle::SetXPos(float xPos)
{
	pos[0] = xPos;
}


void Particle::SetYPos(float yPos)
{
	pos[1] = yPos;
}

glm::vec2 Particle::getCell() {
	return glm::vec2(glm::floor(pos[0] / 512.f * (512 / 32)), glm::floor(pos[1] / 512.f * (512 / 32)));
}

void Particle::setSpecial() {
	special = true;
}
