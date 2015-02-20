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


//Nu ligger det i 16 -> 19

const int NUM_PARTICLES = 27;

const int GRID_WIDTH = 4; //KOLLA I Particle.cpp i funktionen getCellIndex()!!!
const int GRID_HEIGHT = 4;
const int GRID_LENGTH = 4;

const int KERNEL_LIMIT = 20;

const float VISCOUSITY = 2.5f;
const float PARTICLE_MASS = .14f;
const double h = 0.032f;
const float STIFFNESS = 3.f;

Particle particles[NUM_PARTICLES];

Cell cells[GRID_WIDTH * GRID_HEIGHT * GRID_LENGTH];

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
        
    }
    
    int k = 0, j = 0, z = 0 ;
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
		if (i % 9 == 0)
			z++;

        if(i % 3 == 0)
            k++;
        
        if(i % 10 == 0)
            j=0;
        
        j++;
        
        particles[i].setPos(glm::vec3(j*6, k*6, -z*6));
    }
    
    for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT * GRID_LENGTH; j++) {
        cells[j].CreateCell(j);
    }
}

void calculateDensityAndPressure(){


	//for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT * GRID_LENGTH; i++){
	//	//cout << "Grannar för cell nr " << i << endl;
	//	vector<int> neighbors = cells[i].getNeighbours();
	//	int count = neighbors.size();
	//	cout << " Skulle tippa på att de är här felet ligger " << i << endl;
	//	for (int k = 0; k < count; k++){
	//		
	//		if (neighbors.at(k) < 0)
	//			cout << neighbors.at(k) << " //////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	//	
	//	}
	//}

		//cout << "GRATTIS DIN FAGGOT" << endl;
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        float density_sum = 0;
		bool limitBool = false;
        int cellIndex = particles[i].getCellIndex();
        int limit = 0;
		
        vector<int> current_cells = cells[cellIndex].getNeighbours();
		//cout << "cellIndex = " << cellIndex << endl;
		//cout << "current_cells.size() = " << current_cells.size() << endl;
        for(int j = 0; j < current_cells.size(); j++){
			if (limitBool) break;


			//Denna funkar ej, for-loopen kraschar
			//cout << "current_cells.at(j) = " << current_cells.at(j) << endl;

			// Loop through all neighbouring particles
            for(int k = 0; k < cells[current_cells.at(j)].getParticles().size(); k++){
				
				if (++limit > KERNEL_LIMIT){ 
					limitBool = true; // This means that there is many surrounding particles
                    break;
				}
                
                Particle *n = cells[current_cells.at(j)].getParticles().at(k);
				
                glm::vec3 diffvec = particles[i].getPos() - n->getPos();
				
				diffvec/=512.f;

                float abs_diffvec = glm::length(diffvec);
				
                if(abs_diffvec < h){
                    density_sum += PARTICLE_MASS * (315 / (64*M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)),3.0);
					//cout << "Density: " << endl;// << PARTICLE_MASS * (315 / (64 * M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)), 3.0) << endl;
				}
            }
		}
		if(density_sum != 0){
			if (limitBool){
				density_sum = 111000.f;
			}
            particles[i].setDensity(density_sum);
			particles[i].setPressure(STIFFNESS*(density_sum - 998.f));
        }
        else{
            particles[i].setDensity(998.f);
            particles[i].setPressure(STIFFNESS*(998.f - 998.f));
        }
        
    }

}

void calculateForces(){
    
    for(int i = 0; i < NUM_PARTICLES; i++){
        
        glm::vec3 gravity = glm::vec3(0, -9.82f*particles[i].getDensity(), 0);
        glm::vec3 pressure = glm::vec3(0);
        glm::vec3 viscousity = glm::vec3(0);
        
        int cellIndex = particles[i].getCellIndex();
        int limit = 0;
		bool limitBool = false;

		float prevVisc = 0.0f, prevPress = 0.0f;
        
        vector<int> current_cells = cells[cellIndex].getNeighbours();


        
        //Loop through all cells
        for(int j = 0; j < current_cells.size(); j++){
			if (limitBool) break;
            // Loop through all neighbouring particles
            for(int k = 0; k < cells[current_cells.at(j)].getParticles().size(); k++){
                
                Particle *n = cells[current_cells.at(j)].getParticles().at(k);

                if(n->getPos() == particles[i].getPos()){
                    continue;
                }
                
				if (++limit > KERNEL_LIMIT){
					limitBool = true;
                    break;
				}
                
                glm::vec3 diffvec = particles[i].getPos() - n->getPos();
                diffvec/=512.f;
                
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

void handleCamera(){

	glMatrixMode(GL_MODELVIEW);
	newTime = glfwGetTime();
	deltaTime = newTime - currTime;
	currTime = newTime;

	if (glfwGetKey(window, GLFW_KEY_D)) {
		phi -= deltaTime*M_PI / 2.0; // Rotate 90 degrees per second (pi/2)
		phi = fmod(phi, M_PI*2.0); // Wrap around at 360 degrees (2*pi)
		if (phi < 0.0) phi += M_PI*2.0; // If phi<0, then fmod(phi,2*pi)<0
		glRotatef(phi, 0, 1, 0);
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
		glRotatef(theta, 1, 0, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		theta += deltaTime*M_PI / 2.0; // Rotate 90 degrees per second (pi/2)
		theta = fmod(theta, M_PI*2.0);
		glRotatef(theta, 1, 0, 0);
	}


	if (glfwGetKey(window, GLFW_KEY_RIGHT)){
		glTranslatef(10.f,0.0f,0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)){
		glTranslatef(-10.f, 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)){
		glTranslatef(0.f, -10.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_UP)){
		glTranslatef(0.f, 10.0f, 0.0f);
	}

}

void drawAxes(){

    glPushMatrix();
    glBegin(GL_LINES);
    
    glColor3f(1.f, 1.f, 1.f);
    
    glVertex3f(0.f,0.f,0.f);
    glVertex3f(512.f,0.f,0.f);
    
    glVertex3f(0.f,0.f,0.f);
    glVertex3f(0.f,512.f,0.f);
    
    glVertex3f(0.f,0.f,0.f);
    glVertex3f(0.f, 0.f, 512.f);
    
    glEnd();
    glPopMatrix();


}

void drawPlane(){


	glPushMatrix();
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.0f, 0.0f);  
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(512.0f, 0.0f, 0.0f);
	glVertex3f(512.0f, 0.0f, 512.0f);
	glVertex3f(0.0f, 0.0f, 512.0f);
	
	glEnd();
	glPopMatrix();
}


int main(int argc, char *argv[])
{
    
    CreateParticles();

	glfwInit();

    window = glfwCreateWindow(512, 512, "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    while(!glfwWindowShouldClose(window)){

        float ratio;
        int width, height;
       
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        
        glClear(GL_COLOR_BUFFER_BIT);

		//glClearDepth(1.0f);
		        
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

		glOrtho(0.0, 512.0, 0.0, 512.0, -512.0, 512);

		/*ROTATION*/
		handleCamera();

		/*RITA UT PLANET*/
        drawAxes();
		drawPlane();
			
		calculateAcceleration();
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
