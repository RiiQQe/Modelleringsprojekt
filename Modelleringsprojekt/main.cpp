#define GLEW_STATIC
#define _USE_MATH_DEFINES
#define GLFW_KEY_W
#define GLFW_KEY_A
#define GLFW_KEY_S
#define GLFW_KEY_D

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <OpenGL/gl3.h>
#include <iostream>
#include "Particle.h"
#include "Box.h"
#include "Cell.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <sstream>

#include <fstream>

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec2 position;"
"in vec3 color;"
"out vec3 Color;"
"void main() {"
"   Color = color;"
"   gl_Position = vec4(position, 0.0, 1.0);"
"}";

const GLchar* fragmentSource =

"#version 150 core\n"
"uniform vec3 triangleColor;"
"in vec3 Color;"
"out vec4 outColor;"
"void main() {"
"   outColor = vec4(Color, 1.0);"
"}"
;


const int NUM_PARTICLES = 500;
const int KERNEL_LIMIT = 130;

const float VISCOUSITY = 500*5.f;
const float PARTICLE_MASS = 500*.14f;
const double h = 16.f;
const float STIFFNESS = 500*5.f;
const float GRAVITY_CONST = 80000*9.82f;

bool user_running = false;

GLfloat newDown[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat down[4] = { 0.0f, -1.0f, 0.0f, 1.0f };
GLfloat model[16];

//Particle particles[NUM_PARTICLES];
std::vector<Particle> particles;

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

// Adds new particles

void addNewParticles(){
    
    Particle p;
    p.CreateParticle();
    p.setPos(glm::vec3(128.f, 256.f, 128.f));
    p.setVel(glm::vec3(0, -100000.f, 0.f));
    
    particles.push_back(p);
    
}


// Initialize particles system
void initParticles()
{
    
    for(int i = 0; i < NUM_PARTICLES; i++) {
        Particle p;
        p.CreateParticle();
        particles.push_back(p);
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
        
        particles[i].setPos(glm::vec3(20+x*h/2, 20 + y*h/2, 20 + z*h/2));
    }
    
    
    // Create all the cells
    
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
    
    for(int i = 0; i < particles.size(); i++){
        
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
    
    for(int i = 0; i < particles.size(); i++){
        
        glm::vec3 gravity = glm::vec3(newDown[0],newDown[1],newDown[2])*GRAVITY_CONST*particles[i].getDensity();
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
    for(int i = 0; i < particles.size(); i++) {
        
        cells[particles[i].getCellIndex()].addParticle(particles[i]);
        
    }
    
    calculateDensityAndPressure();
    calculateForces();
    
}

void display()
{
    handleFps();
    
    for(int i = 0; i < particles.size(); i++){
        
        particles[i].DrawObjects();
        
    }
    
}

void reshape_window(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void idle()
{
    for (int i = 0; i < particles.size(); i++)
    {
        particles[i].EvolveParticle();
    }
    
}

void handleInputs(){
    
    glMatrixMode(GL_MODELVIEW);
    newTime = glfwGetTime();
    deltaTime = newTime - currTime;
    currTime = newTime;
    
    //GENERAL INPUT
    
    // Init particle system
    if (glfwGetKey(window, GLFW_KEY_G)) {
        user_running = true;
    }
    
    // Add particles
    if (glfwGetKey(window, GLFW_KEY_H)) {
        
        if(user_running)
            addNewParticles();
    }
    
    //CAMERA CONTROLS
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

void drawParticlesContainer(){
    
    
    float containerSize = 256.f / 2;
    
    glPushMatrix();
    
    //Move cube to right coordinates
    glTranslatef(containerSize, containerSize, containerSize);
    
    //The Borders
    //glPushMatrix();
    
    glBegin(GL_LINES);
    glColor3f(1.f, 1.f, 1.f);
    
    //Top
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    //Bottom
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    
    //Front
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    
    //BACK
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    
    // Left
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    
    // Right
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    
    glEnd();
    
    glDisable(GL_LIGHTING);
    
    //For transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //The Walls
    glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
    // Top face (y = 1.0f)
    // Define vertices in counter-clockwise (CCW) order with normal pointing out
    glColor4f(1.f, 1.f, 1.f, 0.2f);     // Green
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    
    
    // Bottom face (y = -1.0f*containerSize)
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    
    // Front face  (z = 1.0f*containerSize)
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    
    // Back face (z = -1.0f*containerSize)
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    
    // Left face (x = -1.0f*containerSize)
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(-1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    
    // Right face (x = 1.0f*containerSize)
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, -1.0f*containerSize);
    glVertex3f(1.0f*containerSize, 1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, 1.0f*containerSize);
    glVertex3f(1.0f*containerSize, -1.0f*containerSize, -1.0f*containerSize);
    glEnd();  // End of drawing color-cube
    
    glEnable(GL_LIGHTING);
    //
    //    const GLfloat black[4] = { 0.f, 0.f, 0.f, 1.f };
    //    glMaterialfv( GL_FRONT, GL_DIFFUSE, black );
    //    glMaterialfv( GL_FRONT, GL_AMBIENT, black );
    //    const GLfloat white[4] = { 1.f, 1.f, 1.f, 1.f };
    //    glMaterialfv( GL_FRONT, GL_SPECULAR, white );
    
    //glDepthFunc(GL_EQUAL);
    
    
    
    //Render a second time for glass
    
    
    
    glPopMatrix();
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

void drawPlane(){
    
    
    
    glColor4f(0.8f, 0.8f, 0.8f, 1.f);
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glEnable(GL_BLEND);
    
    //Bottom
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(256.0f, 0.0f, 0.0f);
    glVertex3f(256.0f, 0.0f, 256.0f);
    
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 256.0f);
    glVertex3f(256.0f, 0.0f, 256.0f);
    
    glEnd();
    glPopMatrix();
    
    
}

int main(int argc, char *argv[])
{
    
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL
    
    // Open a window and create its OpenGL context
    GLFWwindow* window; // (In the accompanying source code, this variable is global)
    window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);
    
    if(window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window); // Initialize GLEW
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    float vertices[] = {
        0.0f,  0.5f, 1.0f, 0.0f, 0.0f, // Vertex 1 (X, Y)
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Vertex 2 (X, Y)
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f  // Vertex 3 (X, Y)
        
    };
    
    
    // This will identify our vertex buffer
    GLuint vbo;
    
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vbo);
    
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //Shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //const GLchar *sourcevertex = "vertexshader.glsl";
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //const GLchar *sourcefrag = "fragmentshader.glsl";
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          5*sizeof(float), 0);
    
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
                          5*sizeof(float), (void*)(2*sizeof(float)));
    
    do{
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        initParticles();
        // display();
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    
    
}
