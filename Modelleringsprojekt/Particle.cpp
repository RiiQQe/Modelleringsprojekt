#include <iostream>
#include "Particle.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;
const float dt = 0.0004f;

// Constructor for a particle.
void Particle::CreateParticle()
{
	pos = vec3(0, 0, 0);
	vel = vec3(0, 0, 0);
    
	//mass = 1.0f;
	//gravity = 9.82f;
    //radius = 8;
    pressure = 0;
    density = 0;
    
    viscousity_force = glm::vec3(0);
    pressure_force = glm::vec3(0);
    gravity_force = glm::vec3(0);
}

//Evolves the particle parameters over time.
//This method changes the vertical and horizontal poition of the particle
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
    
    else if(pos.x > 511){
        
        vel.x = -0.8*vel.x;
        pos.x = 511;
    }
    
    if(pos.y < 1){
        
        vel.y = -0.8*vel.y;
        pos.y = 1;
        
    }
    
    
    else if(pos.y > 511){
        
        vel.y = -0.8*vel.y;
        pos.y = 511;
        
    }
    
}

//Draw all the particles
void Particle::DrawObjects() {
   
    /*
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+2, pos[1]+2,0.5);     // top    right
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-2, pos[1]+2,0.5);     // top    left
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+2, pos[1]-2,0.5);     // bottom right
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-2, pos[1]-2,0.5);     // bottom left*/
    
    glColor3f(0.2,0.2,1);
    
    glBegin(GL_TRIANGLE_FAN);
    for(int ii = 0; ii < 15; ii++)
    {
        float theta = 2.0f * 3.1415926f * float(ii) / float(15);//get the current angle
        
        float x = 16/3 * cosf(theta);//calculate the x component
        float y = 16/3 * sinf(theta);//calculate the y component
        
        glVertex2f(x + pos[0], y + pos[1]);//output vertex
    }
    
    glEnd();
}

const glm::vec3 Particle::getPos() const {
    
	return pos;
    
}

const glm::vec3 Particle::getVelocity(){
    
    return vel;
    
}

int Particle::getCellIndex() {
	glm::vec2 cell = glm::vec2(glm::floor(pos[0] / 512.f * 32 ), glm::floor(pos[1] / 512.f * 32));
    
    int _cellIndex = ((int)cell.x % 32) + ((int)cell.y)*32;
    
   // std::cout << "CELLINDEX : " << _cellIndex << " from pos (" << (int)cell.x % 32 << ", " << (int)cell.y << ")" << std::endl;
	
   // std::cout << _cellIndex;
    //std::cout << "_cellIndex" << _cellIndex << std::endl;
    //std::cout << "cell.x" << cell.x << std::endl;
    //std::cout << "cell.y" << cell.y << std::endl;
    
	return _cellIndex;
}

//float Particle::getRadius(){
//    return radius;
//}

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
