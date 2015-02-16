#include <vector>
#include "Particle.h"

using namespace std;

class Cell {
	private: 
		vector<Particle *> particles;
		vector<int> neighbours;
		int index;

		enum GRID {
			W = 16,
			H = 16,
			L = 16
		};

	public:
		void CreateCell(int _index);
		void addParticle(Particle &_particle);
        vector<Particle*> getParticles();
		void clearParticles();
		const vector<int> &getNeighbours() const;
    
		void setNeighbours();
		const int &getIndex() const;

		void displayParticles();
};
