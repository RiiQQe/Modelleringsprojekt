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

const int NUM_PARTICLES = 50;
const int KERNEL_LIMIT = 160;

const float VISCOUSITY = 500*5.f;
const float PARTICLE_MASS = 500*.14f;
const double h = 16.f;
const float STIFFNESS = 500*5.f;
const float GRAVITY_CONST = 8000*9.82f;

const int TEMPSIZE = 8;

float squares[TEMPSIZE * TEMPSIZE * TEMPSIZE];

int edgeTable[256] = {
	0x0, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x99, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x33, 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0xaa, 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x66, 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff, 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55, 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0xcc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x55, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0xff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x66, 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa, 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33, 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99, 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0 };

int triTable[256][16] =
{ { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
{ 8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1 },
{ 3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1 },
{ 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
{ 4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1 },
{ 9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1 },
{ 10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1 },
{ 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
{ 5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1 },
{ 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1 },
{ 2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
{ 7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
{ 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1 },
{ 11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1 },
{ 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1 },
{ 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1 },
{ 11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1 },
{ 2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1 },
{ 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
{ 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1 },
{ 6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
{ 6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1 },
{ 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1 },
{ 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1 },
{ 3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
{ 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1 },
{ 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1 },
{ 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
{ 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1 },
{ 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1 },
{ 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1 },
{ 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
{ 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
{ 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1 },
{ 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1 },
{ 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
{ 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1 },
{ 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1 },
{ 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1 },
{ 3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
{ 6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1 },
{ 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1 },
{ 10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
{ 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1 },
{ 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
{ 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
{ 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1 },
{ 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1 },
{ 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1 },
{ 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1 },
{ 0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1 },
{ 7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
{ 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1 },
{ 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1 },
{ 10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1 },
{ 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1 },
{ 7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
{ 6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
{ 8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1 },
{ 6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1 },
{ 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1 },
{ 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1 },
{ 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1 },
{ 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
{ 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1 },
{ 10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1 },
{ 10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
{ 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1 },
{ 9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
{ 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1 },
{ 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1 },
{ 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1 },
{ 7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1 },
{ 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1 },
{ 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1 },
{ 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1 },
{ 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1 },
{ 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1 },
{ 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1 },
{ 6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1 },
{ 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1 },
{ 6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1 },
{ 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1 },
{ 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1 },
{ 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1 },
{ 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1 },
{ 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1 },
{ 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1 },
{ 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1 },
{ 0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1 },
{ 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1 },
{ 11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1 },
{ 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1 },
{ 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1 },
{ 2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
{ 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1 },
{ 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1 },
{ 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1 },
{ 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
{ 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1 },
{ 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1 },
{ 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1 },
{ 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1 },
{ 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1 },
{ 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1 },
{ 9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1 },
{ 5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1 },
{ 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1 },
{ 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1 },
{ 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1 },
{ 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1 },
{ 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1 },
{ 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1 },
{ 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1 },
{ 11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
{ 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1 },
{ 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1 },
{ 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1 },
{ 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1 },
{ 1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1 },
{ 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1 },
{ 0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
{ 9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1 },
{ 1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } };

float vertlist[12][3];

int ntriang;
float triangles[12][3];

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
    //Temp
    vector<Particle*> mapped_neighbours;
    
    for(int i = 0; i < KERNEL_LIMIT; i++){
        int random = rand() % theNeighbours.size();
        mapped_neighbours.push_back(theNeighbours.at(random));
        theNeighbours.erase(theNeighbours.begin() + random);
    }
    
    theNeighbours = mapped_neighbours;
}

void calculateDensityAndPressure(){

    for(int i = 0; i < NUM_PARTICLES; i++){
        
        float density_sum = 0;
        int cellIndex = particles[i].getCellIndex();
    	
		//Hämtar neighbouring celler
        vector<int> current_cells = cells[cellIndex].getNeighbours();
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
          
                if(abs_diffvec < h){
                    density_sum += PARTICLE_MASS * (315 / (64*M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)),3.0);
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

//glm::vec3 VertexInterp(float x, float y, float z, float dx, float dy, float dz, float val1, float val2){
glm::vec3 VertexInterp(glm::vec3 xyz, glm::vec3 dxyz, float val1, float val2){

	glm::vec3 temp;
	double mu;

	if (abs(1.0f - val1) < 0.00001)
		return xyz;
	if (abs(1.0f - val2) < 0.00001)
		return dxyz;
	if (abs(val1 - val2) < 0.00001)
		return xyz;

	mu = (1.0f - val1) / (val2 - val1);

	temp.x = xyz.x + mu * (dxyz.x - xyz.x);
	temp.y = xyz.y + mu * (dxyz.y - xyz.y);
	temp.z = xyz.z + mu * (dxyz.z - xyz.z);

	return temp;
}

void displayWithoutMarching(){
	for(int i = 0; i < NUM_PARTICLES; i++){
	
		particles[i].DrawObjects();
	}
}

void display()
{
    handleFps();

	int kSize = 512 / TEMPSIZE;
    
    //for(int i = 0; i < NUM_PARTICLES; i++){
    //
    //	particles[i].DrawObjects();
    //}
	// Iterate through all cells
	for (int k = 0; k < TEMPSIZE * TEMPSIZE * TEMPSIZE; k++) {
		int x = kSize * (k % TEMPSIZE);
		int y = (k / (TEMPSIZE)) * kSize;
		int z = (k / (TEMPSIZE * TEMPSIZE)) * kSize;


		// for each cell, check
		for (int i = 0; i < NUM_PARTICLES; i++) {

			// calculate height map, sum
			// squares contains the radius to the particle
			// jag tror de är denna som inte funkar som vi har tänkt oss riktigt
			squares[k] += (particles[i].getRadius()*particles[i].getRadius()) / ((x - particles[i].getPos().x) * (x - particles[i].getPos().x) +
			(y - particles[i].getPos().y) * (y - particles[i].getPos().y) + 
			(z - particles[i].getPos().z) * (z - particles[i].getPos().z));
		}
	}

	for (int i = 0; i < TEMPSIZE - 1; i++) {
		for (int j = 0; j < TEMPSIZE - 1; j++) {
			for (int k = 0; k < TEMPSIZE - 1; k++){

				int bitwiseSum = 0;

				float a, b, c, d, e, f, g, h;
				//front
				a = squares[i + (j * TEMPSIZE + TEMPSIZE) + k * TEMPSIZE * TEMPSIZE]; // upper left 
				b = squares[i + 1 + (j * TEMPSIZE + TEMPSIZE) + k * TEMPSIZE * TEMPSIZE]; // upper right
				c = squares[i + (j * TEMPSIZE) + k * TEMPSIZE * TEMPSIZE]; // lower left
				d = squares[i + 1 + (j * TEMPSIZE) + k * TEMPSIZE * TEMPSIZE]; // lower right

				//back
				e = squares[i + (j * TEMPSIZE + TEMPSIZE) + k * TEMPSIZE * TEMPSIZE + TEMPSIZE]; // upper left
				f = squares[i + 1 + (j * TEMPSIZE + TEMPSIZE) + k * TEMPSIZE * TEMPSIZE + TEMPSIZE]; // upper right
				g = squares[i + (j * TEMPSIZE) + k * TEMPSIZE * TEMPSIZE + TEMPSIZE]; // lower left
				h = squares[i + 1 + (j * TEMPSIZE) + k * TEMPSIZE * TEMPSIZE + TEMPSIZE]; // lower right

				//Front
				glm::vec3 avec = glm::vec3(i * kSize + kSize / 2, (j + 1) * kSize + kSize / 2, k * kSize + kSize / 2); // Top left
				glm::vec3 bvec = glm::vec3((i + 1) * kSize + kSize / 2, (j + 1) * kSize + kSize / 2, k * kSize + kSize / 2); //Top right
				glm::vec3 cvec = glm::vec3(i * kSize + kSize / 2, j * kSize + kSize / 2, k * kSize + kSize / 2); //Bot left
				glm::vec3 dvec = glm::vec3((i + 1) * kSize + kSize / 2, j * kSize + kSize / 2, k * kSize + kSize / 2); // Bot right
				//Back
				glm::vec3 evec = glm::vec3(i * kSize + kSize / 2, (j + 1) * kSize + kSize / 2, (k + 1) * kSize + kSize / 2); // Top left
				glm::vec3 fvec = glm::vec3((i + 1) * kSize + kSize / 2, (j + 1) * kSize + kSize / 2, (k + 1) * kSize + kSize / 2); //Top right
				glm::vec3 gvec = glm::vec3(i * kSize + kSize / 2, j * kSize + kSize / 2, (k + 1) * kSize + kSize / 2); // Bot left
				glm::vec3 hvec = glm::vec3((i + 1) * kSize + kSize / 2, j * kSize + kSize / 2, (k + 1) * kSize + kSize / 2); // Bot right

				if (a > 1.f)   { bitwiseSum |= 1; } // upper left corner
				if (b > 1.f)   { bitwiseSum |= 2; } // upper right corner
				if (d > 1.f)   { bitwiseSum |= 4; } // lower right corner
				if (c > 1.f)   { bitwiseSum |= 8; } // lower left corner

				if (e > 1.f)   { bitwiseSum |= 16; } // upper left corner
				if (f > 1.f)   { bitwiseSum |= 32; } // upper right corner
				if (h > 1.f)   { bitwiseSum |= 64; } // lower right corner
				if (g > 1.f)   { bitwiseSum |= 128; } // lower left corner

				double mu;
				bool bolesk = true;
				/* Cube is entirely in/out of the surface */
				if (edgeTable[bitwiseSum] == 0)
					bolesk = false;

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[bitwiseSum] & 1 && bolesk){
					glm::vec3 temp = VertexInterp(avec, bvec, a, b);
					cout << "0      << 1    << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[0][0] = temp.x;
					vertlist[0][1] = temp.y;
					vertlist[0][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 2 && bolesk){
					glm::vec3 temp = VertexInterp(bvec, dvec, b, d);
					cout << "1      << 2    << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[1][0] = temp.x;
					vertlist[1][1] = temp.y;
					vertlist[1][2] = temp.z;
				}

				if (edgeTable[bitwiseSum] & 4 && bolesk){
					glm::vec3 temp = VertexInterp(dvec, cvec, d, c);
					cout << "2      << 4    << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[2][0] = temp.x;
					vertlist[2][1] = temp.y;
					vertlist[2][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 8 && bolesk){
					glm::vec3 temp = VertexInterp(cvec, evec, c, e);
					cout << "3      << 8    << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[3][0] = temp.x;
					vertlist[3][1] = temp.y;
					vertlist[3][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 16 && bolesk){
					glm::vec3 temp = VertexInterp(evec, fvec, e, f);
					cout << "4      << 16   << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[4][0] = temp.x;
					vertlist[4][1] = temp.y;
					vertlist[4][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 32 && bolesk){
					glm::vec3 temp = VertexInterp(fvec, hvec, f, h);
					cout << "5      << 32   << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[5][0] = temp.x;
					vertlist[5][1] = temp.y;
					vertlist[5][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 64 && bolesk){
					glm::vec3 temp = VertexInterp(hvec, gvec, h, g);
					cout << "6      << 64   << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[6][0] = temp.x;
					vertlist[6][1] = temp.y;
					vertlist[6][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 128 && bolesk){
					glm::vec3 temp = VertexInterp(gvec, evec, g, e);
					cout << "7      << 128  << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[7][0] = temp.x;
					vertlist[7][1] = temp.y;
					vertlist[7][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 256 && bolesk){
					cout << "8      << 256  << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;
					glm::vec3 temp = VertexInterp(avec, evec, a, e);

					vertlist[8][0] = temp.x;
					vertlist[8][1] = temp.y;
					vertlist[8][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 512 && bolesk){
					glm::vec3 temp = VertexInterp(bvec, fvec, b, f);
					cout << "9      << 512  << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[9][0] = temp.x;
					vertlist[9][1] = temp.y;
					vertlist[9][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 1024 && bolesk){
					glm::vec3 temp = VertexInterp(dvec, hvec, d, h);
					cout << "10     << 1024 << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[10][0] = temp.x;
					vertlist[10][1] = temp.y;
					vertlist[10][2] = temp.z;
				}
				if (edgeTable[bitwiseSum] & 2048 && bolesk){
					glm::vec3 temp = VertexInterp(cvec, gvec, c, g);
					cout << "11     << 2048 << edgeTable[bitwiseSum] = " << edgeTable[bitwiseSum] << " bitewiseSum = " << bitwiseSum << endl;

					vertlist[11][0] = temp.x;
					vertlist[11][1] = temp.y;
					vertlist[11][2] = temp.z;
				}

				glBegin(GL_TRIANGLE_STRIP);
				glColor3f(0.0f,0.0f,1.0f);
				int kalle = 0;
				//Den kan tydligen gå in i flera av if-satserna ovanför, måste ändra bitwiseSum
				//Fast de verkar den ta hand om, om man kollar loggarna
				for (int kalle = 0; triTable[bitwiseSum][kalle] != -1; kalle++){
					int p1 = triTable[bitwiseSum][kalle];

					cout << "p1 = " << p1 << endl;

				/*	cout << "vertlist.x: " << vertlist[p1][0] << endl;
					cout << "vertlist.y: " << vertlist[p1][1] << endl;
					cout << "vertlist.z: " << vertlist[p1][2] << endl;
					cout << endl;*/
					
					glTexCoord2f(0.0, 1.0); glVertex3f(vertlist[p1][0], vertlist[p1][1], vertlist[p1][2]);
				}
				/*if (kalle > 0){
					cout << "------------------------" << endl;
					cout << "------------------------" << endl;
					cout << "i : " << kalle << endl;
				}*/
				
				glEnd();
			}
		}
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
	}

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

	bool running = true;

    while(!glfwWindowShouldClose(window) && running){

        float ratio;
        int width, height;
       
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        
        glClear(GL_COLOR_BUFFER_BIT);
     
		glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

		glOrtho(0.0, 512.0, 0.0, 512.0, -512.0, 512);

		handleCamera();

        // Get rotation matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, model);
        
        calculateNewGravityVec();
        drawCoordinateAxes();
        box.DrawBox();
			
		calculateAcceleration();
        display();
		//displayWithoutMarching();
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
