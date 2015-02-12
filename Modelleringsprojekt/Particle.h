#include <glm/glm.hpp>

#ifndef _Particle_H_
#define _Particle_H_

class Particle
{
    private:
		glm::vec3 pos;
		glm::vec3 vel;
		glm::vec3 pressure_force, gravity_force, viscosity_force;
		float mass,gravity, radius, pressure, density;
		int cellIndex;
	
    public:
        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
        glm::vec3 getPos();
		glm::vec2 getCell();

		void addDensity(float &w);
		const float getDensity() const;

		void addPressure(float &p);
		const float getPressure() const;

		void addPressureForce(glm::vec3 f);
		void addViscosityForce(glm::vec3 f);

		void resetValues();

		const glm::vec3 getVelocity() const;

		void setSpecial();
		void setCellIndex();
		int getCellIndex();	

};

#endif
