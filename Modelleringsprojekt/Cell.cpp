#include <iostream>
#include <list>
#include "Cell.h"

using namespace std;

int numCols = 16;

void Cell::CreateCell(int _index){
	index = _index;

	cellParticles;
}

void Cell::AddParticleToCell(Particle _particle){

	cellParticles.push_back(_particle);

}

void Cell::write(){

	std::list<int>::const_iterator iterator;
	for (iterator = neighbours.begin(); iterator != neighbours.end(); ++iterator) {
		std::cout << *iterator << " ";
	}
}


void Cell::setNeighbours(int index){
	
	neighbours.push_back(index);

	switch (index % numCols) {
	case 0:
		if (index == 0) {
			neighbours.push_back(index + 1);
			neighbours.push_back(index + numCols);
			neighbours.push_back(index + numCols + 1);
		}
		else if (index == 256 - numCols) {
			neighbours.push_back(index + 1);
			neighbours.push_back(index - numCols);
			neighbours.push_back(index - numCols + 1);
		}
		else {
			neighbours.push_back(index + 1);
			neighbours.push_back(index + numCols);
			neighbours.push_back(index - numCols);
			neighbours.push_back(index + numCols + 1);
			neighbours.push_back(index - numCols + 1);
		}
		break;
	case 15:
		if (index == 15) {
			neighbours.push_back(index - 1);
			neighbours.push_back(index + numCols);
			neighbours.push_back(index + numCols - 1);
		}
		else if (index == 255) {
			neighbours.push_back(index - 1);
			neighbours.push_back(index - numCols);
			neighbours.push_back(index - numCols - 1);
		}
		else {
			neighbours.push_back(index - 1);
			neighbours.push_back(index + numCols);
			neighbours.push_back(index - numCols);
			neighbours.push_back(index + numCols - 1);
			neighbours.push_back(index - numCols - 1);
		}
		break;
	default:
		if (index < numCols) {
			neighbours.push_back(index - 1);
			neighbours.push_back(index + 1);
			neighbours.push_back(index + numCols - 1);
			neighbours.push_back(index + numCols);
			neighbours.push_back(index + numCols + 1);
		}
		else if (index > 239) {
			neighbours.push_back(index - 1);
			neighbours.push_back(index + 1);
			neighbours.push_back(index - numCols - 1);
			neighbours.push_back(index - numCols);
			neighbours.push_back(index - numCols + 1);
		}
		else {
			neighbours.push_back(index - numCols - 1);
			neighbours.push_back(index - numCols);
			neighbours.push_back(index - numCols + 1);
			neighbours.push_back(index - 1);
			neighbours.push_back(index + 1);
			neighbours.push_back(index + numCols - 1);
			neighbours.push_back(index + numCols);
			neighbours.push_back(index + numCols + 1);
		}
		break;
	}

}
