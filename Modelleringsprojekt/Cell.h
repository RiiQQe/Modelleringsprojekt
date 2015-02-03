#include <vector>
#include "Particle.h"

using namespace std;

class Cell {
	private: 
		int index;
		int currMax;
		Particle *particles[500];

	public:
		void CreateCell(int _index);
		void addParticle(Particle _particle);
        vector<Particle> getParticles();
		void clearParticles();

		void displayParticles();
		bool hasNeighbours();
};
