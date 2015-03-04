#include <glm/glm.hpp>

#ifndef _Particle_H_
#define _Particle_H_

class Particle
{
    private:
		glm::vec3 pos, vel, pressure_force, viscousity_force, gravity_force;
		float mass, gravity, pressure, density, radius = 0.9f;
		int cellIndex;

    public:
        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
        const glm::vec3 getPos() const;
    	const glm::vec3 getVelocity();
		int getCellIndex();

    	void setPos(glm::vec3 p);
    	void setDensity(float d);
    	void setPressure(float p);
    	void setPressureForce(glm::vec3 f);
    	void setViscousityForce(glm::vec3 f);
    	void setGravityForce(glm::vec3 f);
    	float getDensity();
    	float getPressure();
		float getRadius();
    
};

#endif
