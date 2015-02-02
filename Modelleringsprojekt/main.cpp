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
    
    for(int i = 0; i <= NUM_PARTICLES; i++)
    {
		if (i == 0) {
			std::cout << "(" << particles[i].getCell().x << ", " << particles[i].getCell().y << ")" << std::endl;
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



