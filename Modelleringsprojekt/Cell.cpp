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

	int x = index % W;
	int y = (int)((index / W)) % H;
	int z = floor(index / (H * W));


	if (index == 13)
		cout << "x " << x << "y " << y << "z " << z << " W  " << W << endl;

	switch (index % W) {
	case 0: //Left side
		if (index == 0) { //Top left front (3*3*3) (0)
			neighbours.push_back(1);					// 1
			neighbours.push_back(W);					// 3
			neighbours.push_back(W + 1);				// 4
			neighbours.push_back(W*H);					// 9
			neighbours.push_back(W*H + 1);				// 10
			neighbours.push_back(W*H + W);				// 12
			neighbours.push_back(W*H + W + 1);			// 13
		}
		else if (index == H * (W - 1)) { // Down left front (6)
			neighbours.push_back(index + 1);			// 7
			neighbours.push_back(index - W);			// 3
			neighbours.push_back(index - W + 1);		// 4
			neighbours.push_back(index + W*H);			// 15
			neighbours.push_back(index + W*H + 1);		// 16
			neighbours.push_back(index + W*H - W);		// 12
			neighbours.push_back(index + W*H - W + 1);  // 13
		}
		else if(index == L * H * (W - 1)) { // Top left back (18)
			neighbours.push_back(index + 1);			// 19
			neighbours.push_back(index + W);			// 21
			neighbours.push_back(index + W + 1);		// 22 
			neighbours.push_back(index - W*H);			// 9
			neighbours.push_back(index - W*H + 1);		// 10
			neighbours.push_back(index - W*H + W);		// 12
			neighbours.push_back(index - W*H + W + 1);	// 13
		}
		else if(index == H * L * W - W){// Down left back (24)
			neighbours.push_back(index + 1);			// 25
			neighbours.push_back(index - W);			// 21
			neighbours.push_back(index - W + 1);		// 22
			neighbours.push_back(index - W*H);			// 15
			neighbours.push_back(index - W*H + 1);		// 16
			neighbours.push_back(index - W*H - W);		// 12
			neighbours.push_back(index - W*H - W + 1);	// 13
		}
		else{				//4*4*4			
			
			if (y == H-1 &&  z < H-1 && z > 0){	// (28)
				
				neighbours.push_back(index + 1);		// 29
				neighbours.push_back(index - W);		// 24
				neighbours.push_back(index - W + 1);	// 25
				neighbours.push_back(index + W*H);		// 44
				neighbours.push_back(index + W*H + 1);	// 45
				neighbours.push_back(index + W*H - W);  // 40
				neighbours.push_back(index + W*H - W +1);//41
				neighbours.push_back(index - W*H);		//12
				neighbours.push_back(index - W*H + 1);	//13
				neighbours.push_back(index-W*H - W);	//8
				neighbours.push_back(index - W*H - W + 1); //9
			}

			else if (y == 0 && z < H-1 && z > 0){//(16)
				neighbours.push_back(index + 1);		//17
				neighbours.push_back(index + W);		//20
				neighbours.push_back(index + W + 1);	//21
				neighbours.push_back(index + W*H);		//32
				neighbours.push_back(index + W*H + 1);	//33
				neighbours.push_back(index + W*H + W);	//36
				neighbours.push_back(index + W*H + W + 1); // 37
				neighbours.push_back(index - W*H);		//0
				neighbours.push_back(index - W*H +1);	//1
				neighbours.push_back(index - W*H + W);	//4
				neighbours.push_back(index - W*H + W + 1); //5
			}
			else if (z == 0 && y > 0 && y < H-1){// (4)
				neighbours.push_back(index + 1);		//5
				neighbours.push_back(index + W);		//8
				neighbours.push_back(index + W + 1);	//9
				neighbours.push_back(index - W);		//0
				neighbours.push_back(index - W + 1);	//1
				neighbours.push_back(index + W*H);		//20
				neighbours.push_back(index + W*H + 1);	//21
				neighbours.push_back(index + W*H + W);	//24
				neighbours.push_back(index + W*H + W + 1);//25
				neighbours.push_back(index + W*H - W);	//16
				neighbours.push_back(index + W*H - W + 1);//17
			}

			else if (z == L-1 && x == 0 && y < H-1 && y > 0){ // (52)
				neighbours.push_back(index + 1);			//53
				neighbours.push_back(index + W);			//56
				neighbours.push_back(index + W + 1);		//57
				neighbours.push_back(index - W);			//48
				neighbours.push_back(index - W + 1);		//49
				neighbours.push_back(index - W*H);			//36
				neighbours.push_back(index - W*H + 1);		//37
				neighbours.push_back(index - W*H - W);		//32
				neighbours.push_back(index - W*H - W + 1);	//33
				neighbours.push_back(index - W*H + W);		//40
				neighbours.push_back(index - W*H + W + 1);	//41
			}

			else if (y != 0 && y != H-1 && z != 0 && z != L-1){ // (20) 
				neighbours.push_back(index + 1);				// 21 
				neighbours.push_back(index + W);				// 24
				neighbours.push_back(index + W + 1);			// 25
				neighbours.push_back(index - W);				// 16
				neighbours.push_back(index - W + 1);			// 17
				neighbours.push_back(index + W * H);			// 36
				neighbours.push_back(index + W * H + 1);		// 37
				neighbours.push_back(index + W * H + W);		// 40
				neighbours.push_back(index + W * H + W + 1);	//41
				neighbours.push_back(index + W * H - W);		//32
				neighbours.push_back(index + W * H - W + 1);    //33
				neighbours.push_back(index - W * H);			//4
				neighbours.push_back(index - W * H + 1);		//5
				neighbours.push_back(index - W * H + W );		// 8
				neighbours.push_back(index - W * H + W + 1);	//9
				neighbours.push_back(index - W * H - W);		//0
				neighbours.push_back(index - W * H - W + 1);	//1
			}
		}
			break;
	case (W - 1) : //Right side
		if (index == W - 1) { //Top right front			(2) 
			neighbours.push_back(index - 1);			//1
			neighbours.push_back(index + W);			//5
			neighbours.push_back(index + W - 1);		//4
			neighbours.push_back(index + W*H);			//11
			neighbours.push_back(index + W*H - 1);		//10
			neighbours.push_back(index + W*H + W);		//14
			neighbours.push_back(index + W*H + W - 1);	//13
		}
		else if (index == W * H - 1) { //Down right front (8)
			neighbours.push_back(index - 1);			//7
			neighbours.push_back(index - W);			//5
			neighbours.push_back(index - W - 1);		//4
			neighbours.push_back(index + W*H);			//17
			neighbours.push_back(index + W*H - 1);		//16			
			neighbours.push_back(index + W*H - W);		//14
			neighbours.push_back(index + W*H - W - 1);	//13
		}
		else if (index == W * H * L - W*H + W - 1){ //Top right back (20)
			neighbours.push_back(index - 1);			//19
			neighbours.push_back(index + W);			//23
			neighbours.push_back(index + W - 1);		//22
			neighbours.push_back(index - W*H);			//11
			neighbours.push_back(index - W*H - 1);		//10
			neighbours.push_back(index - W*H + W);		//14
			neighbours.push_back(index - W*H + W - 1);	//13
		}
		else if(index == W * H * L - 1){ //Down right back (26)
			neighbours.push_back(index - 1);			//25
			neighbours.push_back(index - W);			//23
			neighbours.push_back(index - W - 1);		//22
			neighbours.push_back(index - W*H);			//17
			neighbours.push_back(index - W*H - 1);		//16
			neighbours.push_back(index - W*H - W);		//14
			neighbours.push_back(index - W*H - W - 1); // 13
		}
		else{ // (4*4*4)

			if (y == H-1 && x == W-1 && z < L-1 && z > 0){		// (31)

				neighbours.push_back(index - 1);			//30
				neighbours.push_back(index - W);			//27
				neighbours.push_back(index - W - 1);		//26
				neighbours.push_back(index + W*H);			//47
				neighbours.push_back(index + W*H - 1);		//46
				neighbours.push_back(index + W*H - W);		//43
				neighbours.push_back(index + W*H - W - 1);	//42
				neighbours.push_back(index - W*H);			//15
				neighbours.push_back(index - W*H - 1);		//14
				neighbours.push_back(index - W*H - W);		//11
				neighbours.push_back(index - W*H - W - 1);	//10	
			}
			else if (y == 0 && z < L-1 && z > 0){				// (19)
				neighbours.push_back(index - 1);			//18
				neighbours.push_back(index + W);			//23
				neighbours.push_back(index + W - 1);		//22
				neighbours.push_back(index - W*H);			//3
				neighbours.push_back(index - W*H - 1);		//2
				neighbours.push_back(index - W*H + W);		//7
				neighbours.push_back(index - W*H + W - 1);	//6
				neighbours.push_back(index + W*H);			//35
				neighbours.push_back(index + W*H - 1);		//34
				neighbours.push_back(index + W*H + W);		//39
				neighbours.push_back(index + W*H + W - 1);	//38
			}
			else if (z == L-1 && y < H-1 && y > 0){ // (55)
				neighbours.push_back(index - 1);			//54
				neighbours.push_back(index - W);			//51
				neighbours.push_back(index - W - 1);		//50
				neighbours.push_back(index + W);			//59
				neighbours.push_back(index + W - 1);		//58
				neighbours.push_back(index - W*H);			//39
				neighbours.push_back(index - W*H - 1);		//38
				neighbours.push_back(index - W*H - W);		//35
				neighbours.push_back(index - W*H - W - 1);	//34
				neighbours.push_back(index - W*H + W);		//43
				neighbours.push_back(index - W*H + W - 1);	//42
			}
			else if (z == 0 && y > 0 && y < H-1){//(7)
				neighbours.push_back(index - 1);		// 6
				neighbours.push_back(index - W);		//3
				neighbours.push_back(index - W - 1);	//2
				neighbours.push_back(index + W);		//11
				neighbours.push_back(index + W - 1);	//10
				neighbours.push_back(index + W*H);		//23
				neighbours.push_back(index + W*H - 1);  //22
				neighbours.push_back(index + W*H - W);  //19
				neighbours.push_back(index + W*H - W - 1); //18
				neighbours.push_back(index + W*H + W);	//27
				neighbours.push_back(index + W*H + W - 1); //26
			}
			else if (y != 0 && y != H-1 && z != 0 && z != L-1){ //Right side (23)
				neighbours.push_back(index - 1);					//22
				neighbours.push_back(index + W);					//27
				neighbours.push_back(index + W - 1);				//26
				neighbours.push_back(index - W);					//20
				neighbours.push_back(index - W - 1);				//19
				neighbours.push_back(index + W*H);					//39
				neighbours.push_back(index + W*H - 1);				//38
				neighbours.push_back(index + W*H + W);				//43
				neighbours.push_back(index + W*H + W - 1);			//42
				neighbours.push_back(index + W*H - W);				//35
				neighbours.push_back(index + W*H - W - 1);			//34
				neighbours.push_back(index - W*H);					//7
				neighbours.push_back(index - W*H - 1);				//6
				neighbours.push_back(index - W*H + W);				//11
				neighbours.push_back(index - W*H + W - 1);			//10
				neighbours.push_back(index - W*H - W);				//3
				neighbours.push_back(index - W*H - W - 1);			//2
			}
		}
		break;
	default:
		if (x > 0 && x < W-1 && y > 0 && y < H-1 && z > 0 && z < L-1){ //(21) //Centralt

			neighbours.push_back(index - 1);					//20
			neighbours.push_back(index + 1);					//22
			neighbours.push_back(index + W);					//25
			neighbours.push_back(index + W - 1);				//24
			neighbours.push_back(index + W + 1);				//26
			neighbours.push_back(index - W);					//17
			neighbours.push_back(index - W - 1);				//16
			neighbours.push_back(index - W + 1);				//18

			neighbours.push_back(index - W*H);					//5
			neighbours.push_back(index - W*H - 1);				//4
			neighbours.push_back(index - W*H + 1);				//6
			neighbours.push_back(index - W*H - W);				//1
			neighbours.push_back(index - W*H - W - 1);			//0
			neighbours.push_back(index - W*H - W + 1);			//2
			neighbours.push_back(index - W*H + W);				//9
			neighbours.push_back(index - W*H + W - 1);			//8
			neighbours.push_back(index - W*H + W + 1);			//10

			neighbours.push_back(index + W*H);					//37
			neighbours.push_back(index + W*H - 1);				//36
			neighbours.push_back(index + W*H + 1);				//38
			neighbours.push_back(index + W*H - W);				//33
			neighbours.push_back(index + W*H - W - 1);			//32
			neighbours.push_back(index + W*H - W + 1);			//34
			neighbours.push_back(index + W*H + W);				//41
			neighbours.push_back(index + W*H + W - 1);			//40
			neighbours.push_back(index + W*H + W + 1);			//42
		}
		else if (y == H-1 && x > 0 && x < W-1 && z > 0 && z < L-1){ //(29) Bottenplatta

			neighbours.push_back(index - 1);					//28
			neighbours.push_back(index + 1);					//30
			neighbours.push_back(index - W);					//25
			neighbours.push_back(index - W - 1);				//24
			neighbours.push_back(index - W + 1);				//26

			neighbours.push_back(index - W*H);					//13
			neighbours.push_back(index - W*H - 1);				//12
			neighbours.push_back(index - W*H + 1);				//14
			neighbours.push_back(index - W*H - W);				//9
			neighbours.push_back(index - W*H - W - 1);			//8
			neighbours.push_back(index - W*H - W + 1);			//10

			neighbours.push_back(index + W*H);					//45
			neighbours.push_back(index + W*H - 1);				//44
			neighbours.push_back(index + W*H + 1);				//46
			neighbours.push_back(index + W*H - W);				//41
			neighbours.push_back(index + W*H - W - 1);			//40
			neighbours.push_back(index + W*H - W + 1);			//42

		}
		else if (y == 0 && x < W-1 && z > 0 && z < L-1){	//(17) Topplatta

			neighbours.push_back(index - 1);					//16
			neighbours.push_back(index + 1);					//18
			neighbours.push_back(index + W);					//21
			neighbours.push_back(index + W - 1);				//20
			neighbours.push_back(index + W + 1);				//22

			neighbours.push_back(index + W*H);					//33
			neighbours.push_back(index + W*H - 1);				//32
			neighbours.push_back(index + W*H + 1);				//34
			neighbours.push_back(index + W*H + W);				//37
			neighbours.push_back(index + W*H + W - 1);			//36
			neighbours.push_back(index + W*H + W + 1);			//38

			neighbours.push_back(index - W*H);					//1
			neighbours.push_back(index - W*H - 1);				//0
			neighbours.push_back(index - W*H + 1);				//2
			neighbours.push_back(index - W*H + W);				//5
			neighbours.push_back(index - W*H + W - 1);			//4
			neighbours.push_back(index - W*H + W + 1);			//6

		}
		else if (z == 0 && x > 0 && x < W-1 && y > 0 && y < H-1){	//(5) Bortaplatta

			neighbours.push_back(index - 1);					//4
			neighbours.push_back(index + 1);					//6
			neighbours.push_back(index + W);					//9
			neighbours.push_back(index + W - 1);				//8
			neighbours.push_back(index + W + 1);				//10
			neighbours.push_back(index - W);					//1
			neighbours.push_back(index - W - 1);				//0
			neighbours.push_back(index - W + 1);				//2

			neighbours.push_back(index + W*H);					//21
			neighbours.push_back(index + W*H - 1);				//20
			neighbours.push_back(index + W*H + 1);				//22
			neighbours.push_back(index + W*H + W);				//25
			neighbours.push_back(index + W*H + W - 1);			//24
			neighbours.push_back(index + W*H + W + 1);			//26
			neighbours.push_back(index + W*H - W);				//17
			neighbours.push_back(index + W*H - W - 1);			//16
			neighbours.push_back(index + W*H - W + 1);			//18

		}
		else if (z == L-1 && x > 0 && x < W-1 && y > 0 && y < H-1){ //(53) //Motplattan

			neighbours.push_back(index - 1);					//52
			neighbours.push_back(index + 1);					//54
			neighbours.push_back(index + W);					//57
			neighbours.push_back(index + W - 1);				//56
			neighbours.push_back(index + W + 1);				//58

			neighbours.push_back(index - W);					//49
			neighbours.push_back(index - W - 1);				//48
			neighbours.push_back(index - W + 1);				//50

			neighbours.push_back(index - W*H);					//37
			neighbours.push_back(index - W*H - 1);				//36
			neighbours.push_back(index - W*H + 1);				//38

			neighbours.push_back(index - W*H - W);				//33
			neighbours.push_back(index - W*H - W - 1);			//32
			neighbours.push_back(index - W*H - W + 1);			//34

			neighbours.push_back(index - W*H + W);				//41
			neighbours.push_back(index - W*H + W - 1);			//40
			neighbours.push_back(index - W*H + W + 1);			//42
		}
		else if (y == H-1 && z == 0 && x > 0 && x < W-1){			//(13)

			neighbours.push_back(index - 1);					//12
			neighbours.push_back(index + 1);					//14
			neighbours.push_back(index - W);					//9
			neighbours.push_back(index - W - 1);				//8
			neighbours.push_back(index - W + 1);				//10

			neighbours.push_back(index + W*H);					//29
			neighbours.push_back(index + W*H - 1);				//28
			neighbours.push_back(index + W*H + 1);				//30

			neighbours.push_back(index + W*H - W);				//25
			neighbours.push_back(index + W*H - W - 1);			//24
			neighbours.push_back(index + W*H - W + 1);			//26
		}
		else if (y == H-1 && z == L-1 && x > 0 && x < W-1){			//(61)

			neighbours.push_back(index - 1);					//60
			neighbours.push_back(index + 1);					//62

			neighbours.push_back(index - W);					//57
			neighbours.push_back(index - W - 1);				//56
			neighbours.push_back(index - W + 1);				//58

			neighbours.push_back(index - W*H);					//45
			neighbours.push_back(index - W*H - 1);				//44
			neighbours.push_back(index - W*H + 1);				//46

			neighbours.push_back(index - W*H - W);				//41
			neighbours.push_back(index - W*H - W - 1);			//40
			neighbours.push_back(index - W*H - W + 1);			//42
		}
		else if (z == L-1 && y == 0 && x > 0 && x < W-1){			//(49)

			neighbours.push_back(index - 1);					//48
			neighbours.push_back(index + 1);					//50

			neighbours.push_back(index + W);					//53
			neighbours.push_back(index + W - 1);				//52
			neighbours.push_back(index + W + 1);				//54

			neighbours.push_back(index - W*H);					//33
			neighbours.push_back(index - W*H - 1);				//32
			neighbours.push_back(index - W*H + 1);				//34

			neighbours.push_back(index - W*H + W);				//37
			neighbours.push_back(index - W*H + W - 1);			//36
			neighbours.push_back(index + W*H + W + 1);			//38
		}
		else if (z == 0 && y == 0 && x > 0 && x < W-1){			//(1)
			
			neighbours.push_back(index - 1);					//0
			neighbours.push_back(index + 1);					//2

			neighbours.push_back(index + W);					//5
			neighbours.push_back(index + W - 1);				//4
			neighbours.push_back(index + W + 1);				//6

			neighbours.push_back(index + W*H);					//17
			neighbours.push_back(index + W*H - 1);				//16
			neighbours.push_back(index + W*H + 1);				//18

			neighbours.push_back(index + W*H + W);				//21
			neighbours.push_back(index + W*H + W - 1);			//20
			neighbours.push_back(index + W*H + W + 1);			//22
		}
		break;
	}
}

// Return this cell's index;
const int &Cell::getIndex() const{
	return index;
}
