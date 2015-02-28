#include <iostream>
#include "Particle.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Sphere.cpp"

using namespace glm;
//const float dt = 1.f;
bool first = true;
const float dt = 0.0004f;

// Constructor for a particle.
void Particle::CreateParticle()
{
	pos = vec3(0, 0, 0);
	vel = vec3(0, 0, 0);

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
        vel.x = -0.8*vel.x;
        pos.x = 1;
    }
    
    else if(pos.x > 256){

        vel.x = -0.8*vel.x;
        pos.x = 256;
    }
    
    if(pos.y < 1){
        
        vel.y = -0.8*vel.y;
        pos.y = 1;
    }
    
    else if(pos.y > 512){
        
        vel.y = -0.8*vel.y;
        pos.y = 512;
    }

	if (pos.z < 1){
		vel.z = -0.8*vel.z;
		pos.z = 1;
	}
	else if(pos.z > 256){
		vel.z = -0.8*vel.z;
		pos.z = 256;
	}
}

//Draw all the particles
void Particle::DrawObjects() {
    
	
     

    //!special ? glColor3f(0.2,0.2,1) :  glColor3f(1,1,1);
   /* glBegin(GL_TRIANGLE_STRIP);
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
	glPopMatrix();*/
    
    glColor3f(0.2,0.2,1);
    
    Sphere sphere(16/3, 6, 12);
    //glutSolidSphere(16/3, 20.0, 20.0);
    sphere.draw(pos[0], pos[1], pos[2]);
    
    /*glBegin(GL_TRIANGLE_FAN);
    for(int ii = 0; ii < 15; ii++)
    {
        float theta = 2.0f * 3.1415926f * float(ii) / float(15);//get the current angle
        
        float x = 16/3 * cosf(theta);//calculate the x component
        float y = 16/3 * sinf(theta);//calculate the y component
        float z = 16/3 * sinf(theta);//calculate the y component
        
        glVertex3f(x + pos[0], y + pos[1], z + pos[2]);//output vertex
    }
    
    glEnd();*/
}

const glm::vec3 Particle::getPos() const {
    
	return pos;
    
}

const glm::vec3 Particle::getVelocity(){
    
    return vel;
    
}

int Particle::getCellIndex() {
	glm::vec3 cell = glm::vec3(glm::floor(pos[0] / 512.f * (512 / 32)), glm::floor(pos[1] / 512.f * (512 / 32)), glm::floor(pos[2] / 512.f * (512 / 32)));
    int _cellIndex = (int)cell.x % 32 + (int)cell.y % 32 + (int)cell.z * 32;
	
    //std::cout << _cellIndex << std::endl;
    
	return _cellIndex;
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


void Particle::setPressureForce(glm::vec3 f){
    pressure_force = f;
}

void Particle::setViscousityForce(glm::vec3 f){
    
    viscousity_force = f;
    
}

void Particle::setGravityForce(glm::vec3 f){
    
    gravity_force = f;
    
}
