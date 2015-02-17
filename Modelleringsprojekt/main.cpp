#define GLEW_STATIC
#define _USE_MATH_DEFINES
#include <GL/glew.h>

#include <iostream>
#include "Particle.h"
#include "Box.h"
#include "Cell.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <sstream>

const int NUM_PARTICLES = 500;
const int GRID_WIDTH = 32;
const int GRID_HEIGHT = 32;
const int KERNEL_LIMIT = 40;

const float VISCOUSITY = 500*2.5f;
const float PARTICLE_MASS = 500*.14f;
const double h = 16.f;
const float STIFFNESS = 500*3.f;
const float GRAVITY_CONST = 50000*9.82f;

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
void init()
{
    for(int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].CreateParticle();
        
    }
    
    // Set positions
    int k = 0, j = 0;
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        //Y-led
        if(i % 40 == 0)
            k++;
        
        //X
        if(i % 40 == 0)
            j=0;
        
        j++;
        
        particles[i].setPos(glm::vec3(20+j*16/2 - 8, 19*16 + k*16/2 - 8, 0.5));
        
    }
    
    for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
        cells[j].CreateCell(j);
    }
}

//Trying to reduce calculation by removing random number of neighbours up to the limit of the kernel
void reduceNeighbours(vector<Particle*>& theNeighbours){
    
    //Temp
    vector<Particle*> mapped_neighbours;
    
    for(int i = 0; i < KERNEL_LIMIT; i++){
        int random = rand() % theNeighbours.size();
        mapped_neighbours.push_back(theNeighbours.at(random));
        theNeighbours.erase(theNeighbours.begin() + random);
    }
    
    theNeighbours = mapped_neighbours;
    //std::cout << "tokmånga partiklar";
}

void calculateDensityAndPressure(){

    for(int i = 0; i < NUM_PARTICLES; i++){
        
        float density_sum = 0;
        int cellIndex = particles[i].getCellIndex();
        
        vector<int> current_cells = cells[cellIndex].getNeighbours();
    
        for(int j = 0; j < current_cells.size(); j++){
			
            // Loop through all neighbouring particles
            vector<Particle*> neighbours = cells[current_cells.at(j)].getParticles();
            
            // Too many neighbours...
            if(cells[current_cells.at(j)].getParticles().size() > KERNEL_LIMIT)
                reduceNeighbours(neighbours);
            
            for(int k = 0; k < neighbours.size(); k++){
                
                vector<Particle*> neighbours = cells[current_cells.at(j)].getParticles();
                
                Particle *n = neighbours.at(k);
            
                glm::vec3 diffvec = particles[i].getPos() - n->getPos();
            
                float abs_diffvec = glm::length(diffvec);
            
                //std::cout << "ABS DIFFVEC " <<  abs_diffvec << std::endl;
            
                if(abs_diffvec < h){
            
                    density_sum += PARTICLE_MASS * (315 / (64*M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)),3.0);
                    //cout << "Density: " << PARTICLE_MASS * (315 / (64 * M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)), 3.0) << endl;
                }
                
            }
            
        }
        
        particles[i].setDensity(density_sum);
        particles[i].setPressure(STIFFNESS*(density_sum - 998.f));
        
    }

}

void calculateForces(){
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        glm::vec3 gravity = glm::vec3(0, -GRAVITY_CONST*particles[i].getDensity(), 0);
        glm::vec3 pressure = glm::vec3(0);
        glm::vec3 viscousity = glm::vec3(0);
        
        int cellIndex = particles[i].getCellIndex();
        //int limit = 0;
		//bool limitBool = false;
		//float prevVisc = 0.0f, prevPress = 0.0f;
        
        vector<int> current_cells = cells[cellIndex].getNeighbours();
        
        //Loop through all cells
        for(int j = 0; j < current_cells.size(); j++){
            
            // Loop through all neighbouring particles
            vector<Particle*> neighbours = cells[current_cells.at(j)].getParticles();
            
            // Too many neighbours
            if(neighbours.size() > KERNEL_LIMIT)
                reduceNeighbours(neighbours);
            
            for(int k = 0; k < neighbours.size(); k++){
                
                Particle *n = neighbours.at(k);
                
                // Skip comparison of the same particle
                if(n->getPos() == particles[i].getPos()){
                    continue;
                }
                
                glm::vec3 diffvec = particles[i].getPos() - n->getPos();

                float abs_diffvec = glm::length(diffvec);

                if(abs_diffvec < h){
                    
                    float W_const_pressure = 45/(M_PI * glm::pow(h, 6.0)) * glm::pow(h - abs_diffvec, 3.0) / abs_diffvec;
                    
                    glm::vec3 W_pressure_gradient = glm::vec3(W_const_pressure * diffvec.x, W_const_pressure * diffvec.y, 0);
                    
                    float visc_gradient = (45/(M_PI * glm::pow(h, 6.0)))*(h - abs_diffvec);
                    
                    pressure +=  -PARTICLE_MASS * ((particles[i].getPressure() + n->getPressure()) / (2 * n->getDensity())) * W_pressure_gradient;
                    
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
    
    init();
    
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
