#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include "Particle.h"
#include "Box.h"
#include "Cell.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <sstream>

const int NUM_PARTICLES = 500;
const int GRID_WIDTH = 512;
const int GRID_HEIGHT = 512;

const float VISCOUSITY = 2.5f;
const float PARTICLE_MASS = .14f;
const double h = 0.032f;
const float STIFFNESS = 3.f;

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[GRID_WIDTH * GRID_HEIGHT];

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
        
    }
    
    int k = 0, j = 0;
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        if(i % 10 == 0)
            k++;
        
        if(i % 10 == 0)
            j=0;
        
        j++;
        
        particles[i].setPos(glm::vec3(10 + j*6, k*6, 0.5));
        
    }
    
    for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
        cells[j].CreateCell(j);
    }
}

void calculateDensityAndPressure(){

    for(int i = 0; i < NUM_PARTICLES; i++){
        
        float density_sum = 0;
       
        int cellIndex = particles[i].getCellIndex();
        int limit = 0;

        vector<int> current_cells = cells[cellIndex].getNeighbours();
    
        for(int j = 0; j < current_cells.size(); j++){
            
            // Loop through all neighbouring particles
            for(int k = 0; k < cells[current_cells.at(j)].getParticles().size(); k++){
                
                if (++limit > 80)
                    break;
                
                Particle *n = cells[current_cells.at(j)].getParticles().at(k);
                
                glm::vec3 diffvec = particles[i].getPos() - n->getPos();
                
                diffvec/=512.f;
                
                float abs_diffvec = glm::length(diffvec);

                //std::cout << "ABS DIFFVEC: " << abs_diffvec << std::endl;
                
                if(abs_diffvec < h){
               		
                    density_sum += PARTICLE_MASS * (315 / (64*M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)),3.0);
                    
                }
                
            }
            
        }
        
        //std::cout << "DENSITY SUM: " << density_sum << std::endl;
        if(density_sum != 0){
            particles[i].setDensity(density_sum);
            particles[i].setPressure(STIFFNESS*(density_sum - 998.f));
        }
        else{
            particles[i].setDensity(998.f);
            particles[i].setPressure(STIFFNESS*(998.f - 998.f));
        }
        
    }

}

void calculateForces(){
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        glm::vec3 gravity = glm::vec3(0, -9.82f*particles[i].getDensity(), 0);
        glm::vec3 pressure = glm::vec3(0);
        glm::vec3 viscousity = glm::vec3(0);
        
        int cellIndex = particles[i].getCellIndex();
        int limit = 0;
        
        vector<int> current_cells = cells[cellIndex].getNeighbours();
        
        //Loop through all cells
        for(int j = 0; j < current_cells.size(); j++){
            
            // Loop through all neighbouring particles
            for(int k = 0; k < cells[current_cells.at(j)].getParticles().size(); k++){
                
                Particle *n = cells[current_cells.at(j)].getParticles().at(k);
                
                //std::cout << "PARTICLE POS : " << particles[i].getPos().y << std::endl;
                //std::cout << "NEIGHBOUR POS : " << n.getPos().y << std::endl;
                
                if(n->getPos() == particles[i].getPos()){
                   // std::cout << "LDALDLASD";
                    continue;
                }
                
                if (++limit > 80)
                    break;
                
                glm::vec3 diffvec = particles[i].getPos() - n->getPos();
                diffvec/=512.f;
                
                float abs_diffvec = glm::length(diffvec);

                if(abs_diffvec < h){
                    
                    float W_const_pressure = 45/(M_PI * glm::pow(h, 6.0)) * glm::pow(h - abs_diffvec, 3.0) / abs_diffvec;
                    
                    glm::vec3 W_pressure_gradient = glm::vec3(W_const_pressure * diffvec.x, W_const_pressure * diffvec.y, 0);
                    
                    float visc_gradient = (45/(M_PI * glm::pow(h, 6.0)))*(h - abs_diffvec);
                    
                    pressure +=  PARTICLE_MASS * ((particles[i].getPressure() + n->getPressure()) / (2 * n->getDensity())) * W_pressure_gradient;
                    
                    viscousity += VISCOUSITY * PARTICLE_MASS * ((n->getVelocity() - particles[i].getVelocity()) / (n->getDensity())) * visc_gradient;
                    
                    
                }
                
            }
            
        }
        
        particles[i].setViscousityForce(viscousity);
        particles[i].setPressureForce(pressure);
        particles[i].setGravityForce(gravity);
        
    }

}

void calculateAcceleration(){
    
    // Clear all particles in cells
    for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
        
        cells[j].clearParticles();
        
    }
    
    // Push every particle into corresponding cell
    for(int i = 0; i < NUM_PARTICLES; i++) {
        
        cells[particles[i].getCellIndex()].addParticle(particles[i]);
        
    }
    
    calculateDensityAndPressure();
    calculateForces();

}

void display()
{
    handleFps();
    
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
    
}


int main(int argc, char *argv[])
{
    
    CreateParticles();
    
    glfwInit();
    
    window = glfwCreateWindow(512, 512, "OpenGL", nullptr, nullptr); // Windowed
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
        
        calculateAcceleration();
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
