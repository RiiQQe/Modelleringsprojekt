#include <vector>
#include "Particle.h"

using namespace std;

class Cell {
	private: 
		vector<Particle *> particles;
		vector<int> neighbours;
		vector<int> checker;
		int index;
		int x = 0;

		enum GRID {
			W = 4,
			H = 4,
			L = 4
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
