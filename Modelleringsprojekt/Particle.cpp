#include <iostream>
#include "Particle.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

//Constructor for a particle
void Particle::CreateParticle(){

	pos = vec3(fmod(rand(),512), fmod(rand(),512), 0.5);
	vel = vec3(2 - fmod(rand(),5), 2 - fmod(rand(),5), 2 - fmod(rand(), 5));

	mass = 1.0f;
	gravity = 9.82f;
	radius = 3.0f;

}

void Particle::EvolveParticle(){

	vel[1] -= gravity * 0.01f;
	pos[1] += vel[1];

	if (pos[0] <= 0 || pos[0] > 512) {
		vel[0] = -vel[0];
	}
	if (pos[1] <= 0 || pos[1] > 512) {
		vel[1] = -vel[1];
		pos[1] = pos[1] < 0 ? 0 : pos[1];
	}

	pos[0] += vel[0];

}

void Particle::DrawObjects(){
	glColor3f(1,0,0);
	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0.0, 1.0); glVertex3f(pos[0] + 3, pos[1] + 3, pos[2]);     // top    right
	glTexCoord2f(0.0, 1.0); glVertex3f(pos[0] - 3, pos[1] + 3, pos[2]);     // top    left
	glTexCoord2f(0.0, 1.0); glVertex3f(pos[0] + 3, pos[1] - 3, pos[2]);     // bottom right
	glTexCoord2f(0.0, 1.0); glVertex3f(pos[0] - 3, pos[1] - 3, pos[2]);     // bottom left

	glEnd();

}

void Particle::setCellIndex(){

}

int Particle::getCellIndex(){
	return cellIndex;
}