#include <glm/glm.hpp>

#ifndef _Particle_H_
#define _Particle_H_

class Particle
{
    private:
		glm::vec3 pos;
		glm::vec3 vel;
		float mass, gravity, radius, pressure, density;
		bool special;
		int cellIndex;
    	glm::vec3 pressure_force;
    	glm::vec3 viscousity_force;
    	glm::vec3 gravity_force;
	
    public:
        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
        const glm::vec3 getPos() const;
		int getCellIndex();
        void addToVel(glm::vec3 v);
        glm::vec3 getVel();
    
    	void setPos(glm::vec3 p);
    
    	void setDensity(float d);
    	void setPressure(float p);
    	void setPressureForce(glm::vec3 f);
    	void setViscousityForce(glm::vec3 f);
    	void setGravityForce(glm::vec3 f);
    
    	glm::vec3 getPressureForce();
    	glm::vec3 getViscousityForce();
    	glm::vec3 getGravityForce();
    	float getDensity();
    	float getPressure();
    	glm::vec3 getVelocity();
};

#endif
