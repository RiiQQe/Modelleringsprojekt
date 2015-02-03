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

// Create all the particles
void CreateParticles()
{
    
    for(int i = 0; i <= NUM_PARTICLES; i++)
    {
        particles[i].CreateParticle();

		if (i == 0) 
			particles[i].setSpecial();
    }
    
/*	for (int j = 1; j <= GRID_WIDTH * GRID_HEIGHT; j++) {
		cells[j].CreateCell(j);
	}*/
}

void display()
{
    /*for (int j = 0; j <= GRID_WIDTH * GRID_HEIGHT; j++) {
		cells[j].clearParticles();
	}*/

    for(int i = 0; i <= NUM_PARTICLES; i++)
    {
		particles[i].setCellIndex();
		// We want to set the neightbours of each cell. How? 
		// Solution for now - push every particle into corresponding cell - beta style;

	//	glm::vec2 currCell = particles[i].getCell();
	//	int _index = (int) currCell.x % 16 + (int) (currCell.y * 16);

//		cells[_index + 1].addParticle(particles[i]);

		/*
		if (i == 0) {
			//std::cout << "(" << particles[i].getCell().x << ", " << particles[i].getCell().y << ")" << std::endl;
			glm::vec2 toDraw = particles[i].getCell();

			glBegin(GL_TRIANGLES);
				glColor3f(0.3f, 0.3f, 0.3f);
				glVertex2f(toDraw.x*32.f - 32.f, toDraw.y*32.f - 32.f);
				glVertex2f(toDraw.x*32.f + 64.f, toDraw.y*32.f - 32.f);
				glVertex2f(toDraw.x*32.f + 64.f, toDraw.y*32.f + 64.f);

				glVertex2f(toDraw.x*32.f - 32.f, toDraw.y*32.f - 32.f);
				glVertex2f(toDraw.x*32.f + 64.f, toDraw.y*32.f + 64.f);
				glVertex2f(toDraw.x*32.f - 32.f, toDraw.y*32.f + 64.f);	


				glColor3f(0.5f, 0.5f, 0.5f);
				glVertex2f(toDraw.x*32.f, toDraw.y*32.f);
				glVertex2f(toDraw.x*32.f + 32.f, toDraw.y*32.f);
				glVertex2f(toDraw.x*32.f + 32.f, toDraw.y*32.f + 32.f);

				glVertex2f(toDraw.x*32.f, toDraw.y*32.f);
				glVertex2f(toDraw.x*32.f + 32.f, toDraw.y*32.f + 32.f);
				glVertex2f(toDraw.x*32.f, toDraw.y*32.f + 32.f);	
			glEnd();
		}*/

        particles[i].DrawObjects();

    }

	//int currIndexToLookAt = particles[0].getCellIndex();
	int currIndexToLookAt;

	for (int k = 0; k <= NUM_PARTICLES; ++k) {

		currIndexToLookAt = particles[k].getCellIndex();

		for (int l = 0; l <= NUM_PARTICLES; ++l) {
		
            if (currIndexToLookAt == particles[l].getCellIndex() && k != l && glm::sqrt(glm::pow(particles[l].getPos().x - particles[k].getPos().x, 2) + glm::pow(particles[l].getPos().y - particles[k].getPos().y, 2)) < 64)
			{

				glBegin(GL_LINES);
				glColor3f(1, 1, 0);
					glVertex2f(particles[l].getPos().x, particles[l].getPos().y);
					glVertex2f(particles[k].getPos().x, particles[k].getPos().y);
				glEnd();
			}
			/*
			glBegin(GL_TRIANGLES);

				glColor3f(1, 1, 0);
				glVertex2f(particles[k].getCell().x*32.f - 32.f, particles[k].getCell().y*32.f - 32.f);
				glVertex2f(particles[k].getCell().x*32.f + 64.f, particles[k].getCell().y*32.f - 32.f);
				glVertex2f(particles[k].getCell().x*32.f + 64.f, particles[k].getCell().y*32.f + 64.f);
				glVertex2f(particles[k].getCell().x*32.f - 32.f, particles[k].getCell().y*32.f - 32.f);
				glVertex2f(particles[k].getCell().x*32.f + 64.f, particles[k].getCell().y*32.f + 64.f);
				glVertex2f(particles[k].getCell().x*32.f - 32.f, particles[k].getCell().y*32.f + 64.f);	

				glColor3f(1.f, 1.f, 1);
				glVertex2f(particles[k].getCell().x * 32.f, particles[k].getCell().y * 32.f);
				glVertex2f(particles[k].getCell().x * 32.f + 32.f, particles[k].getCell().y * 32.f + 32.f);
				glVertex2f(particles[k].getCell().x * 32.f + 32.f, particles[k].getCell().y * 32.f);
				glVertex2f(particles[k].getCell().x * 32.f, particles[k].getCell().y * 32.f);
				glVertex2f(particles[k].getCell().x * 32.f + 32.f, particles[k].getCell().y * 32.f + 32.f);
				glVertex2f(particles[k].getCell().x * 32.f, particles[k].getCell().y * 32.f + 32.f);
			glEnd();*/
			//break;
		}
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
        
//        box.DrawBox();
        
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



