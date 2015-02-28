#define GLEW_STATIC
#define _USE_MATH_DEFINES
#define GLFW_KEY_W
#define GLFW_KEY_A
#define GLFW_KEY_S
#define GLFW_KEY_D

#include <GL/glew.h>
#include <iostream>
#include "Particle.h"
#include "Box.h"
#include "Cell.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <sstream>

const int NUM_PARTICLES = 500;
const int KERNEL_LIMIT = 100;

const float VISCOUSITY = 500*5.f;
const float PARTICLE_MASS = 500*.14f;
const double h = 16.f;
const float STIFFNESS = 500*5.f;
const float GRAVITY_CONST = 8000*9.82f;

GLfloat newDown[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat down[4] = { 0.0f, -1.0f, 0.0f, 1.0f };
GLfloat model[16];

Particle particles[NUM_PARTICLES];
Box box = Box();

Cell cells[Cell::GRID_WIDTH * Cell::GRID_HEIGHT * Cell::GRID_LENGTH];

// FPS specific vars
double lastTime;
int frames = 0;

// Rotating vars
double newTime, currTime = 0, deltaTime = 0, phi = 0, theta = 0;

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
        std::string fpsString = "Betafluid 0.0.4 | FPS: " + stream.str();
        
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
    

    int x = 0, y = 0, z = 0;
    
    for(int i = 0; i < NUM_PARTICLES; i++){

        if (x == 10){
            y++;
            x = 0;
        }
        
        if(y == 10){
            z++;
            y = 0;
    	}
        
        x++;

        particles[i].setPos(glm::vec3(10+x*h/2, 19*16 + y*h/2, 10 + z*h/2));
    }

    for (int j = 0; j < Cell::GRID_WIDTH * Cell::GRID_HEIGHT * Cell::GRID_LENGTH; j++) {
        
        cells[j].CreateCell(j);
    }
    
}

//Trying to reduce calculation by removing random number of neighbours up to the limit of the kernel
//
void reduceNeighbours(vector<Particle*>& theNeighbours){
    
    //std::cout << theNeighbours.size() << std::endl;;
    
    //Temp
    vector<Particle*> mapped_neighbours;
    
    for(int i = 0; i < KERNEL_LIMIT; i++){
        int random = rand() % theNeighbours.size();
        mapped_neighbours.push_back(theNeighbours.at(random));
        theNeighbours.erase(theNeighbours.begin() + random);
    }
    
    theNeighbours = mapped_neighbours;
    
    //std::cout << "tokmÃ¥nga partiklar";
}

void calculateDensityAndPressure(){

	//
	//																						ANVÄND FÖR ATT TESTA NEIGHBOURS
	//
	//for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT * GRID_LENGTH; i++){
	//	//cout << "Grannar för cell nr " << i << endl;
	//	vector<int> neighbors = cells[i].getNeighbours();
	//	int count = neighbors.size();
	//	
	//	if (i == 19){
	//		int c = 0;
	//		cout << " Pos: " << i << endl;
	//		cout << " Grannar: " << endl;

	//		for (int k = 0; k < count; k++){
	//			c++;
	//			cout << neighbors.at(k) << endl; 
	//		}

	//		cout << "Antal grannar: " << c - 1 << endl;
	//	}
	//}

    for(int i = 0; i < NUM_PARTICLES; i++){
        
        float density_sum = 0;
        int cellIndex = particles[i].getCellIndex();
        //int limit = 0;
		
		//Hämtar neighbouring celler
        vector<int> current_cells = cells[cellIndex].getNeighbours();
		//cout << "cellIndex = " << cellIndex << endl;
		//cout << "current_cells.size() = " << current_cells.size() << endl;
        for(int j = 0; j < current_cells.size(); j++){

            // Loop through all neighbouring particles
            vector<Particle*> neighbours = cells[current_cells.at(j)].getParticles();

            // Too many neighbours...
            if(neighbours.size() > KERNEL_LIMIT)
                reduceNeighbours(neighbours);
            
            for(int k = 0; k < neighbours.size(); k++){
                
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
        
		glm::vec3 gravity = glm::vec3(newDown[0],newDown[1],newDown[2])*GRAVITY_CONST;
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
                    
                    glm::vec3 W_pressure_gradient = glm::vec3(W_const_pressure * diffvec.x, W_const_pressure * diffvec.y, W_const_pressure * diffvec.z);
                    
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
    for (int j = 0; j < Cell::GRID_WIDTH * Cell::GRID_HEIGHT * Cell::GRID_LENGTH; j++) {
        
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

void handleCamera(){

	glMatrixMode(GL_MODELVIEW);
	newTime = glfwGetTime();
	deltaTime = newTime - currTime;
	currTime = newTime;

	if (glfwGetKey(window, GLFW_KEY_D)) {
		phi -= deltaTime*M_PI / 2.0; // Rotate 90 degrees per second (pi/2)
		phi = fmod(phi, M_PI*2.0); // Wrap around at 360 degrees (2*pi)
		if (phi < 0.0) phi += M_PI*2.0; // If phi<0, then fmod(phi,2*pi)<0
		glRotatef(-phi, 0, 1, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		phi += deltaTime*M_PI / 2.0; // Rotate 90 degrees per second (pi/2)
		phi = fmod(phi, M_PI*2.0);
		glRotatef(phi, 0, 1, 0);
	}

	if (glfwGetKey(window, GLFW_KEY_S)) {
		theta -= deltaTime*M_PI / 2.0; // Rotate 90 degrees per second (pi/2)
		theta = fmod(theta, M_PI*2.0); // Wrap around at 360 degrees (2*pi)
		if (theta < 0.0) theta += M_PI*2.0; // If phi<0, then fmod(phi,2*pi)<0
		glRotatef(-theta, 1, 0, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		theta += deltaTime*M_PI / 2.0; // Rotate 90 degrees per second (pi/2)
		theta = fmod(theta, M_PI*2.0);
		glRotatef(theta, 1, 0, 0);
	}

/*
	if (glfwGetKey(window, GLFW_KEY_LEFT)){
		glTranslatef(10.f,0.0f,0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)){
		glTranslatef(-10.f, 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_UP)){
		glTranslatef(0.f, -10.f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)){
		glTranslatef(0.f, 10.f, 0.0f);
	}*/

}

void drawCoordinateAxes(){

    glPushMatrix();
    glBegin(GL_LINES);
    
    glColor3f(1.f, 1.f, 1.f);
    
    glVertex3f(0.f,0.f,0.f);
    glVertex3f(256.f,0.f,0.f);
    
    glVertex3f(0.f,0.f,0.f);
    glVertex3f(0.f,256.f,0.f);

    glVertex3f(0.f,0.f,0.f);
    glVertex3f(0.f, 0.f, 256.f);
    
    glEnd();
    glPopMatrix();


}


//Calculates the gravity vector based on the current position of the box
void calculateNewGravityVec(){
    
    newDown[0] = model[0] * down[0] + model[1] * down[1] + model[2] * down[2] + model[3] * down[3];
    newDown[1] = model[4] * down[0] + model[5] * down[1] + model[6] * down[2] + model[7] * down[3];
    newDown[2] = model[8] * down[0] + model[9] * down[1] + model[10] * down[2] + model[11] * down[3];

}

int main(int argc, char *argv[])
{

    init();
    
    glfwInit();
    
    window = glfwCreateWindow(512, 512, "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
	glTranslatef(512.f, 512.f, 0.0f);

	bool running = true;

    while(!glfwWindowShouldClose(window) && running){

        float ratio;
        int width, height;
       
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        
        glClear(GL_COLOR_BUFFER_BIT);
     
		glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

		glOrtho(0.0, 1024.0, 0.0, 1024.0, -1024.0, 1024);

		handleCamera();

        // Get rotation matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, model);
        
        calculateNewGravityVec();
        drawCoordinateAxes();
        box.DrawBox();
			
		calculateAcceleration();
        display();
        idle();

        //Swap front and back buffers
        glfwSetWindowSizeCallback(window, reshape_window);
		glfwSwapBuffers(window);
        
        //Poll for and process events
        glfwPollEvents();

		glFinish();

		running = !glfwGetKey(window, GLFW_KEY_ESCAPE);

	}
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return EXIT_SUCCESS;
    
}
