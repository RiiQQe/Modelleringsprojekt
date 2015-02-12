#include <vector>
#include "Particle.h"

using namespace std;

class Cell {
	private: 
		vector<Particle *> particles;
		vector<int> neighbours;
		int index;

		

	public:
		void CreateCell(int _index);
		void addParticle(Particle &_particle);
        vector<Particle*> getParticles();
		void clearParticles();
		const vector<int> &getNeighbours() const;
    
		void setNeighbours();
		const int &getIndex() const;

		void displayParticles();

		
		static const int GRID_WIDTH = 100;
		static const int GRID_HEIGHT = 100;
		 
};
