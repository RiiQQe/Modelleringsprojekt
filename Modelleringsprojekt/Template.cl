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
	float4 positions[1000];
	int nrOfParticles;
} Cell;

int getCellIndex(float4 pos) {
	float4 cell = (float4)(floor(pos.x / 512.f * (512 / 32)), floor(pos.y / 512.f * (512 / 32)), floor(pos.z / 512.f * (512 / 32)), 0);
	int _cellIndex = (int)cell.x % 32 + (int)cell.y % 32 + (int)cell.z * 32;

	//std::cout << _cellIndex << std::endl;

	return _cellIndex;
}


__kernel void setCellParticles(__global float4* position,__global Cell *cell)
{
	const int id = get_global_id(0);
	const int NUM_PARTICLES = get_global_size(0);

	int cellInd = getCellIndex(position[id]);
	//Insert position/particle of particle in given Cell
	cell[cellInd].positions[cell[cellInd].nrOfParticles] = position[id];
	cell[cellInd].nrOfParticles++;
	//printf("NR OF PARTICLES IN CELL %i \n", cell[cellInd].nrOfParticles++);

}

//, __global float* velocity, __global float* viscosity_f, __global float* pressure_f,__global float* pressure,__global float* density)
__kernel void calculateDensityAndPressure(__global float4* position, __global float4* velocity,
	__global float4* viscosity_f, __global float4* pressure_f, __global float4* gravity_f,
	__global float* pressure, __global float* density, __global Cell *cell)
{
	const int id = get_global_id(0);
	const int NUM_PARTICLES = get_global_size(0);

	const float VISCOUSITY = 500 * 2.5f;
	const float PARTICLE_MASS = 500 * .14f;
	const float h = 16.f;
	const float STIFFNESS = 500 * 3.f;
	const float GRAVITY_CONST = 80000 * 9.82f;

	int cellindex = getCellIndex(position[id]);

	int neighbours = cell[cellindex].neighbours;
	//printf("cellindex %i \n", cellindex);

	float density_sum = 0;

	for (uint j = 0; j < NUM_PARTICLES; j++){
		//printf("ABS DIFFVEC");
		//Calculate on same particle!!! No if(id != j)
		float4 diffvec = position[id] - position[j];
		//Absvec 
		float abs_diffvec = sqrt(pow(diffvec.x, 2.0f) + pow(diffvec.y, 2.0f) + pow(diffvec.z, 2.0f));


		if (abs_diffvec < h){
			//printf("ABS DIFFVEC %4.8f \n", abs_diffvec);
			density_sum += PARTICLE_MASS* (315.0f / (64.0f * M_PI * pow(h, 9.0f))) * pow((pow(h, 2.0f) - pow(abs_diffvec, 2.0f)), 3.f);

		}
	

	}

	density[id] = density_sum;

	pressure[id] = STIFFNESS*(density_sum - 998.f);


}

	

 
__kernel void calculateAccelerations(__global float4* position, __global float4* velocity,
	__global float4* viscosity_f, __global float4* pressure_f, __global float4* gravity_f,
	__global float* pressure, __global float* density)
{
	const int id = get_global_id(0);
	const int NUM_PARTICLES = get_global_size(0);

	/*const float VISCOUSITY = 500 * 5.f;
	const float PARTICLE_MASS = 500 * .14f;
	const float STIFFNESS = 500* 3.f;
	const float GRAVITY_CONST = 80000 * 9.82f;
	const float h = 16.f;
	*/
	const float VISCOUSITY = 500 * 2.5f;
	const float PARTICLE_MASS = 500 * .14f;
	const float h = 16.f;
	const float STIFFNESS = 500 * 3.f;
	const float GRAVITY_CONST = 80000 * 9.82f;

	//NOW CALCULATE FORCES 
	float4 gravityforce = (float4)(0, -GRAVITY_CONST * density[id], 0, 0);  //(float4)(gravity_f[id].x * density[id], gravity_f[id].y * density[id], gravity_f[id].z * density[id], 0);

	float4 pressureforce = (float4)(0, 0, 0, 0);
	float4 viscosityforce = (float4)(0, 0, 0, 0);

	for (int k = 0; k < NUM_PARTICLES; k++){


		//Dont calculate same particle
		if (id != k){
			float4 diffvec = position[id] - position[k];

			//Absvec 
			float abs_diffvec = sqrt(pow(diffvec.x, 2.0f) + pow(diffvec.y, 2.0f) + pow(diffvec.z, 2.0f));

			if (abs_diffvec < h){

				float W_const_pressure = 45.0f / (M_PI * pow(h, 6.0f)) * pow(h - abs_diffvec, 3.0f) / abs_diffvec;
				//printf("W_const_pressure :   %4.8f \n", W_const_pressure);
				float4 W_pressure_gradient = (float4)(W_const_pressure * diffvec.x, W_const_pressure * diffvec.y, W_const_pressure * diffvec.z, 0);
				//printf("W_pressure_gradient :   %4.8f \n", W_pressure_gradient.x);
				float visc_gradient = (45.f / (M_PI* pow(h, 6.0f)))*(h - abs_diffvec);
				//printf("visc_gradient :   %4.8f \n", visc_gradient);
				pressureforce += -PARTICLE_MASS * ((pressure[id] + pressure[k]) / (2.0 * density[k])) * W_pressure_gradient;

				viscosityforce += VISCOUSITY * PARTICLE_MASS * ((velocity[k] - velocity[id]) / (density[k])) * visc_gradient;

			}
		}


	}


	//printf("Particle Pressure Force:   %4.8f \n", pressureforce.x);
	//printf("Particle Gravity Force Y:   %4.8f \n", gravityforce.y);
	viscosity_f[id] = viscosityforce;
	pressure_f[id] = pressureforce;
	gravity_f[id] = gravityforce;



}

__kernel void moveParticles(__global float4* position, __global float4* velocity,
	__global float4* viscosity_f, __global float4* pressure_f, __global float4* gravity_f,
	__global float* pressure, __global float* density)
{
	const int id = get_global_id(0);
	const int NUM_PARTICLES = get_global_size(0);

	/*const float VISCOUSITY = 500 * 5.f;
	const float PARTICLE_MASS = 500 * .14f;
	const float STIFFNESS = 500* 3.f;
	const float GRAVITY_CONST = 80000 * 9.82f;
	const float h = 16.f;
	*/
	const float VISCOUSITY = 500 * 2.5f;
	const float PARTICLE_MASS = 500 * .14f;
	const float h = 16.f;
	const float STIFFNESS = 500 * 3.f;
	const float GRAVITY_CONST = 50000 * 9.82f;

	float dt = 0.0004f;

	float4 newPos = position[id] + dt * velocity[id] + dt * dt* ((gravity_f[id] +  pressure_f[id] + viscosity_f[id]) / density[id]) / 2.f;
	float4 newVel = (newPos - position[id]) / dt;

	position[id] = newPos;
	velocity[id] = newVel;

	if (position[id].x < 1){
		velocity[id].x = -0.8*velocity[id].x;
		position[id].x = 1;
	}

	else if (position[id].x > 256){

		velocity[id].x = -0.8*velocity[id].x;
		position[id].x = 256;
	}

	if (position[id].y < 1){
		velocity[id].y = -0.8*velocity[id].y;
		position[id].y = 1;
	}


	else if (position[id].y > 512){
		velocity[id].y = -0.8*velocity[id].y;
		position[id].y = 512;

	}

	if (position[id].z < 1){
		velocity[id].z = -0.8*velocity[id].z;
		position[id].z = 1;
	}
	else if (position[id].z > 256){
		velocity[id].z = -0.8*velocity[id].z;
		position[id].z = 256;
	}
	//printf("PARTICLE POSITION Z %f4.8 \n ", position[id].z);




}


