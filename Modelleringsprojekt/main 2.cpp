/*
 *  main.cpp
 *  Graphics Assignment Four
 *
 *  Created by Karl Grogan on 24/04/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <stdio.h>
#include "main.h"
#include <stdlib.h>
#include <GLUT/glut.h>
#include "Particle.h"
#include "Box.h"

using namespace std;


// Horizontal position of the box.
Particle particles[500];
//Box myBox;


// Create the particles forthe fountain:
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
			
		case GLUT_KEY_UP:  
			for(int i = 0; i <= 500; i++)
			{
				particles[i].SetG((float)particles[i].GetG()+0.01f); 
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

static void display(void)
{
	// geometry
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for(int i = 0; i <= 500; i++)
	{
		
		particles[i].DrawObjects();
		//cout << particles[i].GetXPos()<< endl;
	}	
	
	
    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

void idle(void)
{
	for (int i = 0; i < 500; i++)
	{
		particles[i].EvolveParticle();
	}
	
    glutPostRedisplay();
}


int main(int argc, char *argv[])
{
	
	CreateParticles();
	
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
	
	return EXIT_SUCCESS;
	
}


