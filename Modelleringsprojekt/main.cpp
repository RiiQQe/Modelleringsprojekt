#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include "Particle.h"
#include "Box.h"
#include "Cell.h"
#include <GLFW/glfw3.h>
#include <thread>

const int NUM_PARTICLES = 100;
//const int GRID_WIDTH = 20;
//const int GRID_HEIGHT = 20;

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

void display()
{
    
	
	int cellInd = 0;
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		particles[i].EvolveParticle();
		particles[i].DrawObjects();
		cellInd = particles[i].getCellIndex();
		cells[cellInd].addCellParticle(particles[i]);

	}
	
	for (int k = 0; k < GRID_WIDTH * GRID_HEIGHT; k++) {

		vector <int> cell_neighbours = cells[k].getNeighbourIndexes();
		vector <Particle> cell_particles = cells[k].getCellParticles();
		for (int i = 0; i < cell_particles.size(); i++){

			for (vector<int>::iterator n_iterator = cell_neighbours.begin(); n_iterator != cell_neighbours.end(); n_iterator++) {
				//cout << "Neighbours index " << *n_iterator << endl;

				vector <Particle> neighbours = cells[*n_iterator].getCellParticles();
				for (vector<Particle>::iterator p_iterator2 = neighbours.begin(); p_iterator2 != neighbours.end(); p_iterator2++){
					if (cell_particles[k].getPos() != (*p_iterator2).getPos()){
						glBegin(GL_LINES);
						glColor3f(1, 1, 0);
						glVertex2f(cell_particles[k].getPos().x, cell_particles[k].getPos().y);
						glVertex2f((*p_iterator2).getPos().x, (*p_iterator2).getPos().y);
						glEnd();
					}
				}
			}
		}


	}
	for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
		cells[j].clearCellParticles();
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



