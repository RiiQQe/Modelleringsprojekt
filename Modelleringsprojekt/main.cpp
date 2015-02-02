#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include "Particle.h"
#include "Box.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <vector>

const int NR_OF_PARTICLES = 1000;

Particle particles[NR_OF_PARTICLES];
Box box = Box();

// Create all the particles
void CreateParticles()
{
    
    for(int i = 0; i <= NR_OF_PARTICLES; i++)
    {
        particles[i].CreateParticle();
    }
    
}

void display()
{
 
    
    for(int i = 0; i <= NR_OF_PARTICLES; i++)
    {
        particles[i].DrawObjects();
        
    }
    
}

void reshape_window(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void calculatePressure(){
    
    for(int i = 0; i < NR_OF_PARTICLES; i++){
        
        particles[i].setPressure(0);
        
        // Row
        for(int x = 0; x < 16; x++){
            // Col
            for(int y = 0; y < 16; y++){
                
                // Iterate through neighboring cells
                for(int offsetX = -1; offsetX <= 1; offsetX++){
                    
                    //Check if out of bounds
                    if (x+offsetX < 0) continue;
                    if (x+offsetX >= 16) break;
                    
                    for(int offsetY = -1; offsetY <= 1; offsetY++){
                        
                        if (y+offsetY < 0) continue;
                        if (y+offsetY >= 16) break;
                        
                        std::vector<Particle> neighbors;
                        
                    
                    
                    }
                
                }
        	
            }
        
        }

    }

}

void idle()
{
    
    calculatePressure();
    
    for (int i = 0; i < NR_OF_PARTICLES; i++)
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
    
    double oldTimeSinceStart = 0;
    
    
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




/*
 
 glutInitWindowSize(512,512);
 glutInitWindowPosition(100,100);
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
 glutCreateWindow("OpenGL1");
 
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluOrtho2D(0.0, 512.0, 0.0, 512.0);
 
 glutDisplayFunc(display);
 glutReshapeFunc(reshape);
 glutIdleFunc(idle);
 glutSpecialFunc(special);
 glutMainLoop();
 
 return EXIT_SUCCESS;*/


