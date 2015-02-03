#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include "Particle.h"
#include "Box.h"
#include <GLFW/glfw3.h>
#include <thread>

const int NUM_PARTICLES = 500;

Particle particles[NUM_PARTICLES];
Box box = Box();

// Create all the particles
void CreateParticles()
{
    
    for(int i = 0; i <= NUM_PARTICLES; i++)
    {
        particles[i].CreateParticle();
		if (i == 0) 
			particles[i].setSpecial();
    }
    
}

void display()
{
    
    
    //Set grid indices to all particles
    for(int i = 0; i < NUM_PARTICLES; i++)
        particles[i].setCurrentGridCell();
    
    // Col
    for(int x = 0; x < 16; x++){
        // Row
        for(int y = 0; y < 16; y++){
            
            for(int i = 0; i < NUM_PARTICLES; i++){
            	
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
                        	if(neighbour.getCell() == glm::vec2(x+offsetX, y+offsetY)){
                            
                            	glm::vec2 diffposition = p.getPos() - neighbour.getPos();
                            	//std::cout << "(" << diffposition.x << ", " << diffposition.y << ")" << std::endl;

                            
                        	}
                            
                    	}
                 
                    }
                 
                }
            
            }
            
        }
        
    }
    
    
    
    

    
    for(int i = 0; i <= NUM_PARTICLES; i++)
    {
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
		}

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



