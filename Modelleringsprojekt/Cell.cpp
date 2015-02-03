#include <iostream>
#include "Cell.h"

using namespace std;

// Constructor for creation of cell
void Cell::CreateCell(int _index) {

	index = _index;

	cout << "Cell creation: index: " << index << endl;
	cout << "contains: " << particles.size() << " items. WHoah!" << endl;
}

void Cell::addParticle(Particle _particle) {
	particles.push_back(_particle);
}

vector<Particle> Cell::getParticles() {
	return particles;
}

void Cell::clearParticles() {
	//std::cout << "clearing particle of index: " << index << std::endl;
	if (!particles.empty()) {
		//std::cout << "particles variable is not empty! it is filled with : " << particles.size() << std::endl;
		particles.clear();
	}
}

void Cell::displayParticles() {
	
}

bool Cell::hasNeighbours() {
	return particles.size() > 1;
}
