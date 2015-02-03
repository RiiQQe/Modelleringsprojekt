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
	
    public:
        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
        glm::vec3 getPos();
    	void SetXPos(float xPos);
	    void SetYPos(float yPos);
		glm::vec2 getCell();
		
		void setSpecial();
};

#endif
