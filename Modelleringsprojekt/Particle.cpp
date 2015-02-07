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
	pos = vec2(0, 0 ) ;
	vel = vec2(0);

	mass = 1.0f;
	gravity = 9.82f;
	radius = 3.0f;
	special = false;
    pressure = 0;
    density = 0;
    
    viscousity_force = glm::vec2(0);
    pressure_force = glm::vec2(0);
    gravity_force = glm::vec2(0);
    
    current_gridcell = vec2(0,0);
}
	

//Evolves the particle parameters over time.
//This method changes the vertical and horizontal poition of the particle

void Particle::EvolveParticle()
{
    
    /*
	vel[1] -= gravity * 0.01f;
	pos[1] += vel[1];

	if (pos[0] <  0 || pos[0] > 512) {
		vel[0] = -vel[0];
	}
	if (pos[1] < 0 || pos[1] > 512) {
		vel[1] = -vel[1] * 0.7;
		pos[1] = pos[1] < 0 ? 0 : pos[1];
	}

	pos[0] += vel[0];*/
    
    if(pos.x <= 1){
    	
        pos.x = 1;
        vel.x = -0.7*vel.x;
    }
    
    else if(pos.x >= 512){
        pos.x = 512;
        vel.x = -0.7*vel.x;
    	
    }
    
    if(pos.y <= 1){
    	
        pos.y = 1;
        vel.y = -0.7*vel.y;
    	
    }
    
    else if(pos.y >= 512){
    	
        pos.y = 512;
        vel.y = -0.7*vel.y;
    
    }
    
}

void Particle::setPressureForce(glm::vec2 f){

    pressure_force = f;
}

void Particle::setViscousityForce(glm::vec2 f){
	
    viscousity_force = f;

}

void Particle::setGravityForce(glm::vec2 f){

    gravity_force = f;
	
}


glm::vec2 Particle::getGravityForce(){
	
    return gravity_force;
	
}

glm::vec2 Particle::getViscousityForce(){
    
    return viscousity_force;
    
}

glm::vec2 Particle::getPressureForce(){
    
    return pressure_force;
    
}


void Particle::setCurrentGridCell(){

    current_gridcell = glm::vec2(glm::floor(pos[0] / 512.f * (512 / 32)), glm::floor(pos[1] / 512.f * (512 / 32)));
    
}

//Draw all the particles
void Particle::DrawObjects()
{

    !special ? glColor3f(0.2,0.2,1) :  glColor3f(1,1,1);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+1, pos[1]+1,0.5);     // top    right
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-1, pos[1]+1,0.5);     // top    left
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]+1, pos[1]-1,0.5);     // bottom right
    glTexCoord2f(0.0,1.0); glVertex3f(pos[0]-1, pos[1]-1,0.5);     // bottom left
    
    glEnd();
	
}

glm::vec2 Particle::getPos() {
	return pos;
}	

void Particle::setVelocity(glm::vec2 v){

    vel = v;

}

void Particle::setPos(glm::vec2 p){

    pos = p;
    
}


glm::vec2 Particle::getVelocity(){

    return vel;

}


glm::vec2 Particle::getCell() {
	return glm::vec2(glm::floor(pos[0] / 512.f * (512 / 32)), glm::floor(pos[1] / 512.f * (512 / 32)));
}

void Particle::setSpecial() {
	special = true;
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



