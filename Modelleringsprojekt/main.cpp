#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include <GLUT/glut.h>
#include "Particle.h"
#include "Box.h"
#include <GLFW/glfw3.h>
#include <thread>



// Horizontal position of the box.
Particle particles[500];
Box myBox = Box();

// Create the particles
void CreateParticles()
{
    
    for(int i = 0; i <= 500; i++)
    {
        particles[i].CreateParticle();
    }
    
}

/*
 Allows for user input to control the particles.
 */
static void special (int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_DOWN:
            for(int i = 0; i <= 500; i++)
            {
                particles[i].SetYPos((float)particles[i].GetYPos()*1.01f);
            }
            break;

            //case GLUT_KEY_UP:      particles[i].SetXPos((float)particles[i].GetXPos()+0.01f);        break;
            //case GLUT_KEY_LEFT:      zpos -= 5;        break;
            //case GLUT_KEY_RIGHT:      boxX += 5;        break;
            //case GLUT_KEY_F1:	rotAngle += 5;		break;
            
        default: break;
    }
    
}

//////////////////////////////////////////////////////////////////////////////////////////

void display()
{
    
    for(int i = 0; i <= 500; i++)
    {
        particles[i].DrawObjects();
        
    }
    
   // myBox.DrawBox();
    
}

void reshape_window(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void idle()
{
    for (int i = 0; i < 500; i++)
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
        
        //Swap front and back buffers
        glfwSetWindowSizeCallback(window, reshape_window);
        glfwSwapBuffers(window);
        
        //Poll for and process events
        glfwPollEvents();
        
    }
    
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return EXIT_SUCCESS;
    
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
    
}


