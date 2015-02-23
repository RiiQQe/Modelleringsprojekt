#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include "Particle.h"
#include "Box.h"
#include "Cell.h"

#include <thread>

const int NUM_PARTICLES = 3;
const int GRID_WIDTH = 16;
const int GRID_HEIGHT = 16;

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[GRID_WIDTH * GRID_HEIGHT];
const int TEMPSIZE = 256;
float squares[TEMPSIZE * TEMPSIZE]; // hard coded values for now, with marching squares
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
        particles[i].DrawObjects();
    }
    
    // Beta balls begins
    
    // Marching cubes algorithm -
    // bitwise tiling (Jonsson, 2015)
    // Using ..
    // f(x,y) = sum(i = 1 -> n) ri^2 / ((x - xi)^2 + (y - yi)^2)
    //
    
    // Needed for scaling
    int kSize = 512 / TEMPSIZE;
    
    // Iterate through all cells
    for (int k = 0; k < TEMPSIZE * TEMPSIZE; k++) {
        int x = kSize * (k % TEMPSIZE);
        int y = (k / (TEMPSIZE)) * kSize;
        
        
        // for each cell, check
        for(int i = 0; i < NUM_PARTICLES; i++) {
            // calculate height map, sum
            
            squares[k] += (20*20) / ((x - particles[i].getPos().x) * (x - particles[i].getPos().x) + (y - particles[i].getPos().y) * (y - particles[i].getPos().y));
        }
    }
    
    // draw stuff
    for (int i = 0; i < TEMPSIZE - 1; i++) {
        for (int j = 0; j < TEMPSIZE - 1; j++) {
            int bitwiseSum = 0;
            
            // calculate bitwise sum
            //
            
            if (squares[i     + (j * TEMPSIZE + TEMPSIZE)] > 1.f)       { bitwiseSum += 1; } // upper left corner
            if (squares[i + 1 + (j * TEMPSIZE + TEMPSIZE)] > 1.f)   { bitwiseSum += 2; } // upper right corner
            if (squares[i + 1 + (j * TEMPSIZE)] > 1.f)                  { bitwiseSum += 4; } // upper left corner
            if (squares[i     + (j * TEMPSIZE)] > 1.f)              { bitwiseSum += 8; } // lower right corner
            
            // Depending on the bitwiseSum, draw triangles.
            
            if (bitwiseSum >= 1.f) {
                //if (bitwiseSum >= 14) cout << bitwiseSum << endl;
            
                glBegin(GL_TRIANGLE_STRIP);
                glColor3f(1.f, 1.f, 1.f);

                switch (bitwiseSum) {
                    case 1:
                        // top left triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,            (j + 1) * kSize + kSize / 2,  0);           // top         left
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2 + kSize / 2,(j + 1) * kSize + kSize / 2,  0);           // top         left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,             j * kSize + kSize / 2 + kSize / 2,    0);  // bot+half    left
                        break;
                    case 2:
                        // top right triangle
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     (j + 1) * kSize + kSize / 2,  0);             // top          right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2 + kSize / 2,           (j + 1) * kSize + kSize / 2,  0); // top          left+half
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2 + kSize / 2,        0); // bot+half     right
                        break;
                    case 3:
                        // top rectangle
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     (j + 1) * kSize + kSize / 2,  0);           // top      right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           (j + 1) * kSize + kSize / 2,  0);           // top      left
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + 2 * (kSize / 2),        0);     // bot+half right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + 2 * (kSize / 2),        0);     // bot+half left

                        break;
                    case 4:
                        // bot right triangle
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2,        0);             // bot         right
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2 + kSize / 2,        0); // bot+half    right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2 + kSize / 2,           j * kSize + kSize / 2,        0); // bot left+half
                        break;
                    case 5:
                        std::cout << "SPECIAL CASE" << endl;
                        break;
                    case 6:
                        // right rectangle
                        
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     (j + 1) * kSize + kSize / 2,  0);           // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + 2 * (kSize / 2),           (j + 1) * kSize + kSize / 2,  0);     // top    left+half
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2,        0);           // bottom right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + 2 * (kSize / 2),           j * kSize + kSize / 2,        0);     // bottom left+half
                        
                        break;
                    case 7:
                        // missing bot left triangle
                        
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           (j + 1) * kSize + kSize / 2,  0);             // top          left
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     (j + 1) * kSize + kSize / 2,  0);             // top          right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,            j * kSize + 2 * (kSize / 2), 0);             // bottom+half left
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + 2 * (kSize / 2),           j * kSize + kSize / 2,        0);       // bottom left+half
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     (j + 1) * kSize + kSize / 2,  0);             // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2,        0);             // bottom       right
                        break;
                    case 8:
                        // bot left triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,              j * kSize + kSize / 2,        0);        // bottom      left
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,              j * kSize + kSize / 2 + kSize / 2, 0);   // bottom+half left
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2 + kSize / 2,  j * kSize + kSize / 2,        0);        // bottom      left+half
                        break;
                    case 9:
                        // left rectangle
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           (j + 1) * kSize + kSize / 2,  0);           // top    left
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + kSize / 2,        0);           // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + 2 * (kSize / 2),           (j + 1) * kSize + kSize / 2,  0);     // top    left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + 2 * (kSize / 2),           j * kSize + kSize / 2,        0);     // bottom left+half
                        
                        break;
                    case 10:
                        std::cout << "SPECIAL CASE" << endl;
                        break;
                    case 11:
                        // missing bot right triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           (j + 1) * kSize + kSize / 2,  0);     // top    left
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     (j + 1) * kSize + kSize / 2,  0);     // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + kSize / 2,        0);     // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2 + kSize / 2,        0); // bot+half
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + 2 * (kSize / 2),           j * kSize + kSize / 2,        0);     // bottom left+half
                        
                        break;
                    case 12:
                        // bot rectangle
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2,        0);           // bottom right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + kSize / 2,        0);           // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + 2 * (kSize / 2),        0);     // bot+half right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + 2 * (kSize / 2),        0);     // bot+half left

                        break;
                    case 13:
                        // missing top right triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           (j + 1) * kSize + kSize / 2,  0);     // top    left
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + 2 * (kSize / 2),           (j + 1) * kSize + kSize / 2,  0);     // top    left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + kSize / 2,        0);            // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + 2 * (kSize / 2),        0);     // bot+half right
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2,        0);     // bottom right
                        
                        
                        break;
                    case 14:
                        // missing top left triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + 2 * (kSize / 2),        0);     // bot+half left
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + 2 * (kSize / 2),           (j + 1) * kSize + kSize / 2,  0);     // top    left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + kSize / 2,        0);     // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     (j + 1) * kSize + kSize / 2,  0);     // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2,        0);     // bottom right
                        
                        break;
                    case 15:
                        // draw full square
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     (j + 1) * kSize + kSize / 2,  0);     // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           (j + 1) * kSize + kSize / 2,  0);     // top    left
                        glTexCoord2f(0.0,1.0); glVertex3f((i + 1) * kSize + kSize / 2,     j * kSize + kSize / 2,        0);     // bottom right
                        glTexCoord2f(0.0,1.0); glVertex3f(i * kSize + kSize / 2,           j * kSize + kSize / 2,        0);     // bottom left
                        
                        break;
                    default:
                        break;
                }
                
                glEnd();
            }
        }
    }

    // clear squares
    for (int i = 0; i < TEMPSIZE * TEMPSIZE; ++i) {
        squares[i] = 0.f;
    }
    
    // Beta balls ends
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
        
        
        //box.DrawBox();
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
    
}
