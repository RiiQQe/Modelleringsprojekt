#define GLEW_STATIC
#define _USE_MATH_DEFINES
#include <GL/glew.h>

#include <cmath>
#include <iostream>
#include "Particle.h"
#include "Box.h"
#include "Cell.h"
#include <GLFW/glfw3.h>
#include <thread>

const int NUM_PARTICLES = 300;
const float density_0 = 998.0f;
const float K = 3.0f;
const float H = 0.045f;
const int neighbour_limit = 100;
const float PARTICLE_MASS = 0.02;

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[GRID_WIDTH * GRID_HEIGHT];



// Create all the particles
void Init()
{
	int cellInd = 0;
	for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++){
		cells[i].setNeighbours(i);
	}
    
    for(int i = 0; i <= NUM_PARTICLES; i++)
    {
        particles[i].CreateParticle();
		cellInd = particles[i].getCellIndex();
		cells[cellInd].addCellParticle(particles[i]);
    }
	
   
}

float WviscosityLaplacian(double radiusSquared) {

	static double coefficient = 45.0 / (M_PI*pow(H, 6));

	double radius = sqrt(radiusSquared);

	return coefficient * (H - radius);
}

void calculateAcceleration(){
	for (int k = 0; k < NUM_PARTICLES; k++)
	{
		int index = particles[k].getCellIndex();
		Particle &p = particles[k];
		vector <int> cell_neighbours = cells[index].getNeighbourIndexes();
		//GRADIENT
		float density_p = p.getDensity();
		float pressure_p = p.getPressure();
		glm::vec3 velocity_p = p.getVelocity();
		for (int i = 0; i < cell_neighbours.size(); i++){
			vector <Particle*> neighbours = cells[i].getCellParticles();
			int n_limit = neighbours.size() < neighbour_limit ? neighbours.size() : neighbour_limit;
			for (int j = 0; j < n_limit; j++) {
				Particle *n = neighbours[j];
				float r = glm::length(particles[k].getPos() - n->getPos()) / 512.f;
				if (r < H*H){
					glm::vec3 r_vec = p.getPos() - n->getPos() / 512.f;
					float density_n = n->getDensity();
					float pressure_n = n->getPressure();
					glm::vec3 velocity_n = n->getVelocity();
					float w_scal = -(1890.f / (64.f* (float)M_PI * pow(H, 9.0f)))*pow((pow(H, 2.0f) - pow(r, 2.0f)), 2.0f);
					glm::vec3 W_grad = glm::vec3(r_vec.x * w_scal, r_vec.y * w_scal, r_vec.z * w_scal);
					//cout << "pressure_p    " << pressure_p << " pressure_n    " << pressure_n << endl;
					glm::vec3 A_pressure = (pressure_p + pressure_n) / (2.0f * density_p*density_n) * W_grad;
					glm::vec3 A_viscocity = (velocity_p - velocity_n) / (density_p * density_n) * WviscosityLaplacian(glm::length(r_vec));

					p.addPressureForce(A_pressure);
					p.addViscosityForce(A_viscocity);
					//cout << "FORCE X   " << A_pressure.x << endl;
					



				}

			}

		}

	}

}

void updateParticleParam(){
	for (int k = 0; k < NUM_PARTICLES; k++) {
		int index = particles[k].getCellIndex();
		Particle &p = particles[k];
		vector <int> cell_neighbours = cells[index].getNeighbourIndexes();
		float sum_density = 0;
		//vector <Particle> cell_particles = cells[k].getCellParticles();
		for (int i = 0; i < cell_neighbours.size(); i++){

			vector <Particle*> neighbours = cells[cell_neighbours[i]].getCellParticles();
			int n_limit = neighbours.size() < neighbour_limit ? neighbours.size() : neighbour_limit;
			for (int j = 0; j < n_limit; j++) {
				Particle *n = neighbours[j];
				float r = glm::length(particles[k].getPos() - n->getPos()) / 512.f;
				if (r <= H*H){
					float W = (315.f / (64.f * (float)(M_PI)*pow(H, 9.0f)))* pow((pow(H, 2.0f) - pow(r, 2.0f)), 3.0f);
					sum_density += W;

					//cout << "Neighbours index " << neighbours.size() << endl;
					/*glBegin(GL_LINES);
					glColor3f(1, 1, 0);
					glVertex2f(p.getPos().x, p.getPos().y);
					glVertex2f(n->getPos().x, n->getPos().y);
					glEnd();*/
				}

			}



		}
		if (sum_density != 0)
		{
			p.addDensity(sum_density);
		}else 
		{
			sum_density = 500.f;
			p.addDensity(sum_density);
		}
		float P = K*(sum_density - density_0);
		p.addPressure(P);
		//cout << "DENSITY  " << p.getDensity() << endl;
	}
}

void display()
{
	
	int cellInd = 0;
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		cells[particles[i].getCellIndex()].addCellParticle(particles[i]);
	}
	
	updateParticleParam();

	//Calculate pressure acceleration
	calculateAcceleration();


	
	for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
		cells[j].clearCellParticles();
	}
	for (int i = 0; i < NUM_PARTICLES; i++){
		particles[i].EvolveParticle();
		particles[i].DrawObjects();
		particles[i].resetValues();
	}

	
	

}

void reshape_window(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void idle()
{
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particles[i].EvolveParticle();
    }
    
    //glutPostRedisplay();
}


int main(int argc, char *argv[])
{
    
    Init();
    
    glfwInit();
    
    GLFWwindow* window = glfwCreateWindow(512, 512, "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    
    while(!glfwWindowShouldClose(window)){
        
        float ratio;
        int width, height;
       
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 512.0, 0.0, 512.0, -1, 1);
        
        display();
        //idle();
        
        box.DrawBox();
        
        //Swap front and back buffers
        glfwSetWindowSizeCallback(window, reshape_window);
        glfwSwapBuffers(window);
        
        //Poll for and process events
        glfwPollEvents();
        
    }
    
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return EXIT_SUCCESS;
    
}



