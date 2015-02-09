#include <iostream>
#include "Particle.h"
#include "Cell.h"
#include <stdlib.h>
//#include <GLUT/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;


// Constructor for a particle.
void Particle::CreateParticle()
{
	pos = vec3(fmod(rand() , 512), fmod(rand(), 512), 0.5f);
	vel = vec3(2 - (int)rand() % 5, 2 - (int)rand() % 5, 2 - (int)rand() % 5 );

	mass = 1.0f;
	gravity = 9.82f;
	radius = 3.0f;
}
	

//Evolves the particle parameters over time.
//This method changes the vertical and horizontal poition of the particle

void Particle::EvolveParticle()
{
	vel[1] -= gravity * 0.01f;
	pos[1] += vel[1];
	//std::cout << "POS X " << pos[0] << std::endl;

	//Just to make sure position isnt outside the bounderies 
	if (pos[0] <=  1 || pos[0] >= 511) {
		vel[0] = -vel[0];
	}
	if (pos[1] <= 0 || pos[1] >= 512) {
		vel[1] = -vel[1];// * 0.7;
		pos[1] = pos[1] < 0 ? 0 : pos[1];
	}
	pos[0] += vel[0];
	//std::cout << "POS X AFTER " << pos[0] << std::endl
}

//Draw all the particles
void Particle::DrawObjects() {
	glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+3, pos[1]+3,pos[2]);     // top    right
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-3, pos[1]+3,pos[2]);     // top    left
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+3, pos[1]-3,pos[2]);     // bottom right
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-3, pos[1]-3,pos[2]);     // bottom 
    glEnd();
}

glm::vec3 Particle::getPos() {
	return pos;
}	

glm::vec2 Particle::getCell() {
	//std::cout << " GLM::VEC2 X  " << glm::floor(pos[0] / 512.f * (512 / 32)) << " Y  " << glm::floor(pos[1] / 512.f * (512 / 32)) << std::endl;
	return glm::vec2(glm::floor(pos[0] / 512.f * GRID_WIDTH ), glm::floor(pos[1] / 512.f * GRID_HEIGHT));
}

//void Particle::setSpecial() {
//	special = true;
//}

//void Particle::setCellIndex() {
//	glm::vec2 cell = getCell();
//	cellIndex = (int)cell.x % 16 + (int)cell.y * 16;
//}

int Particle::getCellIndex() {
	glm::vec2 cell = getCell();
	cellIndex = (int)cell.x % GRID_WIDTH + (int)cell.y * GRID_HEIGHT;
	return cellIndex;
}
