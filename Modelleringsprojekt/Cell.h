#include <list>
#include "Particle.h"

using namespace std;

class Cell{
	public:
		void CreateCell(int _index);
		void AddParticleToCell(Particle _particle);
		void setNeighbours(int _index);
		void write();

	private:
		int index;
		int currMax;

		list<int> neighbours;
		list<Particle> cellParticles;
	

};