
class Particle
{
    private:
		float xpos,ypos,zpos;                 // position of the particle
		float xspeed,yspeed,zspeed;           // speed of the particle
	
    public:
        void CreateParticle();
        void EvolveParticle();
        void DrawObjects();
    	float GetXPos();
        float GetYPos();
    	void SetXPos(float xPos);
	    void SetYPos(float yPos);

};