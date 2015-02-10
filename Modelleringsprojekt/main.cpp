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

const float VISCOUSITY = 2.5f;
const float PARTICLE_MASS = .14f;
const double h = 0.032f;
const float STIFFNESS = 3.f;

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[GRID_WIDTH * GRID_HEIGHT];


// Create all the particles
void CreateParticles()
{
    for(int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].CreateParticle();
        
    }
    
    int k = 0, j = 0;
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        if(i % 10 == 0)
            k++;
        
        if(i % 10 == 0)
            j=0;
        
        j++;
        
        particles[i].setPos(glm::vec3(10 + j*0.2, 400 - k*0.2, 0.5));
        
    }
    
    for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
        cells[j].CreateCell(j);
    }
}

void calculateDensity(){

    for(int i = 0; i < NUM_PARTICLES; i++){
        
        float density_sum = 0;
       
        int cellIndex = particles[i].getCellIndex();
        //std::cout << cellIndex;
        
        vector<int> current_cells = cells[cellIndex].getNeighbours();
        
      //  std::cout << "SIZE" << current_cells.size() << std::endl;
        
        //std::cout << current_cells.size();
    
        for(int j = 0; j < current_cells.size(); j++){
            
            // Loop through all neighbouring particles
            for(int k = 0; k < cells[current_cells.at(j)].getParticles().size(); k++){
                
                Particle n = cells[current_cells.at(j)].getParticles().at(k);
                
                glm::vec3 diffvec = particles[i].getPos() - n.getPos();
                diffvec/=512.f;
                
                float abs_diffvec = glm::length(diffvec);

                //std::cout << "ABS DIFFVEC: " << abs_diffvec << std::endl;
                
                if(abs_diffvec < h){
               		
                    density_sum += PARTICLE_MASS * (315 / (64*M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)),3.0);
                    
                }
                
            }
            
        }
        
        //std::cout << "DENSITY SUM: " << density_sum << std::endl;
        if(density_sum != 0)
            particles[i].setDensity(density_sum);
        
    }

}

void calculatePressure(){
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        //std::cout << "PRESSURE " << particles[i].getDensity();
        particles[i].setPressure(STIFFNESS*(particles[i].getDensity() - 1000.f));
        
    }
    
}


void calculateForces(){
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        glm::vec3 gravity = glm::vec3(0, -9.82f*particles[i].getDensity(), 0);
        glm::vec3 pressure = glm::vec3(0);
        glm::vec3 viscousity = glm::vec3(0);
        
        int cellIndex = particles[i].getCellIndex();
        
        vector<int> current_cells = cells[cellIndex].getNeighbours();
        
        //std::cout << "SIZE" << current_cells.size() << std::endl;
        
        for(int j = 0; j < current_cells.size(); j++){
            
            // Loop through all neighbouring particles
            for(int k = 0; k < cells[current_cells.at(j)].getParticles().size(); k++){
                
                Particle n = cells[current_cells.at(j)].getParticles().at(k);
                
                if(n.getPos() == particles[i].getPos())
                    continue;
                
                glm::vec3 diffvec = particles[i].getPos() - n.getPos();
                diffvec/=512.f;
                
                float abs_diffvec = glm::length(diffvec);

                if(abs_diffvec < h){
                    
                    float W_const_pressure = 45/(M_PI * glm::pow(h, 6.0)) * glm::pow(h - abs_diffvec, 3.0) / abs_diffvec;
                    
                    glm::vec3 W_pressure_gradient = glm::vec3(W_const_pressure * diffvec.x, W_const_pressure * diffvec.y, 0);
                    
                    float visc_gradient = (45/(M_PI * glm::pow(h, 6.0)))*(h - abs_diffvec);
                    
                    pressure +=  PARTICLE_MASS * ((particles[i].getPressure() + n.getPressure()) / (2 * n.getDensity())) * W_pressure_gradient;
                    
                    viscousity += VISCOUSITY * PARTICLE_MASS * ((n.getVelocity() - particles[i].getVelocity()) / (n.getDensity())) * visc_gradient;
                    
                    
                }
                
            }
            
        }
        
        particles[i].setViscousityForce(viscousity);
        particles[i].setPressureForce(pressure);
        particles[i].setGravityForce(gravity);
        
    }

}

void display()
{
    
    // Clear all particles in cells
    for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
        
		cells[j].clearParticles();
        
	}
    
    // Push every particle into corresponding cell
    for(int i = 0; i < NUM_PARTICLES; i++) {
        
		cells[particles[i].getCellIndex()].addParticle(particles[i]);
        
        //std::cout << "index:" << particles[i].getCellIndex() << std::endl;
        
	}
    
    
    calculateDensity();
    calculatePressure();
    calculateForces();
    
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particles[i].EvolveParticle();
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
{/*
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particles[i].EvolveParticle();
    }*/
    
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
