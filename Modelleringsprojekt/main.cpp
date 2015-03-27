#define GLEW_STATIC
#define _USE_MATH_DEFINES
#include <GL/glew.h>

#include <iostream>
#include "Particle.h"
#include "Box.h"
#include "Cell.h"
#include <GLFW/glfw3.h>

#ifdef __APPLE__
//#include "TargetConditionals.h"
#include <GLUT/glut.h>
//#include <OpenGL/OpenGL.h>
#elif defined _WIN32 || defined _WIN64
#include <GL\glut.h>
#endif 

#include <thread>
#include <sstream>

const int NUM_PARTICLES = 300;
const int KERNEL_LIMIT = 75;

const float VISCOUSITY = 500*5.f;
const float PARTICLE_MASS = 500*.13f;
const double h = 16.f;
const float STIFFNESS = 500*5.f;
const float GRAVITY_CONST = 80000*9.82f;

bool pressed = false;
bool show_betaballs = true;

const int TEMPSIZE = 128;
float squares[TEMPSIZE * TEMPSIZE]; // hard coded values for now, with marching squares

//GLUT font 
void * font = GLUT_BITMAP_9_BY_15;

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[Cell::GRID_WIDTH * Cell::GRID_HEIGHT];

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
        std::string fpsString = "Betafluid 0.1.0 | FPS: " + stream.str();
        
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
        
        particles[i].setPos(glm::vec3(20+j*h/2 - 8, 19*16 + k*h/2 - 8, 0.5));
        
    }
    
    for (int j = 0; j < Cell::GRID_WIDTH * Cell::GRID_HEIGHT; j++) {
        cells[j].CreateCell(j);
    }
}

//Trying to reduce calculation by removing random number of neighbours up to the limit of the kernel
void reduceNeighbours(vector<Particle*>& theNeighbours){
    
    vector<Particle*> mapped_neighbours;
    
    for(int i = 0; i < KERNEL_LIMIT; i++){
        int random = rand() % theNeighbours.size();
        mapped_neighbours.push_back(theNeighbours.at(random));
        theNeighbours.erase(theNeighbours.begin() + random);
    }
    
    theNeighbours = mapped_neighbours;

}

void calculateDensityAndPressure(){
    
    /*for (int i = 0; i < 32 * 32; i++){
        //cout << "Grannar för cell nr " << i << endl;
        vector<int> neighbors = cells[i].getNeighbours();
        int count = neighbors.size();
        
        if (i == 34){
            int c = 0;
            cout << " Pos: " << i << endl;
            cout << " Grannar: " << endl;
            for (int k = 0; k < count; k++){
                c++;
                cout << neighbors.at(k) << endl;
            }
            cout << "Antal grannar: " << c - 1 << endl;
        }
    }*/

    for(int i = 0; i < NUM_PARTICLES; i++){
        
        float density_sum = 0;
        int cellIndex = particles[i].getCellIndex();
        
        vector<int> current_cells = cells[cellIndex].getNeighbours();
        
        //std::cout << current_cells.size() << std::endl;
        
        //std::cout << "cellindex in density and pressure: " << cellIndex << std::endl;
    
        for(int j = 0; j < current_cells.size(); j++){
			
            // Loop through all neighbouring particles
            vector<Particle*> neighbours = cells[current_cells.at(j)].getParticles();
            
            // Too many neighbours...
            if(neighbours.size() > KERNEL_LIMIT)
                reduceNeighbours(neighbours);
            
            for(int k = 0; k < neighbours.size(); k++){
                
                //std::cout << "SIZE " << neighbours.size() << std::endl;
                
                Particle *n = neighbours.at(k);
            
                glm::vec3 diffvec = particles[i].getPos() - n->getPos();
            
                float abs_diffvec = glm::length(diffvec);
            
                if(abs_diffvec < h){
            
                    density_sum += PARTICLE_MASS * (315 / (64*M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)),3.0);
                    //cout << "Density: " << PARTICLE_MASS * (315 / (64 * M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)), 3.0) << endl;
                }
                
            }
            
        }
        

        particles[i].setDensity(density_sum);
        particles[i].setPressure(STIFFNESS*(density_sum - 998.f));
        particles[i].applyOtherForce(glm::vec3(0, 0, 0));
        
        
    }

}

void calculateForces(){
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        glm::vec3 gravity = glm::vec3(0, -GRAVITY_CONST*particles[i].getDensity(), 0);
        glm::vec3 pressure = glm::vec3(0);
        glm::vec3 viscousity = glm::vec3(0);
        
        int cellIndex = particles[i].getCellIndex();
        
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
                    
                    float W_const_pressure = 45.0f/(M_PI * glm::pow(h, 6.0)) * glm::pow(h - abs_diffvec, 3.0) / abs_diffvec;
                    
                    glm::vec3 W_pressure_gradient = glm::vec3(W_const_pressure * diffvec.x, W_const_pressure * diffvec.y, 0);
                    
                    float visc_gradient = (45/(M_PI * glm::pow(h, 6.0)))*(h - abs_diffvec);
                    
                    pressure +=  -PARTICLE_MASS * ((particles[i].getPressure() + n->getPressure()) / (2 * n->getDensity())) * W_pressure_gradient;
                    
                    viscousity += VISCOUSITY * PARTICLE_MASS * ((n->getVelocity() - particles[i].getVelocity()) / (n->getDensity())) * visc_gradient;
                    
                }
                
            }
            
        }
        
       // if(i == 10)
        //	std::cout << "pressure x " << pressure.x << "pressure y " << pressure.y << std::endl;
        
        particles[i].setViscousityForce(viscousity);
        particles[i].setPressureForce(pressure);
        particles[i].setGravityForce(gravity);
        
    }

}

void calculateAcceleration(){
    
    // Clear all particles in cells
    for (int j = 0; j < Cell::GRID_WIDTH * Cell::GRID_HEIGHT; j++) {
        
        cells[j].clearParticles();
        
    }
    
    // Push every particle into corresponding cell
    for(int i = 0; i < NUM_PARTICLES; i++) {
        
        cells[particles[i].getCellIndex()].addParticle(particles[i]);
        
    }
    
    calculateDensityAndPressure();
    calculateForces();

}

void drawBetaBalls(){
    
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
        
        int a,b;
        
        //int cellIndex = (x + y*32) / kSize;
    
        //a = (k/2) % TEMPSIZE;
        //b = ((k/TEMPSIZE)*Cell::GRID_HEIGHT)/2;
        
        
        a = (k % TEMPSIZE) / (TEMPSIZE/Cell::GRID_HEIGHT);
        b = k / (TEMPSIZE * (TEMPSIZE/Cell::GRID_HEIGHT));
        
        
       // if(k == 4)
            //std::cout << "CELLINDEX: " << cellIndex << std::endl;
        //std::cout << "a : " << a << std::endl << "b: " << b << std::endl << "a + b: " << a+b << std::endl;

        //std::cout << "cellindex: " << cellIndex << std::endl;
        int cellIndex = a + b * Cell::GRID_HEIGHT;
        
        //std::cout << "cellindex: " << cellIndex << std::endl;
        
        vector<int> current_cells = cells[cellIndex].getNeighbours();
        
        //std::cout << "curent cells " << current_cells.size() << std::endl;
        //std::cout << "cellindex " << cellIndex << std::endl;
        
        for(int j = 0; j < current_cells.size(); j++){
            
            //Loop through all neighbouring particles
            vector<Particle*> neighbours = cells[current_cells.at(j)].getParticles();
            
             //Too many neighbours...
           // if(neighbours.size() > KERNEL_LIMIT)
             //   reduceNeighbours(neighbours);
            
            for(int m = 0; m < neighbours.size(); m++){
                
               // std::cout << "squares " << squares[k] << std::endl;
                squares[k] += (neighbours.at(m)->getRadius()*neighbours.at(m)->getRadius()) / ((x - neighbours.at(m)->getPos().x) * (x - neighbours.at(m)->getPos().x) + (y - neighbours.at(m)->getPos().y) * (y - neighbours.at(m)->getPos().y));
        
                
            }
            
            
        }
        
        // Brute force
         //for each cell, check
//        for(int i = 0; i < NUM_PARTICLES; i++) {
//            // calculate height map, sum
//            
//            squares[k] += (particles[i].getRadius()*particles[i].getRadius()) / ((x - particles[i].getPos().x) * (x - particles[i].getPos().x) + (y - particles[i].getPos().y) * (y - particles[i].getPos().y));
//        }
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
            //t
            
            
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
            
            if (bitwiseSum >= 1.f) { // bitwiseSum cannot assume negative values and 0 ®is an empty square.
                
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
                
                glColor4f(0.5f, 0.7f, 1.f, 0.0f);
                
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
                        //std::cout << "SPECIAL CASE 5" << endl;
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
                        //std::cout << "SPECIAL CASE 10" << endl;
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
    
    /*for(int i = 0; i < NUM_PARTICLES; i++){
     
    	particles[i].DrawObjects();
     
     }*/


}

void drawParticles(){
    
    for(int i = 0; i < NUM_PARTICLES; i++){
     
    	particles[i].DrawObjects();
     
     }

}

void display()
{
    handleFps();
    
    if(show_betaballs)
    	drawBetaBalls();
    else
        drawParticles();
    
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
        
        //if(i == 10)
           // std::cout << " x " << particles[i].getPos().x << " y " << particles[i].getPos().y << std::endl;
        
    }
    
}

void handleInputs(){

     // Apply magic spells from mouse input.
    if(glfwGetMouseButton(window, 0)) {
    
	    double xMouse,yMouse;
    
   	 	glfwGetCursorPos(window, &xMouse, &yMouse);
    
        xMouse = xMouse / 512.f;
        yMouse = (512.f - yMouse)  / 512.f;
    
    	if (xMouse < 0 || xMouse > 1)
        	xMouse = xMouse > 0.5 ? 1 : 0;
    
    	if (yMouse < 0 || yMouse > 1)
        	yMouse = yMouse > 0.5 ? 1 : 0;
    
    	int x = (int) (xMouse * Cell::GRID_WIDTH);
    	int y = (int) (yMouse * Cell::GRID_HEIGHT);
    
    	int cellIndex = x + y * Cell::GRID_WIDTH;
    
        vector<int> current_cells = cells[cellIndex].getNeighbours();
    
    	for(int j = 0; j < current_cells.size(); j++){
        
        	vector<Particle*> neighbours = cells[current_cells.at(j)].getParticles();
     	
        	for (int i = 0; i < neighbours.size(); i++) {
            
            	float vec_x = 0;
                float vec_y = 300000;
            
            	neighbours.at(i)->applyOtherForce(glm::vec3(vec_x, vec_y, 0));
            
        	}
		}
    }
	
    // Toggle Betaballs
    if (glfwGetKey(window, GLFW_KEY_G) ) {
        
        if (!pressed){
            pressed = true;
            show_betaballs = !show_betaballs;
        }
        
    } else {
        if (pressed) {
            pressed = false;
        }
    }

}

void renderString(void * font, std::string k, int posx, int posy){

	glRasterPos2i(posx, posy);

	for (std::string::iterator i = k.begin(); i != k.end(); i++){
		char c = *i;
		glutBitmapCharacter(font, c);
	}
}

//Show the text
void drawText(){
	glPushMatrix();

		glLoadIdentity();
		gluOrtho2D(0.0, 512.0, 0.0, 512.0);
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();

			glLoadIdentity();
			glColor3f(1.0, 0.0, 0.0);
			//renderString(font, string, posx, posy)
			renderString(font, "Press G(spot) to get balls", 230, 480);
			renderString(font, "Carl Bildt this city", 230, 460);
			renderString(font, "Play with water using mouse", 230, 440);
			glMatrixMode(GL_MODELVIEW);

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);

	glPopMatrix();
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
        
        box.DrawBox();
		drawText();
        calculateAcceleration();
        handleInputs();
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
