#include <glm/glm.hpp>

class Particle
{
    private:
		glm::vec2 pos;
		glm::vec2 vel;
    	glm::vec2 current_gridcell;
		float mass, gravity, radius, pressure, density;
		bool special;
    	glm::vec2 pressure_force;
    	glm::vec2 viscousity_force;
    	glm::vec2 gravity_force;
	
    public:

        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
        glm::vec2 getPos();
        glm::vec2 getCell();
    
    	void setPos(glm::vec2 p);
    
    	float getDensity();
    	float getPressure();
    
    	void setDensity(float d);
    	void setPressure(float p);
    
		void setSpecial();
    	void setCurrentGridCell();
    
    	void setVelocity(glm::vec2 v);
    	glm::vec2 getVelocity();
    
    	void setPressureForce(glm::vec2 f);
    	void setViscousityForce(glm::vec2 f);
    	void setGravityForce(glm::vec2 f);
    
    	glm::vec2 getPressureForce();
        glm::vec2 getViscousityForce();
    	glm::vec2 getGravityForce();
    
};
