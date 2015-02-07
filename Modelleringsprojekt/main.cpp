#define GLEW_STATIC
#include <GL/glew.h>

#include "Particle.h"
#include "Cell.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <thread>

const int NUM_PARTICLES = 15000;
const int GRID_WIDTH = 16;
const int GRID_HEIGHT = 16;

Particle particles[NUM_PARTICLES];

Cell cells[GRID_WIDTH * GRID_HEIGHT];

void init(){
	
	for (int i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++){

		cells[i].setNeighbours(i);

	}

	cells[255].write();
	
	for (int i = 0; i <= NUM_PARTICLES; i++){
		particles[i].CreateParticle();
	}

}



void display(){

	for (int i = 0; i < NUM_PARTICLES; i++){
		
		particles[i].setCellIndex();

		cells[particles[i].getCellIndex()].AddParticleToCell(particles[i]);

		particles[i].DrawObjects();
	}
}

void idle(){
	for (int i = 0; i < NUM_PARTICLES; i++){
		particles[i].EvolveParticle();
	}
}

void reshape_window(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(int argc, char *argv[]){
	init();

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(512, 512, "OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window)){
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0,0,width,height);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0.0, 512.0, 0.0, 512.0, -1, 1);
		display();
		idle();

		glfwSetWindowSizeCallback(window, reshape_window);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return EXIT_SUCCESS;
}

