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
		static int count;

        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
        const glm::vec3 &getPos() const;
		int getCellIndex();
        void addToVel(glm::vec2 v);
        glm::vec2 getVel();
};

#endif
