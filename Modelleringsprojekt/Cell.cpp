#include <iostream>
#include "Cell.h"

using namespace std;

// Constructor for creation of cell
void Cell::CreateCell(int _index) {
	// Let each cell know it's own index in the big picture
	index = _index;

	// At first execution, find my neighbours and save them
	setNeighbours();
}

// Function to add particle to this list
void Cell::addParticle(Particle &_particle) {
	particles.push_back(&_particle);
}

// Return particles currently within this cell
vector<Particle*> Cell::getParticles() {
	return particles;
}

// Clear the particles list
void Cell::clearParticles() {
	particles.clear();
}

// Return the neighbours
const std::vector<int> &Cell::getNeighbours() const {
	return neighbours;
}

// Set the neighbours
void Cell::setNeighbours() {
	
	neighbours.push_back(index);

	switch (index % W){
		case 0:
			if (index == 0){
				neighbours.push_back(index + 1);
				neighbours.push_back(index + W);
				neighbours.push_back(index + W + 1);
				neighbours.push_back(index + 2*W);
				neighbours.push_back(index + 2*W + 1);
				neighbours.push_back(index + 2*W*H);
				neighbours.push_back(index + 2*W*H + 1);

			}






	default:
		break;
	}





	switch (index % W) {
		case 0: 
			if (index == 0) {
				neighbours.push_back(index + 1);
				neighbours.push_back(index + W);
				neighbours.push_back(index + W + 1);
			}
			else if (index == H * ( W - 1)) {
				neighbours.push_back(index + 1);
				neighbours.push_back(index - W);
				neighbours.push_back(index - W + 1);
			}
			else {
				neighbours.push_back(index + 1);
				neighbours.push_back(index + W);
				neighbours.push_back(index - W);
				neighbours.push_back(index + W + 1);
				neighbours.push_back(index - W + 1); 
			}
			break;
		case (W - 1):
			if (index == W - 1) {
				neighbours.push_back(index - 1);
				neighbours.push_back(index + W);
				neighbours.push_back(index + W - 1);
			}
			else if (index == W * H - 1) {
				neighbours.push_back(index - 1);
				neighbours.push_back(index - W);
				neighbours.push_back(index - W - 1);
			}
			else {
				neighbours.push_back(index - 1);
				neighbours.push_back(index + W);
				neighbours.push_back(index - W);
				neighbours.push_back(index + W - 1);
				neighbours.push_back(index - W - 1); 
			}
			break;
		default:
			if (index < W) {
				neighbours.push_back(index - 1);
				neighbours.push_back(index + 1);
				neighbours.push_back(index + W - 1);
				neighbours.push_back(index + W);
				neighbours.push_back(index + W + 1);
			}
			else if (index > H * (W - 1) - 1) {
				neighbours.push_back(index - 1);
				neighbours.push_back(index + 1);
				neighbours.push_back(index - W - 1);
				neighbours.push_back(index - W);
				neighbours.push_back(index - W + 1);
			}
			else {
				neighbours.push_back(index - W - 1);
				neighbours.push_back(index - W);
				neighbours.push_back(index - W + 1);
				neighbours.push_back(index - 1);
				neighbours.push_back(index + 1);
				neighbours.push_back(index + W - 1);
				neighbours.push_back(index + W);
				neighbours.push_back(index + W + 1);
			}
			break;
	}

	/*std::cout << "Setting the neighbours of cell: " << index << std::endl;
	std::cout << "Neighbours are: "; 

	for (std::vector<int>::iterator it = neighbours.begin(); it != neighbours.end(); ++it) {
		std::cout << *it << ", ";
	}
	std::cout << std::endl;*/
}

// Return this cell's index;
const int &Cell::getIndex() const{
	return index;
}
