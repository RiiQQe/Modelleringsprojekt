#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include "Particle.h"
#include "Box.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <math.h>
#include <vector>

const int NUM_PARTICLES = 500;
const float VISCOUSITY = 1.1f;
const float PARTICLE_MASS = .02f;
const double h = 1.5f;
const float STIFFNESS = 1.5f;

Particle particles[NUM_PARTICLES];

Box box = Box();

// Create all the particles
void CreateParticles()
{
    
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        particles[i].CreateParticle();
		
        if (i == 0)
			particles[i].setSpecial();
    
        glm::vec2 newPos = glm::vec2((float) 1.3 * (i % 40) + 100, (float) 0.27 * (i * 3 / 40) + 100);
        particles[i].setPos(newPos);
        
    }
    
}

void calculateDensity(){
    
    // Col
    for(int x = 0; x < 16; x++){
        // Row
        for(int y = 0; y < 16; y++){
            
            for(int i = 0; i < NUM_PARTICLES; i++){
                
                Particle p = particles[i];
                
                float density_sum = 0;
                
                //Check only particles that is inside that current cell
                if(p.getCell() != glm::vec2(x,y))
                    continue;
                
                //std:: cout << "PARTICLE CELL = " << p.getCell().x << std::endl;;
                
                // Iterate through neighboring cells
                for(int offsetX = -1; offsetX <= 1; offsetX++){
                    
                    //If out of bounds
                    if (x+offsetX < 0) continue;
                    if (x+offsetX >= 16) break;
                    
                    for(int offsetY = -1; offsetY <= 1; offsetY++){
                        
                        if (y+offsetY < 0) continue;
                        if (y+offsetY >= 16) break;
                        
                        //Check all possible neighbours
                        for(int j = 0; j < NUM_PARTICLES; j++){
                            
                            Particle neighbour = particles[j];
                            //std::cout << "NEIGHTBOUR : " << neighbour.getCell().x << std::endl;
                            if(neighbour.getCell() == glm::vec2(x+offsetX, y+offsetY) && i != j){
                        
                                glm::vec2 diffvec = p.getPos() - neighbour.getPos();
                                
                                //std::cout << "(" << neighbour.getPos().x << ", " << neighbour.getPos().y << ")" << std::endl;
                                //std::cout << diffvec.x << std::endl;
                                //std::cout << glm::length(diffvec) << std::endl;
                                
                                //if smaller than h...
                                if(glm::length(diffvec) < h){
                                	//density_sum += 1.505 * glm::pow(10, 10) * (glm::pow(h, 2.0) - glm::pow(glm::length(diffvec), 2.f));
                                    
                                    float diffvec_abs = glm::length(diffvec);
                                    
                                    density_sum += PARTICLE_MASS * (315 / (64*M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(diffvec_abs, 2.f)),3.0);
                                
                                    
                                }
                                
            
                            }
                            
                        }
                        
                    }
                    
                }
                
				//std::cout << "DENSITY SUM: " << density_sum << std::endl;
                particles[i].setDensity(density_sum);
                // End particle loop
                
            }
            
        }
        
    }

}

void calculatePressure(){

    for(int i = 0; i < NUM_PARTICLES; i++){
        
        //std::cout << "PRESSURE " << particles[i].getDensity();
        
        particles[i].setPressure(STIFFNESS*(particles[i].getDensity() - 1000.f));
        
    }

}

void calculatePressureAndViscousityGradients(){

    // Col
    for(int x = 0; x < 16; x++){
        // Row
        for(int y = 0; y < 16; y++){
            
            for(int i = 0; i < NUM_PARTICLES; i++){
                
                glm::vec2 pressure = glm::vec2(0);
                glm::vec2 viscousity = glm::vec2(0);
                glm::vec2 gravity = glm::vec2(0, -9.82f);
                
                //std::cout << "LOLOL" << particles[i].getPressure() << std::endl;
                
                Particle p = particles[i];

                //Check only particles that is inside the current cell
                if(p.getCell() != glm::vec2(x,y))
                    continue;
                
                //std:: cout << "PARTICLE CELL = " << p.getCell().x << std::endl;;
                
                // Iterate through neighboring cells
                for(int offsetX = -1; offsetX <= 1; offsetX++){
                    
                    //If out of bounds
                    if (x+offsetX < 0) continue;
                    if (x+offsetX >= 16) break;
                    
                    for(int offsetY = -1; offsetY <= 1; offsetY++){
                        
                        if (y+offsetY < 0) continue;
                        if (y+offsetY >= 16) break;
                        
                        //Check all possible neighbours
                        for(int j = 0; j < NUM_PARTICLES; j++){
                            
                            Particle neighbour = particles[j];
                            //std::cout << "NEIGHTBOUR : " << neighbour.getCell().x << std::endl;
                            if(neighbour.getCell() == glm::vec2(x+offsetX, y+offsetY) && i != j){
                                
                                glm::vec2 diffvec = p.getPos() - neighbour.getPos();
                                
                                float abs_diffvec = glm::length(diffvec);
                                
                                // std::cout << "diffvec x : " << abs_diffvec << std::endl;
                                
                                // if smaller than h...
                                if(abs_diffvec < h){
                                    
                                    float W_const = 45/(M_PI * glm::pow(abs_diffvec, 6.f)) * glm::pow(h - abs_diffvec, 3.0) / abs_diffvec;
                                    
                                    glm::vec2 W_pressure_gradient = glm::vec2(W_const * diffvec.x, W_const * diffvec.y);
                                    
                                    float visc_gradient = W_const * (h - abs_diffvec);
                                    
                                    pressure +=  - PARTICLE_MASS * ((p.getPressure() + neighbour.getPressure()) / 2 * neighbour.getPressure()) * W_pressure_gradient;
                                    
                                    viscousity += VISCOUSITY * PARTICLE_MASS * (neighbour.getVelocity() - p.getVelocity()) * visc_gradient;
                                    
                                    //std::cout << "visc" << viscousity.x;
                                    //pressure = -PARTICLE_MASS*((p.getPressure() + neighbour.getPressure())/2*neighbour.getPressure());
                                   
                                
                                }
                                
                            }

                        }
                        
                    }
                    
                }
                
                particles[i].setViscousityForce(viscousity);
                particles[i].setPressureForce(pressure);
                particles[i].setGravityForce(gravity);
                // End particle loop
            }
            
        }
        
    }
    
}


void display()
{
    //Set grid indices to all particles
    for(int i = 0; i < NUM_PARTICLES; i++)
        particles[i].setCurrentGridCell();
    
    //Calculate new density and pressure
    calculateDensity();
    calculatePressure();
    
    //Update forces
    calculatePressureAndViscousityGradients();
    
    glm::vec2 accelerationVec;
    
    for(int i = 0; i < NUM_PARTICLES; i++){
    	
        accelerationVec = particles[i].getPressureForce() + particles[i].getViscousityForce() + particles[i].getGravityForce();
        
        //std::cout << "X VELOCITY : " << particles[i].getGravityForce().y << std::endl;
        //<< "Y VELOCITY : " << particles[i].getGravityForce().y << std::endl;
        
        particles[i].setVelocity(particles[i].getVelocity() + .01f*accelerationVec);
        particles[i].setPos(particles[i].getPos() + 1.f*particles[i].getVelocity());
        
    }
    
    for(int i = 0; i <= NUM_PARTICLES; i++)
    {
        //particles[i].EvolveParticle();
    }

    for(int i = 0; i <= NUM_PARTICLES; i++)
    {
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



