#include <iostream>
#include "Cell.h"

using namespace std;

// Constructor for creation of cell
void Cell::CreateCell(int _index) {
	index = _index;
	
	setNeighbours();
//	cout << "Cell creation: index: " << index << endl;
//	cout << "contains: " << currMax << " items. WHoah!" << endl;
}

// Function to add particle to this list
void Cell::addParticle(Particle &_particle) {
	particles.push_back(_particle);
}

// Return particles currently within this cell
const vector<Particle> &Cell::getParticles() const {
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
	// set neighbours

	neighbours.push_back(index);
	
	switch (index % 16) {
		case 0: 
			if (index == 0) {
				neighbours.push_back(index + 1);
				neighbours.push_back(index + 16);
				neighbours.push_back(index + 16 + 1);
			}
			else if (index == 256 - 16) {
				neighbours.push_back(index + 1);
				neighbours.push_back(index - 16);
				neighbours.push_back(index - 16 + 1);
			}
			else {
				neighbours.push_back(index + 1);
				neighbours.push_back(index + 16);
				neighbours.push_back(index - 16);
				neighbours.push_back(index + 16 + 1);
				neighbours.push_back(index - 16 + 1); 
			}
			break;
		case 15: 
			if (index == 15) {
				neighbours.push_back(index - 1);
				neighbours.push_back(index + 16);
				neighbours.push_back(index + 16 - 1);
			}
			else if (index == 255) {
				neighbours.push_back(index - 1);
				neighbours.push_back(index - 16);
				neighbours.push_back(index - 16 - 1);
			}
			else {
				neighbours.push_back(index - 1);
				neighbours.push_back(index + 16);
				neighbours.push_back(index - 16);
				neighbours.push_back(index + 16 - 1);
				neighbours.push_back(index - 16 - 1); 
			}
			break;
		default:
			if (index < 16) {
				neighbours.push_back(index - 1);
				neighbours.push_back(index + 1);
				neighbours.push_back(index + 16 - 1);
				neighbours.push_back(index + 16);
				neighbours.push_back(index + 16 + 1);
			}
			else if (index > 239) {
				neighbours.push_back(index - 1);
				neighbours.push_back(index + 1);
				neighbours.push_back(index - 16 - 1);
				neighbours.push_back(index - 16);
				neighbours.push_back(index - 16 + 1);
			}
			else {
				neighbours.push_back(index - 16 - 1);
				neighbours.push_back(index - 16);
				neighbours.push_back(index - 16 + 1);
				neighbours.push_back(index - 1);
				neighbours.push_back(index + 1);
				neighbours.push_back(index + 16 - 1);
				neighbours.push_back(index + 16);
				neighbours.push_back(index + 16 + 1);
			}
			break;
	}

	std::cout << "Setting the neighbours of cell: " << index << std::endl;
	std::cout << "Neighbours are: "; 

	for (std::vector<int>::iterator it = neighbours.begin(); it != neighbours.end(); ++it) {
		std::cout << *it << ", ";
	}
	std::cout << std::endl;
}

// Return this cell's index;
const int &Cell::getIndex() const{
	return index;
}
