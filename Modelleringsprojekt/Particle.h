#include <glm/glm.hpp>


class Particle
{
    private:
    	glm::vec2 pos;
    	glm::vec2 vel;
    	float gravity, mass, density;
	
    public:
        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
    
    	glm::vec2 getPos();
    	void setPos(float xPos, float yPos);
    	void setPressure(float value);
};