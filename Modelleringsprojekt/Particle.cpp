#define _USE_MATH_DEFINES

#include <iostream>
#include "Particle.h"
#include "Cell.h"
#include <stdlib.h>
//#include <GLUT/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;
int STEP = 0;
float X = 0;
const float dt = 0.1f;
const float VISCOSITY =3.5;
const float PARTICLE_MASS = 0.002;
const float p_size = 4.f;

// Constructor for a particle.
void Particle::CreateParticle()
{
	if (STEP % 100 == 0)
		X = X + 5;
	pos = vec3(40.f + STEP , 200.f + X, 0.5f);
	if (STEP > 100)
		STEP = 0;
	else
		STEP = STEP + 9.f;
	

	vel = vec3(0.f, 0.f, 0.f);
	//vel = vec3(2 - (int)rand() % 5, 2 - (int)rand() % 5, 2 - (int)rand() % 5 );

	gravity_force = glm::vec3(0.0f, -9.82f, 0.0f);
	pressure_force = glm::vec3(0.f, 0.f, 0.f);
	viscosity_force = glm::vec3(0.f, 0.f, 0.f);
	density = 0.f;
	pressure = 0.f;

	mass = 1.0f;
	gravity = 9.82f;
	radius = 3.0f;
}
	

void Particle::addDensity(float &w)
{
	density =  w;
}

const float Particle::getDensity() const{
	return density;
}

void Particle::addPressure(float &p){
	pressure = p;
}
const float Particle::getPressure() const{
	return pressure;
}
const glm::vec3 Particle::getVelocity() const
{
	return vel;
}
//void updateForce(){
//	glm::vec2 r_vec = p.getPos() - n.getPos()
//	float h = 25;
//	glm::vec2 W_grad = -1890 / (64 * M_PI * pow(h, 9.0))*pow((pow(h, 2.0) - pow(r, 2.0)), 2.0) * r_vec;
//	force = 
//}

void Particle::addPressureForce(glm::vec3 f){
	pressure_force = pressure_force + f;
	//gravity_force.y = density * gravity_force.y;
}

void Particle::addViscosityForce(glm::vec3 f){
	viscosity_force = viscosity_force + f;
}
//Evolves the particle parameters over time.
//This method changes the vertical and horizontal poition of the particle

void Particle::EvolveParticle()
{
	/*viscosity_force *= VISCOSITY * PARTICLE_MASS;
	pressure_force *= -PARTICLE_MASS;*/
	glm::vec3 acc = (pressure_force + gravity_force*density + viscosity_force)/density;
	//cout << "FORCE x " << acc.x << " Force y " << pressure_force.y << endl;
	//vel[0] = vel[0] + 1000*force.x; //  0.0000000000000001f*force.x;
	//vel[1] = vel[1] ; // 0.0000000000000001f*force.y;
	//cout << " FORCE Y " << vel[1] << endl;
	// Move the particle
	//vel = vel + acc;

	glm::vec3 newPos = pos + vel * dt + acc * dt * dt / 2.f;
	/*pos[0] += vel[0];
	pos[1] += vel[1];*/

	glm::vec3 newVel = (newPos - pos) / dt;

	pos = newPos;
	vel = newVel;

	if (pos[0] < 1 || pos[0] > 511) {
		pos[0] =  pos[0] < 1.f ? 1.f : 511.f;
		vel[0] = -vel[0]*0.9f;
	}

	if (pos[1] < 1 || pos[1] > 511) {
		pos[1] = pos[1] < 1.f ? 1.f : 511.f;
		vel[1] = -vel[1]*0.5f;
	}

	
	//std::cout << "POS X AFTER " << pos[0] << std::endl
}

//Draw all the particles
void Particle::DrawObjects() {
	glColor3f(0, 0, 1);
    glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.0, 1.0); glVertex3f(pos[0] + p_size, pos[1] + p_size, pos[2]);     // top    right
	glTexCoord2f(0.0, 1.0); glVertex3f(pos[0] - p_size, pos[1] + p_size, pos[2]);     // top    left
	glTexCoord2f(0.0, 1.0); glVertex3f(pos[0] + p_size, pos[1] - p_size, pos[2]);     // bottom right
	glTexCoord2f(0.0, 1.0); glVertex3f(pos[0] - p_size, pos[1] - p_size, pos[2]);     // bottom 
    glEnd();
}

glm::vec3 Particle::getPos() {
	return pos;
}	

glm::vec2 Particle::getCell() {
	//std::cout << " GLM::VEC2 X  " << glm::floor(pos[0] / 512.f * (512 / 32)) << " Y  " << glm::floor(pos[1] / 512.f * (512 / 32)) << std::endl;
	return glm::vec2(glm::floor(pos[0] / 512.f * GRID_WIDTH ), glm::floor(pos[1] / 512.f * GRID_HEIGHT));
}

void Particle::resetValues(){
	density = 0.f;
	pressure = 0.f;
	pressure_force = glm::vec3(0.f, 0.f, 0.f);

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
