//#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream> 

#include <vector>
#include <algorithm>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;
using namespace std;


#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"

// CPU representation of a particle
struct Particle{
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

const int MaxParticles = 100000;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticle(){

	for (int i = LastUsedParticle; i<MaxParticles; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i<LastUsedParticle; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void SortParticles(){
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static const GLfloat g_vertex_buffer_data_cube[] = {
	-1.0f, -1.0f, -1.0f, // triangle 1 : begin
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f, -1.0f, // triangle 2 : begin
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f, // triangle 2 : end
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};
// One color for each vertex. They were generated randomly.
static const GLfloat g_color_buffer_data[] = {
	0.583f, 0.771f, 0.014f, 0.1f,
	0.609f, 0.115f, 0.436f, 0.1f,
	0.327f, 0.483f, 0.844f, 0.1f,
	0.822f, 0.569f, 0.201f, 0.1f,
	0.435f, 0.602f, 0.223f, 0.1f,
	0.310f, 0.747f, 0.185f, 0.1f,
	0.597f, 0.770f, 0.761f, 0.1f,
	0.559f, 0.436f, 0.730f, 0.1f,
	0.359f, 0.583f, 0.152f, 0.1f,
	0.483f, 0.596f, 0.789f, 0.1f,
	0.559f, 0.861f, 0.639f, 0.1f,
	0.195f, 0.548f, 0.859f, 0.1f,
	0.014f, 0.184f, 0.576f, 0.1f,
	0.771f, 0.328f, 0.970f, 0.1f,
	0.406f, 0.615f, 0.116f, 0.1f,
	0.676f, 0.977f, 0.133f, 0.1f,
	0.971f, 0.572f, 0.833f, 0.1f,
	0.140f, 0.616f, 0.489f, 0.1f,
	0.997f, 0.513f, 0.064f, 0.1f,
	0.945f, 0.719f, 0.592f, 0.1f,
	0.543f, 0.021f, 0.978f, 0.1f,
	0.279f, 0.317f, 0.505f, 0.1f,
	0.167f, 0.620f, 0.077f, 0.1f,
	0.347f, 0.857f, 0.137f, 0.1f,
	0.055f, 0.953f, 0.042f, 0.1f,
	0.714f, 0.505f, 0.345f, 0.1f,
	0.783f, 0.290f, 0.734f, 0.1f,
	0.722f, 0.645f, 0.174f, 0.1f,
	0.302f, 0.455f, 0.848f, 0.1f,
	0.225f, 0.587f, 0.040f, 0.1f,
	0.517f, 0.713f, 0.338f, 0.1f,
	0.053f, 0.959f, 0.120f, 0.1f,
	0.393f, 0.621f, 0.362f, 0.1f,
	0.673f, 0.211f, 0.457f, 0.1f,
	0.820f, 0.883f, 0.371f, 0.1f,
	0.982f, 0.099f, 0.879f, 0.1f
};


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}




	glfwWindowHint(GLFW_SAMPLES, 4); // anti aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // openGL major version to be 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // minor set to 3, which makes the version 3.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OS only
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 18 - Particules", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetCursorPos(window, 1024 / 2, 768 / 2);
	glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = true; // Needed for cor§e profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// Create and compile our GLSL program from the shaders
	GLuint programID_Cube = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	// Create and compile our GLSL program from the shaders
	GLuint program_ID = LoadShaders("Particle.vertexshader", "Particle.fragmentshader");

	//// Vertex shader
	GLuint CameraRight_worldspace_ID = glGetUniformLocation(program_ID, "CameraRight_worldspace");
	GLuint CameraUp_worldspace_ID = glGetUniformLocation(program_ID, "CameraUp_worldspace");
	GLuint ViewProjMatrixID = glGetUniformLocation(program_ID, "VP");

	// fragment shader
	GLuint TextureID = glGetUniformLocation(program_ID, "myTextureSampler");

	static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
	static GLubyte* g_particule_color_data = new GLubyte[MaxParticles * 4];

	for (int i = 0; i<MaxParticles; i++){
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
	}



	GLuint Texture = loadDDS("particle.DDS");

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};

	//Buffers for particles 
	GLuint billboard_vertex_buffer;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	GLuint particles_position_buffer;
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	GLuint particles_color_buffer;
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	

	for (int i = 0; i<500; i++){
		int particleIndex = FindUnusedParticle();
		ParticlesContainer[particleIndex].life = 10.0f; // This particle will live 5 seconds.
		ParticlesContainer[particleIndex].pos = glm::vec3(0, 0, -1.0f);

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
		// Very bad way to generate a random direction; 
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc)
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
			);

		ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;


		// Very bad way to generate a random color
		ParticlesContainer[particleIndex].r = rand() % 256;
		ParticlesContainer[particleIndex].g = rand() % 256;
		ParticlesContainer[particleIndex].b = rand() % 256;
		ParticlesContainer[particleIndex].a = (rand() % 256) / 3;

		ParticlesContainer[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;

	}
	
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID_Cube, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, -3), // Camera is at (4,3,-3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	glm::mat4 VP = Projection * View;
	//Buffers for Cube
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_cube), g_vertex_buffer_data_cube, GL_STATIC_DRAW);
	//Color Cube
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	

	double lastTime = glfwGetTime();
	do
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader

		//Updating the all matrix
		computeMatricesFromInputs();

		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;


		
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		//// We will need the camera's position in order to sort the particles
		//// w.r.t the camera's distance.
		//// There should be a getCameraPosition() function in common/controls.cpp, 
		//// but this works too.
		//glm::inverse(ViewMatrix)[3]
		glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

		glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;


		// Simulate all particles
		int ParticlesCount = 0;
		for (int i = 0; i<MaxParticles; i++){

			Particle& p = ParticlesContainer[i]; // shortcut

			if (p.life > 0.0f){

				// Decrease life
				//p.life -= delta;
				if (p.life > 0.0f){
					glm::vec3 randomdir = glm::vec3(
						(rand() % 2000 - 1000.0f) / 1000.0f,
						(rand() % 2000 - 1000.0f) / 1000.0f,
						(rand() % 2000 - 1000.0f) / 1000.0f
						);

					// Simulate simple physics : gravity only, no collisions
					/*p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;*/

					if (p.pos.y < -1.0f){
						//p.pos.y = -1.0f;
						glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
						p.speed += maindir * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;
					}
					else
					{
						p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;
					}
					if (p.pos.x < -1.0f){
						glm::vec3 maindir = glm::vec3(10.0f, 0.0f, 0.0f);
						p.speed += maindir * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;
					}
					if (p.pos.z < -1.0f){
						glm::vec3 maindir = glm::vec3(0.0f, 0.0f, 10.0f);
						p.speed += maindir * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;
					}
					if (p.pos.z > 2.0f){
						glm::vec3 maindir = glm::vec3(0.0f, 0.0f, -10.0f);
						p.speed += maindir * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;
					}
					if (p.pos.x > 2.0f){
						glm::vec3 maindir = glm::vec3(-10.0f, 0.0f, 0.0f);
						p.speed += maindir * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;
					}
					
					
					
					p.cameradistance = glm::length2(p.pos);
					//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

					// Fill the GPU buffer
					g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
					g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
					g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

					g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

					g_particule_color_data[4 * ParticlesCount + 0] = p.r;
					g_particule_color_data[4 * ParticlesCount + 1] = p.g;
					g_particule_color_data[4 * ParticlesCount + 2] = p.b;
					g_particule_color_data[4 * ParticlesCount + 3] = p.a;

				}
				//else{
				//	// Particles that just died will be put at the end of the buffer in SortParticles();
				//	p.cameradistance = -1.0f;
				//}

				ParticlesCount++;

			}
		}
		
		SortParticles();


		/*printf("%d ",ParticlesCount);
		std::cout << "Camera up " << View[0][1] << "   " << View[1][1] << "   " << View[2][1] << endl;
		std::cout << "headup ID  " << ViewMatrix[0][1] << "   " << ViewMatrix[1][1] << "    " << ViewMatrix[2][1] << endl;
		std::cout << "CAMERAPOS " << CameraPosition[2] << endl;*/

		// Update the buffers that OpenGL uses for rendering.
		// There are much more sophisticated means to stream data from the CPU to the GPU, 
		// but this is outside the scope of this tutorial.
		// http://www.opengl.org/wiki/Buffer_Object_Streaming


		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//// Use our shader
		glUseProgram(program_ID);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		 //Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		//// Same as the billboards tutorial
		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

		//// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		//// These functions are specific to glDrawArrays*Instanced*.
		//// The first parameter is the attribute buffer we're talking about.
		//// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
		//// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	

		// Use our shader
		glUseProgram(programID_Cube);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);
		// 1rst attribute buffer : vertices
		//glClearColor(0.0f, 0.4f, 0.3f, 0.3f);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			3,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			4,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);
		// Draw the CUBE !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); //CUBE


		

		
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);

		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);


	//delete[] g_particule_position_size_data;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteProgram(program_ID);
	glDeleteProgram(programID_Cube);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);


	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
