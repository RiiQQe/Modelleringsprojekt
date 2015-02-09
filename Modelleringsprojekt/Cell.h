#include <vector>
#include <list>
#include "Particle.h"

using namespace std;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 10;

class Cell {

	public:
		Cell(){
			//cout << "cell created " << currMax << endl;
		};
		~Cell(){
			//cout << "Destructor" << endl;
		};
		void CreateCell(int _index);
		void addCellParticle(Particle &_particle);

		const vector <Particle> &getCellParticles() const;
		vector <Particle> getNeighbours() const;
		void clearCellParticles();

		void setNeighbours(int index);
		const vector <int> &getNeighbourIndexes() const;
		

		void displayParticles();
		bool hasNeighbours();
private:
	
		vector <int> neighbours;
		vector <Particle> cellParticles;



};
