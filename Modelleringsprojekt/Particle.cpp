#include <iostream>
#include "Particle.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;
const float dt = 1.f;
bool first = true;

// Constructor for a particle.
void Particle::CreateParticle()
{
	pos = vec3(0, 0, 0);
	vel = vec3(0, 0, 0);
	
	mass = 1.0f;
	gravity = 9.82f;
	radius = 3.0f;
	special = false;
    pressure = 0;
    density = 0;
    
    viscousity_force = glm::vec3(0);
    pressure_force = glm::vec3(0);
    gravity_force = glm::vec3(0);
}

//Evolves the particle parameters over time.
//This method changes the vertical and horizontal position of the particle
void Particle::EvolveParticle()
{
    glm::vec3 newPos = pos + dt*vel + dt*dt*((gravity_force + pressure_force + viscousity_force)/density)/2.f;
	glm::vec3 newVel = (newPos - pos) / dt;

    pos = newPos;
    vel = newVel;
    
    if(pos.x < 1){
        vel.x = -vel.x;
        pos.x = 1;
    }
    
    else if(pos.x > 512){
        vel.x = -vel.x;
        pos.x = 512;
    }
    
    if(pos.y < 1){
        vel.y = -vel.y;
        pos.y = 1;
    }
    
    else if(pos.y > 512){
        vel.y = -vel.y;
        pos.y = 512;
    }

	//std::cout << "Pos.x = " << pos.x << " Pos.y = " << pos.y << " Pos.z = " << pos.z << std::endl;
}

//Draw all the particles
void Particle::DrawObjects() {
    //!special ? glColor3f(0.2,0.2,1) :  glColor3f(1,1,1);
    //glBegin(GL_TRIANGLE_STRIP);
	glPushMatrix();
	glBegin(GL_QUADS);
	// Top face (y = 1.0f)
	// Define vertices in counter-clockwise (CCW) order with normal pointing out
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f(pos[0] + 2.0f, pos[1] + 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] + 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] + 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] + 2.0f, pos[1] + 2.0f, pos[2] + 2.0f);

	// Bottom face (y = -1.0f)
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f(pos[0] + 2.0f, pos[1] - 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] - 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] - 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] + 2.0f, pos[1] - 2.0f, pos[2] - 2.0f);

	// Front face  (z = 1.0f)
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f(pos[0] + 2.0f, pos[1] + 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] + 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] - 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] + 2.0f, pos[1] - 2.0f, pos[2] + 2.0f);

	// Back face (z = -1.0f)
	glColor3f(0.0f, 1.0f, 1.0f);     // Blue
	glVertex3f(pos[0] + 2.0f, pos[1] - 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] - 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] + 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] + 2.0f, pos[1] + 2.0f, pos[2] - 2.0f);

	// Left face (x = -1.0f)
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
	glVertex3f(pos[0] - 2.0f, pos[1] + 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] + 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] - 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] - 2.0f, pos[1] - 2.0f, pos[2] + 2.0f);

	// Right face (x = 1.0f)
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
	glVertex3f(pos[0] + 2.0f, pos[1] + 2.0f, pos[2] - 2.0f);
	glVertex3f(pos[0] + 2.0f, pos[1] + 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] + 2.0f, pos[1] - 2.0f, pos[2] + 2.0f);
	glVertex3f(pos[0] + 2.0f, pos[1] - 2.0f, pos[2] - 2.0f);

    glEnd();
	glPopMatrix();
}

const glm::vec3 Particle::getPos() const {
	return pos;
}	

int Particle::getCellIndex() {
	glm::vec3 cell = glm::vec3(glm::floor(pos[0] / 512.f * (512 / 32)), glm::floor(pos[1] / 512.f * (512 / 32)), glm::floor(pos[2] / 512.f * (512 / 32)));
	int _cellIndex = (int)cell.x % 16 + (int)cell.y * 16;
	
	return _cellIndex;
}

void Particle::addToVel(glm::vec3 v) {
    vel.x += v.x;
    vel.y += v.y;
	vel.z += v.z;
}

glm::vec3 Particle::getVel() {
    return glm::vec3(vel.x, vel.y, vel.z);
}


void Particle::setPos(glm::vec3 p){
    pos = p;
}

void Particle::setPressure(float p){
    pressure = p;
}

void Particle::setDensity(float d){
    density = d;
}

float Particle::getDensity(){
    return density;
}

float Particle::getPressure(){
    return pressure;
}

glm::vec3 Particle::getGravityForce(){
    
    return gravity_force;
    
}

glm::vec3 Particle::getViscousityForce(){
    
    return viscousity_force;
    
}

glm::vec3 Particle::getPressureForce(){
    
    return pressure_force;
    
}

void Particle::setPressureForce(glm::vec3 f){
    
    pressure_force = f;
}

void Particle::setViscousityForce(glm::vec3 f){
    
    viscousity_force = f;
    
}

void Particle::setGravityForce(glm::vec3 f){
    
    gravity_force = f;
    
}

glm::vec3 Particle::getVelocity(){
    
    return vel;
    
}

