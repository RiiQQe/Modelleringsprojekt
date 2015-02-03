#include <glm/glm.hpp>

#ifndef _Particle_H_
#define _Particle_H_

class Particle
{
    private:
		glm::vec3 pos;
		glm::vec3 vel;
		float mass, gravity, radius;
		bool special;
		int cellIndex;
	
    public:
        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
        glm::vec3 getPos();
		glm::vec2 getCell();
		void setSpecial();
		void setCellIndex();
		int getCellIndex();	
};

#endif
