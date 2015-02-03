#include <glm/glm.hpp>

class Particle
{
    private:
		glm::vec2 pos;
		glm::vec3 vel;
    	glm::vec2 current_gridcell;
		float mass, gravity, radius;
		bool special;
	
    public:
        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
        glm::vec2 getPos();
    	void SetXPos(float xPos);
	    void SetYPos(float yPos);
		glm::vec2 getCell();
		
		void setSpecial();
    	void setCurrentGridCell();
};
