#include <iostream>
#include "Cell.h"

using namespace std;

// Function to add particle to this list
void Cell::addCellParticle(Particle &_particle){
	cellParticles.push_back(_particle);
}

const list <int> Cell::getNeighbourIndexes() const{
	return neighbours;
}


const list <Particle> Cell::getCellParticles() const{
	return cellParticles;
}

void Cell::setNeighbours(int i){
	int x = i % GRID_WIDTH;		//Get right x 
	int y = (int)(i / GRID_HEIGHT); //Get right y
	cout << "Y " << y << " X " << x << endl;
	switch (x){
		//Vänster sida
		case 0:
			//Upp
			if (y == 0){
				neighbours.push_back(1);  //right to Cell
				neighbours.push_back(GRID_WIDTH);
				neighbours.push_back((GRID_WIDTH + 1));
			}//Nere
			else if (y == (GRID_HEIGHT - 1)){
				neighbours.push_back(i + 1);  //right to Cell
				neighbours.push_back(i - (GRID_WIDTH - 1));
				neighbours.push_back(i - GRID_WIDTH);
			}
			else {
				neighbours.push_back(i - GRID_WIDTH); //top
				neighbours.push_back(i - (GRID_WIDTH - 1)); //top right
				neighbours.push_back(i + 1);  //right 
				neighbours.push_back(i + GRID_WIDTH); //bottom
				neighbours.push_back(i + (GRID_WIDTH + 1)); //bottom right
			}
			break;

		//Höger sida
		case (GRID_WIDTH-1):
			//Upp
			if (y == 0){
				neighbours.push_back(i - 1);  //Left to Cell
				neighbours.push_back(i + (GRID_WIDTH - 1)); 
				neighbours.push_back(i + GRID_WIDTH);
			}//Ner
			else if (y == (GRID_HEIGHT - 1)){
				neighbours.push_back(i - 1);  //Left to Cell
				neighbours.push_back(i - (GRID_WIDTH - 1));
				neighbours.push_back(i - GRID_WIDTH);
			}
			else {
				neighbours.push_back(i - GRID_WIDTH); //top
				neighbours.push_back(i - (GRID_WIDTH + 1)); //top left
				neighbours.push_back(i - 1);  //left
				neighbours.push_back(i + (GRID_WIDTH - 1)); //bottom left
				neighbours.push_back(i + GRID_WIDTH); //bottom
			}
			break;
		default:
			if (y == 0) {
				neighbours.push_back(i - 1);
				neighbours.push_back(i + 1);
				neighbours.push_back(i + GRID_WIDTH - 1);
				neighbours.push_back(i + GRID_WIDTH);
				neighbours.push_back(i + GRID_WIDTH + 1);
			}
			else if (y == (GRID_HEIGHT - 1)) {
				neighbours.push_back(i - 1);
				neighbours.push_back(i + 1);
				neighbours.push_back(i - GRID_WIDTH - 1);
				neighbours.push_back(i - GRID_WIDTH);
				neighbours.push_back(i - GRID_WIDTH + 1);
			}
			else {
				neighbours.push_back(i - 1);  //Left to Cell
				neighbours.push_back(i - (GRID_WIDTH + 1)); //Left top 
				neighbours.push_back(i - GRID_WIDTH); //top
				neighbours.push_back(i - (GRID_WIDTH - 1)); //top right
				neighbours.push_back(i + 1); //right
				neighbours.push_back(i + (GRID_WIDTH - 1)); //left bottom
				neighbours.push_back(i + GRID_WIDTH); //bottom
				neighbours.push_back(i + (GRID_WIDTH + 1)); //bottom right

			}
			break;

	}
}

void Cell::clearCellParticles(){
	cellParticles.clear();
}

