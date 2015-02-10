#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include "Particle.h"
#include "Box.h"
#include "Cell.h"

#include <thread>

const int NUM_PARTICLES = 100;
const int GRID_WIDTH = 16;
const int GRID_HEIGHT = 16;

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[GRID_WIDTH * GRID_HEIGHT];
int Particle::count = 1;

// FPS specific vars
    double lastTime;
    int frames = 0;

// References and pointer needed globally
    GLFWwindow* window;

// Neat way of displaying FPS
void handleFps() {
    frames++;
    double currentTime = glfwGetTime() - lastTime;
    double fps = (double) frames / currentTime;
    
    if (currentTime > 1.0) {
        lastTime = glfwGetTime();
        frames = 0;
        
        std::ostringstream stream;
        stream << fps;
        std::string fpsString = "Betafluid 0.0.2 | FPS: " + stream.str();
        
        // Convert the title to a c_str and set it
        const char* pszConstString = fpsString.c_str();
        
        glfwSetWindowTitle(window, pszConstString);
    }	
}

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
    handleFps();
    
    // Clear all particles in cells
    for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
		cells[j].clearParticles();
	}
    
    // Push every particle into corresponding cell
    for(int i = 0; i < NUM_PARTICLES; i++) {
		cells[particles[i].getCellIndex()].addParticle(particles[i]);
	}
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        int cellIndex = particles[i].getCellIndex();
        vector<int> current_cells = cells[cellIndex].getNeighbours();
        
        for(int j = 0; j < current_cells.size(); j++){

            // Loop through all neighbouring particles
            for(int k = 0; k < cells[current_cells.at(j)].getParticles().size(); k++){
                
                Particle n = cells[current_cells.at(j)].getParticles().at(k);
                
                glBegin(GL_LINES);
                glColor3f(1.f, 1.f, 0.f);
                
                glVertex2f(particles[i].getPos().x, particles[i].getPos().y);
                glVertex2f(n.getPos().x, n.getPos().y);
                
                glEnd();
        
        	}
        }
    }
    
    for(int i = 0; i < NUM_PARTICLES; i++){
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
    
    window = glfwCreateWindow(512, 512, "Betafluid 0.0.2", nullptr, nullptr); // Windowed
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
