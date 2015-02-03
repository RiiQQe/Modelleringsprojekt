#include <iostream>
#include "Cell.h"

using namespace std;

// Constructor for creation of cell
void Cell::CreateCell(int _index) {

	index = _index;
	currMax = 0;

//	cout << "Cell creation: index: " << index << endl;
//	cout << "contains: " << currMax << " items. WHoah!" << endl;
}

// Function to add particle to this list
void Cell::addParticle(Particle _particle) {
	particles[currMax] = &_particle;
	currMax++;
}

// Return particles currently within this cell
vector<Particle> Cell::getParticles() {
	vector<Particle> p;
	for (int i = 0; i < currMax; ++i) {
		p.push_back(*particles[i]);
	}

	return p;
}

// Clear the particles list
void Cell::clearParticles() {
	if (currMax != 0) {
		for (int i = 0; i < currMax; ++i) {
//			delete particles[i];
			particles[i] = nullptr;
		}
	
		currMax = 0;
	}
}

void Cell::displayParticles() {
	/*
	for (int i = 0; i < currMax; ++i) {
		std::cout << particles[i] << ", ";
	}
	*/
}

bool Cell::hasNeighbours() {
	return currMax > 1;
}
