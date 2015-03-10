/*****************************************************************************
 * Copyright (c) 2013-2014 Intel Corporation
 * All rights reserved.
 *
 * WARRANTY DISCLAIMER
 *
 * THESE MATERIALS ARE PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR ITS
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THESE
 * MATERIALS, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Intel Corporation is the author of the Materials, and requests that all
 * problem reports or change requests be submitted to it directly
 *****************************************************************************/


typedef struct{
	int neighbours[30];
	int particleIDs[2000];
	int nrOfParticles;
	int nrOfNeighbours;
} Cell;


int getCellIndex(float4 pos) {
	float4 cell = (float4)(floor(pos.x / 256.f * (256.f / 8.f)), floor(pos.y / 256.f * (256.f / 8.f)), floor(pos.z / 256.f * (256.f / 8.f)), 0);
	int _cellIndex = (int)cell.x % 32 + (int)cell.y * 32 + (int)cell.z * 32 * 32;
	//printf(" Cell  x = %4.5f  y = %4.5f  z = %4.5f \n ", cell.x, cell.y, cell.z);
	//printf(" Pos  x = %4.5f  y = %4.5f  z = %4.5f \n ", pos.x, pos.y, pos.z);
	//printf("PARTICLE ID %i \n", (int)2 % 32);
	/*if (_cellIndex == 0){
		printf(" CellIndex %i \n ", _cellIndex);
		printf(" Pos  x = %4.5f  y = %4.5f  z = %4.5f \n ", pos.x, pos.y, pos.z);
		printf(" Nr OF particles %i \n ", cell[cellInd].nrOfParticles);
	}*/
	return _cellIndex;
}


__kernel void setCellParticles(__global float4* position,__global Cell *cell)
{
	const int id = get_global_id(0);
	const int NUM_PARTICLES = get_global_size(0);

	int cellInd = getCellIndex(position[id]);
	
	//Insert position/particle of particle in given Cell
	//printf("PARTICLE ID %i \n", id);
	if (cell[cellInd].nrOfParticles < 2000)
	{
		cell[cellInd].particleIDs[cell[cellInd].nrOfParticles] = id;
		cell[cellInd].nrOfParticles = cell[cellInd].nrOfParticles + 1;
	}
	//if (cellInd == 0){
	//	printf(" CellIndex %i \n ", cellInd);
	//	//printf(" Pos  x = %4.5f  y = %4.5f  z = %4.5f \n ", pos.x, pos.y, pos.z);
	//	printf(" Nr OF particles %i \n ", cell[cellInd].nrOfParticles);
	//}
	

}

//, __global float* velocity, __global float* viscosity_f, __global float* pressure_f,__global float* pressure,__global float* density)
__kernel void calculateDensityAndPressure(__global float4* position, __global float4* velocity,
	__global float4* viscosity_f, __global float4* pressure_f, __global float4* gravity_f,
	__global float* pressure, __global float* density, __global Cell *cell)
{
	const int id = get_global_id(0);
	//const int id_cells = get_global_id(1);
	const int NUM_PARTICLES = get_global_size(0);
	const int NUM_CELLS = get_global_size(1);

	const float PARTICLE_MASS = 500 * .14f;
	const float h = 13.f;
	const float STIFFNESS = 500 * 5.f;


	//int neighbours = cell[cellindex].neighbours;
	int cellInd = getCellIndex(position[id]);
	/*printf("Position X %4.5f \n", position[id].x);
	printf("cell[cellInd].nrOfParticles %i \n", cell[cellInd].nrOfParticles);*/

	//printf("NR OF PARTICLES IN CELL %i \n", cell[cellInd].nrOfParticles);
	//printf("id_cells %i \n", id);
	//printf("cellindex %i \n", cell[id].neighbours[2]);

	float density_sum = 0.000;
	//printf("NrOf neighbours ID %i \n", cell[cellInd].nrOfNeighbours);

	for (int i = 0; i < cell[cellInd].nrOfNeighbours; i++){
		int neighbourCellInd = cell[cellInd].neighbours[i];

			//printf("NEIGHBOUR CELL INDEX %i Och nrOfNEih %i \n", i, cell[cellInd].nrOfNeighbours);
			for (int k = 0; k < cell[neighbourCellInd].nrOfParticles; k++){
				int particleID = cell[neighbourCellInd].particleIDs[k];
				//printf("index K  %i \n", k);

				if (particleID >= 0){



					//Calculate on same particle!!! No if(id != j)
					float4 diffvec = position[id] - position[particleID];

					//Absvec 
					float abs_diffvec = sqrt(pow(diffvec.x, 2.0f) + pow(diffvec.y, 2.0f) + pow(diffvec.z, 2.0f));
					//if (abs_diffvec == 0 && particleID != id)
						//printf("DIFFVEC = 0 , position X  =  %4.8f  position Y =  %4.8f  position Z  =  %4.8f and particleId = %i Cellindex =  %i \n", position[id].x, position[id].y, position[id].z , particleID, cellInd);

					//printf("ABS DIFFVEC %4.8f \n", abs_diffvec);

					if (abs_diffvec < h){
						/*printf("ABS DIFFVEC %4.8f \n", abs_diffvec);*/
						density_sum += PARTICLE_MASS* (315.0f / (64.0f * M_PI * pow(h, 9.0f))) * pow((pow(h, 2.0f) - pow(abs_diffvec, 2.0f)), 3.0f);
						if (particleID == id){
							/*printf("Particle  ID THE SAME !! %i andra %i \n", particleID, id);*/
							//printf("Density_sum %4.8f \n", density_sum);
						}
					}
				
				}
				

			}
			
		}
	

	if (density_sum == 0)
	{
		density_sum += PARTICLE_MASS* (315.0f / (64.0f * M_PI * pow(h, 9.0f))) * pow((pow(h, 2.0f) - pow(0, 2.0f)), 3.0f);
		//printf("\n \n Density SUM = 0 Cellindex =  %i \n ", cellInd);
		//for (int i = 0; i < cell[cellInd].nrOfNeighbours; i++){
		//	int neighbourCellInd = cell[cellInd].neighbours[i];

		//	//printf("NEIGHBOUR CELL INDEX %i Och nrOfNEih %i \n", i, cell[cellInd].nrOfNeighbours);
		//	for (int k = 0; k < cell[neighbourCellInd].nrOfParticles; k++){
		//		int particleID = cell[neighbourCellInd].particleIDs[k];
		//		printf("particle Id in CELL = %i  THIS partilce ID = %i \n ", cell[cellInd].particleIDs[i], id);
		//	}
		//}
		
	}
	


	density[id] = density_sum;

	pressure[id] = STIFFNESS * (density_sum - 998.11f);

	//std::fill(cells[i].particleIDs, cells[i].particleIDs + 1000, -1);


}

	

 
__kernel void calculateAccelerations(__global float4* position, __global float4* velocity,
	__global float4* viscosity_f, __global float4* pressure_f, __global float4* gravity_f,
	__global float* pressure, __global float* density, __global Cell *cell, __global float* gravity_d)
{
	const int id = get_global_id(0);
	const int NUM_PARTICLES = get_global_size(0);

	//printf("gravity Dir %f3.3 \n", gravity_d[0]);
	const float VISCOUSITY = 500 * 5.0f;
	const float PARTICLE_MASS = 500 * .14f;
	const float h = 13.f;
	const float GRAVITY_CONST = 80000 * 9.82f;


	//NOW CALCULATE FORCES 
	float4 gravityforce = (float4)(gravity_d[0] * GRAVITY_CONST * density[id], gravity_d[1] * GRAVITY_CONST * density[id], gravity_d[2] * GRAVITY_CONST * density[id], 0);  //(float4)(gravity_f[id].x * density[id], gravity_f[id].y * density[id], gravity_f[id].z * density[id], 0);

	float4 pressureforce = (float4)(0, 0, 0, 0);
	float4 viscosityforce = (float4)(0, 0, 0, 0);

	//printf(" Position x %4.8f \n ", position[id].x);
	int cellInd = getCellIndex(position[id]);


	for (int i = 0; i < cell[cellInd].nrOfNeighbours; i++){
		int neighbourCellInd = cell[cellInd].neighbours[i];
		for (int k = 0; k < cell[neighbourCellInd].nrOfParticles; k++){
			int particleID = cell[neighbourCellInd].particleIDs[k];

			if (particleID >= 0){


				//Dont calculate same particle
				if (id != particleID){
					float4 diffvec = position[id] - position[particleID];

					//Absvec 
					float abs_diffvec = sqrt(pow(diffvec.x, 2.0f) + pow(diffvec.y, 2.0f) + pow(diffvec.z, 2.0f));

					if (abs_diffvec < h){

						float W_const_pressure = 45.0f / (M_PI * pow(h, 6.0f)) * pow(h - abs_diffvec, 3.0f) / abs_diffvec;
						//printf("W_const_pressure :   %4.8f \n", W_const_pressure);
						float4 W_pressure_gradient = (float4)(W_const_pressure * diffvec.x, W_const_pressure * diffvec.y, W_const_pressure * diffvec.z, 0);
						//printf("W_pressure_gradient :   %4.8f \n", W_pressure_gradient.x);
						float visc_gradient = (45.f / (M_PI* pow(h, 6.0f)))*(h - abs_diffvec);
						//printf("visc_gradient :   %4.8f \n", visc_gradient);
						pressureforce += -PARTICLE_MASS * ((pressure[id] + pressure[particleID]) / (2.0 * density[particleID])) * W_pressure_gradient;

						viscosityforce += VISCOUSITY * PARTICLE_MASS * ((velocity[particleID] - velocity[id]) / (density[particleID])) * visc_gradient;

					}
				}
			}


		}
	}



	//printf("Particle Pressure Force:   %4.8f \n", pressureforce.x);
	//printf("Particle Gravity Force Y:   %4.8f \n", gravityforce.y);
	viscosity_f[id] = viscosityforce;
	pressure_f[id] = pressureforce;
	gravity_f[id] = gravityforce;



	/*for (int i = 0; i < cell[cellInd].nrOfNeighbours; i++){

		cell[cellInd].particleIDs[i] = -1;
	}*/

	cell[cellInd].nrOfParticles = 0;
	
	
}

__kernel void moveParticles(__global float4* position, __global float4* velocity,
	__global float4* viscosity_f, __global float4* pressure_f, __global float4* gravity_f,
	__global float* pressure, __global float* density, __global Cell *cell)
{
	const int id = get_global_id(0);
	const int NUM_PARTICLES = get_global_size(0);

	int cellInd = getCellIndex(position[id]);
	//for( int i = 0;  i < 32000; i++){ 
	//	//cell[cellInd].particleIDs[i] = -1;
	//	
	//}
	

	float dt = 0.0001f;

	float4 newPos = position[id] + dt * velocity[id] + dt * dt* ((gravity_f[id] +  pressure_f[id] + viscosity_f[id]) / density[id]) / 2.f;
	float4 newVel = (newPos - position[id]) / dt;

	position[id] = newPos;
	velocity[id] = newVel;

	/*if(velocity[id].x == 0 && velocity[id].y == 0  && velocity[id].z == 0){ 
		printf("Velocity X = 0 , Particle ID = %i \n", id );
	}*/
	
	if (position[id].x < 8){
		velocity[id].x = -0.6f*velocity[id].x;
		position[id].x = 8.f;
	}

	else if (position[id].x > 248){

		velocity[id].x = -0.6f*velocity[id].x;
		position[id].x = 248.f;
	}

	if (position[id].y < 8){
		velocity[id].y = -0.6f*velocity[id].y;
		position[id].y = 8.f;
	}


	else if (position[id].y > 248){
		velocity[id].y = -0.6f*velocity[id].y;
		position[id].y = 248.f;
	}

	if (position[id].z < 8){
		velocity[id].z = -0.6f*velocity[id].z;
		position[id].z = 8.f;
	}
	else if (position[id].z > 248){
		velocity[id].z = -0.6f*velocity[id].z;
		position[id].z = 248.f;
	}
	//printf("PARTICLE POSITION Z %f4.8 \n ", position[id].z);


	

	
	



}


