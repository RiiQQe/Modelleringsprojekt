#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include "Particle.h"
#include "Box.h"
#include "Cell.h"

#include <thread>

const int NUM_PARTICLES = 9;
const int GRID_WIDTH = 16;
const int GRID_HEIGHT = 16;

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[GRID_WIDTH * GRID_HEIGHT];
const int TEMPSIZE = 32;
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
        //particles[i].DrawObjects();
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
            
            squares[k] += (particles[i].getRadius()*particles[i].getRadius()) / ((x - particles[i].getPos().x) * (x - particles[i].getPos().x) + (y - particles[i].getPos().y) * (y - particles[i].getPos().y));
        }
    }
    
    // draw stuff
    for (int i = 0; i < TEMPSIZE - 1; i++) {
        for (int j = 0; j < TEMPSIZE - 1; j++) {
            int bitwiseSum = 0;
            
            //  ***** USED FOR MARCHING SQUARES *****
            //
            //       CASE EXAMPLE:  bitwiseSum = 4
            //    (only point d is within a metaball):
            //
            //
            //             |            |
            //             | a          | b
            //       ------+------------+------
            //             |            |
            //             |            *  (Qx, Qy)
            //             |        _ ^ |
            //             | c   _ ^    | d
            //       ------+-- * -------+------
            //             | (Px, Py)   |
            //             |            |
            //
            //
            
            
            // calculate bitwise sum
            // sum increments by 2^n for each corner, going clockwise, starting in top right
            
            float a, b, c, d;
            a = squares[i     + (j * TEMPSIZE + TEMPSIZE)]; // upper left
            b = squares[i + 1 + (j * TEMPSIZE + TEMPSIZE)]; // upper right
            c = squares[i     + (j * TEMPSIZE)]; // lower left
            d = squares[i + 1 + (j * TEMPSIZE)]; // lower right
            
            if (a > 1.f)   { bitwiseSum += 1; } // upper left corner
            if (b > 1.f)   { bitwiseSum += 2; } // upper right corner
            if (d > 1.f)   { bitwiseSum += 4; } // lower right corner
            if (c > 1.f)   { bitwiseSum += 8; } // lower left corner
            
            // Depending on the bitwiseSum, draw triangles.
            
            if (bitwiseSum >= 1.f) { // bitwiseSum cannot assume negative values and 0 is an empty square.
                
                // These are used for interpolation process, increasing the possible angles from n * PI/4 to basically infinity.
                float qx, qy, px, py;
                
                // all points, position
                float ax, ay, bx, by, cx, cy, dx, dy;
                
                // topleft
                ax =  i * kSize + kSize / 2;
                ay = (j + 1) * kSize + kSize / 2;
                
                // topright
                bx = (i + 1) * kSize + kSize / 2;
                by = (j + 1) * kSize + kSize / 2;
                
                // botleft
                cx =  i * kSize + kSize / 2;
                cy =  j * kSize + kSize / 2;
                
                // botright
                dx = (i + 1) * kSize + kSize / 2;
                dy =  j * kSize + kSize / 2;
                
                glBegin(GL_TRIANGLE_STRIP);
                
                glColor3f(1.f, 1.f, 1.f);
                
                switch (bitwiseSum) {
                    case 1:
                        // Points of intersection of square bounadires from metaballs
                        // left intersection
                        qx = ax;
                        qy = ay + (cy - ay) * ((1 - a) / (c - a));
                        
                        // upper intersection
                        px = bx + (ax - bx) * ((1 - b) / (a - b));
                        py = ay;
                        
                        // top left triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(ax,             ay,  0);  // top         left
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);  // top         left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);  // bot+half    left
                        break;
                    case 2:
                        // Points of intersection of square boundaries from metaballs
                        // right intersection
                        qx = bx;
                        qy = by + (dy - by) * ((1 - b) / (d - b));
                        
                        // upper intersection
                        px = bx + (ax - bx) * ((1 - b) / (a - b));
                        py = ay;
                        
                        // top right triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(bx,             by,  0);  // top          right
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);  // top          left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);  // bot+half     right
                        
                        break;
                    case 3:
                        // Points of intersection of square boundaries from metaballs
                        // right intersection
                        qx = bx;
                        qy = by + (dy - by) * ((1 - b) / (d - b));
                        
                        // left intersection
                        px = ax;
                        py = ay + (cy - ay) * ((1 - a) / (c - a));
                        
                        
                        // top rectangle
                        glTexCoord2f(0.0,1.0); glVertex3f(bx,             by,  0);  // top      right
                        glTexCoord2f(0.0,1.0); glVertex3f(ax,             ay,  0);  // top      left
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);  // bot+half right
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);  // bot+half left

                        break;
                    case 4:
                        // Points of intersection of square boundaries from metaballs
                        // right intersection
                        qx = bx;
                        qy = by + (dy - by) * ((1 - b) / (d - b));
                        
                        // lower intersection
                        px = dx + (cx - dx) * ((1 - d) / (c - d));
                        py = cy;
                        
                        // bot right triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(dx,             dy,  0);   // bot      right
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);   // bot      left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);   // bot+half right
                        
                        break;
                    case 5:
                        std::cout << "SPECIAL CASE 5" << endl;
                        break;
                    case 6:
                        // Points of intersection of square boundaries from metaballs
                        // upper intersection
                        qx = bx + (ax - bx) * ((1 - b) / (a - b));
                        qy = ay;
                        
                        // lower intersection
                        px = dx + (cx - dx) * ((1 - d) / (c - d));
                        py = cy;
                        
                        // right rectangle
                        glTexCoord2f(0.0,1.0); glVertex3f(bx,             by,  0);    // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);    // top    left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(dx,             dy,  0);    // bottom right
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);    // bottom left+half
                        
                        break;
                    case 7:
                        // Points of intersection of square boundaries from metaballs
                        // left intersection
                        qx = ax;
                        qy = ay + (cy - ay) * ((1 - a) / (c - a));
                        
                        // lower intersection
                        px = dx + (cx - dx) * ((1 - d) / (c - d));
                        py = cy;
                        
                        // missing bot left triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(ax,             ay,  0);    // top        left
                        glTexCoord2f(0.0,1.0); glVertex3f(bx,             by,  0);    // top        right
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);    // bot+half   left
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);    // bot        left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(bx,             by,  0);    // top        right
                        glTexCoord2f(0.0,1.0); glVertex3f(dx,             dy,  0);    // bot        right
                        break;
                    case 8:
                        // Points of intersection of square boundaries from metaballs
                        // left intersection
                        qx = ax;
                        qy = ay + (cy - ay) * ((1 - a) / (c - a));
                        
                        // lower intersection
                        px = dx + (cx - dx) * ((1 - d) / (c - d));
                        py = cy;
                        
                        // bot left triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(cx,             cy,   0);   // bottom      left
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,   0);   // bottom+half left
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,   0);   // bottom      left+half
                        break;
                    case 9:
                        // Points of intersection of square boundaries from metaballs
                        // upper intersection
                        qx = bx + (ax - bx) * ((1 - b) / (a - b));
                        qy = ay;
                        
                        // lower intersection
                        px = dx + (cx - dx) * ((1 - d) / (c - d));
                        py = cy;
                        
                        // left rectangle
                        glTexCoord2f(0.0,1.0); glVertex3f(ax,             ay,   0);   // top    left
                        glTexCoord2f(0.0,1.0); glVertex3f(cx,             cy,   0);   // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,   0);   // top    left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,   0);   // bottom left+half
                        
                        break;
                    case 10:
                        std::cout << "SPECIAL CASE 10" << endl;
                        break;
                    case 11:
                        // Points of intersection of square boundaries from metaballs
                        // right intersection
                        qx = bx;
                        qy = by + (dy - by) * ((1 - b) / (d - b));
                        
                        // lower intersection
                        px = dx + (cx - dx) * ((1 - d) / (c - d));
                        py = cy;
                        
                        // missing bot right triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(ax,             ay,  0);    // top    left
                        glTexCoord2f(0.0,1.0); glVertex3f(bx,             by,  0);    // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f(cx,             cy,  0);    // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);    // bot+half right
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);    // bottom left+half
                        break;
                    case 12:
                        // Points of intersection of square boundaries from metaballs
                        qx = bx;
                        qy = by + (dy - by) * ((1 - b) / (d - b));
                        
                        // left intersection
                        px = ax;
                        py = ay + (cy - ay) * ((1 - a) / (c - a));
                        
                        
                        // bot rectangle
                        glTexCoord2f(0.0,1.0); glVertex3f(dx,             dy,  0);    // bottom right
                        glTexCoord2f(0.0,1.0); glVertex3f(cx,             cy,  0);    // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);    // bot+half right
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);    // bot+half left

                        break;
                    case 13:
                        // Points of intersection of square boundaries from metaballs
                        // right intersection
                        qx = bx;
                        qy = by + (dy - by) * ((1 - b) / (d - b));
                        
                        // upper intersection
                        px = bx + (ax - bx) * ((1 - b) / (a - b));
                        py = ay;
                        
                        // missing top right triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(ax,             ay,  0);    // top    left
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);    // top    left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(cx,             cy,  0);    // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);    // bot+half right
                        glTexCoord2f(0.0,1.0); glVertex3f(dx,             dy,  0);    // bottom right
                        
                        break;
                    case 14:
                        
                        
                        // Points of intersection of square bounadires from metaballs
                        // left intersection
                        qx = ax;
                        qy = ay + (cy - ay) * ((1 - a) / (c - a));
                        
                        // upper intersection
                        px = bx + (ax - bx) * ((1 - b) / (a - b));
                        py = ay;
                        
                        // missing top left triangle
                        glTexCoord2f(0.0,1.0); glVertex3f(qx,             qy,  0);    // bot+half left
                        glTexCoord2f(0.0,1.0); glVertex3f(px,             py,  0);    // top    left+half
                        glTexCoord2f(0.0,1.0); glVertex3f(cx,             cy,  0);    // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f(bx,             by,  0);    // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f(dx,             dy,  0);    // bottom right
                        
                        break;
                    case 15:
                        // draw full square
                        glTexCoord2f(0.0,1.0); glVertex3f(ax,             ay,  0);    // top    left
                        glTexCoord2f(0.0,1.0); glVertex3f(bx,             by,  0);    // top    right
                        glTexCoord2f(0.0,1.0); glVertex3f(cx,             cy,  0);    // bottom left
                        glTexCoord2f(0.0,1.0); glVertex3f(dx,             dy,  0);    // bottom right
                        
                        break;
                    default:
                        cout << "THIS IS FUCKED" << endl;
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
