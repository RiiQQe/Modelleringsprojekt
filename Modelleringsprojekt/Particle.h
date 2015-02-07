#include <glm/glm.hpp>

#ifndef _Particle_H_
#define _Particle_H_

class Particle{
	public:
		void CreateParticle();
		void EvolveParticle();
		void DrawObjects();
		void setCellIndex();
		int getCellIndex();


	private:
		glm::vec3 pos;
		glm::vec3 vel;
		float mass, gravity, radius;
		int cellIndex;


};


#endif