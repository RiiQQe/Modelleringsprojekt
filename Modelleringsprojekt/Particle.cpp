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
	vel = vec3(2 - (int)rand() % 5, 2 - (int)rand() % 5, 0);

	mass = 1.0f;
	gravity = 9.82f;
	radius = 20.0f;
	special = false;
}

//Evolves the particle parameters over time.
//This method changes the vertical and horizontal poition of the particle

void Particle::EvolveParticle(){
	// Move the particle
	pos[0] += vel[0];
	pos[1] += vel[1];

	if (pos[0] < 0 || pos[0] > 511) {
		pos[0] = pos[0] / 511 < 0.5 ? 0 : 511;
        vel[0] = -vel[0];
	}

	if (pos[1] < 0 || pos[1] > 511) {
		pos[1] = pos[1] / 511 < 0.5 ? 0 : 511;
        vel[1] = -vel[1];
	}
}

//Draw all the particles
void Particle::DrawObjects() {
    glBegin(GL_TRIANGLE_STRIP);
	    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+3, pos[1]+3,pos[2]);     // top    right
   		glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-3, pos[1]+3,pos[2]);     // top    left
    	glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+3, pos[1]-3,pos[2]);     // bottom right
    	glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-3, pos[1]-3,pos[2]);     // bottom left
    glEnd();
}

const glm::vec3 &Particle::getPos() const {
	return pos;
}	

int Particle::getCellIndex() {
	glm::vec2 cell = glm::vec2(glm::floor(pos[0] / 512.f * (512 / 32)), glm::floor(pos[1] / 512.f * (512 / 32)));
	int _cellIndex = (int)cell.x % 16 + (int)cell.y * 16;
	
	return _cellIndex;
}

void Particle::addToVel(glm::vec2 v) {
    vel.x += v.x;
    vel.y += v.y;
}

glm::vec2 Particle::getVel() {
    return glm::vec2(vel.x, vel.y);
}
