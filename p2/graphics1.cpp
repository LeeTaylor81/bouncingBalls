// OpenGL/GLUT starter kit for Windows 7 and Visual Studio 2010
// Created spring, 2011
//
// This is a starting point for OpenGl applications.
// Add code to the "display" function below, or otherwise
// modify this file to get your desired results.
//
// For the first assignment, add this file to an empty Windows Console project
//		and then compile and run it as is.
// NOTE: You should also have glut.h,
// glut32.dll, and glut32.lib in the directory of your project.
// OR, see GlutDirectories.txt for a better place to put them.

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <iostream>

#include "ball.h"

#ifdef _WIN32
	#include "glut.h"
#else
#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif
#endif

// Global Variables (Only what you need!)
double screen_x = 700;
double screen_y = 500;

double gMinVelocity = -0.5;
double gMaxVelocity = 0.5;

double gGravity = -0.0022;
double gFriction = 0.9999;

size_t gBallCount = 10;
std::vector<Ball> balls;

// Global functions
double randDouble(double low, double high)
{
	double range = high - low;
	double rnd = low + (range * rand()) / (RAND_MAX + 1.0);
	return rnd;
}

bool detectCollision(Ball lBall, Ball rBall)
{
	double x1 = lBall.getNextX();
	double x2 = rBall.getNextX();
	double a = x2 - x1;

	double y1 = lBall.getNextY();
	double y2 = rBall.getNextY();
	double b = y2 - y1;

	double radiusSum = lBall.getRadius() + rBall.getRadius();

	if (pow(a, 2) + pow(b, 2) < pow(radiusSum, 2)) 
	{
		return true;
	}
	return false;
}

struct vectortype
{
	double x;
	double y;
};

void Collide(int p1, int p2, std::vector<Ball> &particles)
{
	vectortype en; // Center of Mass coordinate system, normal component
	vectortype et; // Center of Mass coordinate system, tangential component
	vectortype u[2]; // initial velocities of two particles
	vectortype um[2]; // initial velocities in Center of Mass coordinates
	double umt[2]; // initial velocities in Center of Mass coordinates, tangent component
	double umn[2]; // initial velocities in Center of Mass coordinates, normal component
	vectortype v[2]; // final velocities of two particles
	double m[2];	// mass of two particles
	double M; // mass of two particles together
	vectortype V; // velocity of two particles together
	double size;
	int i;

	double xdif = particles[p1].getNextX() - particles[p2].getNextX();
	double ydif = particles[p1].getNextY() - particles[p2].getNextY();

	// set Center of Mass coordinate system
	size = sqrt(xdif*xdif + ydif*ydif);
	xdif /= size; ydif /= size; // normalize
	en.x = xdif;
	en.y = ydif;
	et.x = ydif;
	et.y = -xdif;

	// set u values
	u[0].x = particles[p1].getXVelocity();
	u[0].y = particles[p1].getYVelocity();
	m[0] = particles[p1].getRadius()*particles[p1].getRadius();
	u[1].x = particles[p2].getXVelocity();
	u[1].y = particles[p2].getYVelocity();
	m[1] = particles[p2].getRadius()*particles[p2].getRadius();

	// set M and V
	M = m[0] + m[1];
	V.x = (u[0].x*m[0] + u[1].x*m[1]) / M;
	V.y = (u[0].y*m[0] + u[1].y*m[1]) / M;

	// set um values
	um[0].x = m[1] / M * (u[0].x - u[1].x);
	um[0].y = m[1] / M * (u[0].y - u[1].y);
	um[1].x = m[0] / M * (u[1].x - u[0].x);
	um[1].y = m[0] / M * (u[1].y - u[0].y);

	// set umt and umn values
	for (i = 0; i<2; i++)
	{
		umt[i] = um[i].x * et.x + um[i].y * et.y;
		umn[i] = um[i].x * en.x + um[i].y * en.y;
	}

	// set v values
	for (i = 0; i<2; i++)
	{
		v[i].x = umt[i] * et.x - umn[i] * en.x + V.x;
		v[i].y = umt[i] * et.y - umn[i] * en.y + V.y;
	}

	// reset particle values
	particles[p1].setXVelocity(v[0].x*gFriction);
	particles[p1].setYVelocity(v[0].y*gFriction);
	particles[p2].setXVelocity(v[1].x*gFriction);
	particles[p2].setYVelocity(v[1].y*gFriction);

}

// 
// Functions that draw basic primitives
//
void DrawCircle(double x1, double y1, double radius)
{
	glBegin(GL_POLYGON);
	for(int i=0; i<32; i++)
	{
		double theta = (double)i/32.0 * 2.0 * 3.1415926;
		double x = x1 + radius * cos(theta);
		double y = y1 + radius * sin(theta);
		glVertex2d(x, y);
	}
	glEnd();
}

void DrawRectangle(double x1, double y1, double x2, double y2)
{
	glBegin(GL_QUADS);
	glVertex2d(x1,y1);
	glVertex2d(x2,y1);
	glVertex2d(x2,y2);
	glVertex2d(x1,y2);
	glEnd();
}

void DrawTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
	glBegin(GL_TRIANGLES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glVertex2d(x3,y3);
	glEnd();
}

// Outputs a string of text at the specified location.
void DrawText(double x, double y, char *string)
{
	void *font = GLUT_BITMAP_9_BY_15;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	int len, i;
	glRasterPos2d(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) 
	{
		glutBitmapCharacter(font, string[i]);
	}

	glDisable(GL_BLEND);
}


//
// GLUT callback functions
//

// This callback function gets called by the Glut
// system whenever it decides things need to be redrawn.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (size_t i = 0; i < balls.size(); i++)
	{
		// Get current velocity
		double xVelocity = balls[i].getXVelocity();
		double yVelocity = balls[i].getYVelocity();

		// Add gravity
		yVelocity += gGravity;

		// Add air friction
		yVelocity *= gFriction;
		xVelocity *= gFriction;

		// Handle ball-wall collisions
		double radius = balls[i].getRadius();
		double x = balls[i].getX();
		double y = balls[i].getY();

			// Handle collisions with right wall
		if (x + radius >= screen_x)
			xVelocity = -fabs(xVelocity);

			// Handle collisions with left wall
		if (x - radius <= 0)
			xVelocity = fabs(xVelocity);

			// Handle collisions with ceiling
		if (y + radius >= screen_y)
			yVelocity = -fabs(yVelocity);

			// Handle collisions with floor
		if (y - radius <= 0)
			yVelocity = fabs(yVelocity);

			// Store ball's velocity
		balls[i].setXVelocity(xVelocity);
		balls[i].setYVelocity(yVelocity);

		// Handle ball-ball collisions
		for (size_t j = 0; j < balls.size(); j++)
		{
			if (i != j) {
				if (detectCollision(balls[i], balls[j]))
				{
					Collide(i, j, balls);
				}
			}
		}

		// Have velocities update positions
		x = balls[i].getNextX();
		balls[i].setX(x);
		y = balls[i].getNextY();
		balls[i].setY(y);

		// Draw ball
		std::vector<double> rgb = balls[i].getColor();
		glColor3d(rgb[0], rgb[1], rgb[2]);
		DrawCircle(x, y, radius);
	}
	

	glutSwapBuffers();
	// Forces dislpay to refresh
	glutPostRedisplay();
}


// This callback function gets called by the Glut
// system whenever a key is pressed.
void keyboard(unsigned char c, int x, int y)
{
	switch (c) 
	{
		case 27: // escape character means to quit the program
			exit(0);
			break;
		case 'b':
			// do something when 'b' character is hit.
			break;
		default:
			return; // if we don't care, return without glutPostRedisplay()
	}

	glutPostRedisplay();
}


// This callback function gets called by the Glut
// system whenever the window is resized by the user.
void reshape(int w, int h)
{
	// Reset our global variables to the new width and height.
	screen_x = w;
	screen_y = h;

	// Set the pixel resolution of the final picture (Screen coordinates).
	glViewport(0, 0, w, h);

	// Set the projection mode to 2D orthographic, and set the world coordinates:
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);

}

// This callback function gets called by the Glut
// system whenever any mouse button goes up or down.
void mouse(int mouse_button, int state, int x, int y)
{
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
	}
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
	{
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) 
	{
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) 
	{
	}
	glutPostRedisplay();
}

// Your initialization code goes here.
void InitializeMyStuff()
{
	// Create balls
	for (size_t i = 0; i < gBallCount; i++)
	{
		double radius = randDouble(10.0, 40.0);
		double xVelocity = randDouble(gMinVelocity, gMaxVelocity);
		double yVelocity = randDouble(gMinVelocity, gMaxVelocity);
		double r = randDouble(0, 1.0);
		double g = randDouble(0, 1.0);
		double b = randDouble(0, 1.0);
		
		// Make sure the new ball is not inside another ball
		bool created = false;
		Ball newBall;
		while (!created)
		{
			double x = randDouble(radius, screen_x - radius);
			double y = randDouble(radius, screen_y - radius);
			newBall = Ball(x, y, radius, xVelocity, yVelocity, r, g, b);

			bool collision = false;
			if (balls.size() > 0)
			{
				for (size_t i = 0; i < balls.size(); i++)
				{
					if (detectCollision(newBall, balls[i]))
					{
						collision = true;
						break;
					}
				}
			}
			else
			{
				created = true;
			}
			if (!collision) {
				created = true;
			}
			
		}
		
		balls.push_back(newBall);
	}
}

int main(int argc, char **argv)
{
	srand((unsigned)time(0));
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screen_x, screen_y);
	glutInitWindowPosition(50, 50);

	int fullscreen = 0;
	if (fullscreen) 
	{
		glutGameModeString("800x600:32");
		glutEnterGameMode();
	} 
	else 
	{
		glutCreateWindow("10 Bouncing Balls");
	}

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);

	glColor3d(0,0,0); // forground color
	glClearColor(1, 1, 1, 0); // background color
	InitializeMyStuff();

	glutMainLoop();

	return 0;
}
