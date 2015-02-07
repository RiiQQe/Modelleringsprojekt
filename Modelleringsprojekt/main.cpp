#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include "Particle.h"
#include "Box.h"
#include "Cell.h"
#include <GLFW/glfw3.h>
#include <thread>

const int NUM_PARTICLES = 100;
const int GRID_WIDTH = 16;
const int GRID_HEIGHT = 16;

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[GRID_WIDTH * GRID_HEIGHT];
int Particle::count = 1; 

// Create all the particles
void CreateParticles()
{
    for(int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].CreateParticle();
		std::cout << particles[i].count++ << std::endl;
    }
    
	for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
		cells[j].CreateCell(j);
	}
}

void display()
{
    for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
		//std::cout << j << ": contains " << cells[j].getParticles().size() << " particles." << std::endl;
		cells[j].clearParticles();
	}

    for(int i = 0; i < NUM_PARTICLES; i++) {
		//particles[i].setCellIndex();
		// We want to set the neightbours of each cell. How? 
		// Solution for now - push every particle into corresponding cell - beta style;

		cells[particles[i].getCellIndex()].addParticle(particles[i]);

        particles[i].DrawObjects();
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
    
    CreateParticles();
    
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
        idle();
        
        // box.DrawBox();
        
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
