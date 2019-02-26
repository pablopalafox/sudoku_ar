
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <math.h>

/* PI */
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

void drawSphere(double r, int lats, int longs) {
	int i, j;
	for (i = 0; i <= lats; i++) { // horizontal slices
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
		double z0 = r * sin(lat0);
		double zr0 = r * cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / lats);
		double z1 = r * sin(lat1);
		double zr1 = r * cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++) { // vertical slices
			double lng = 2 * M_PI * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}
}

void drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
{

	// draw the upper part of the cone
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, height);
	for (int angle = 0; angle < 360; angle++) {
		glVertex3f(sin((double)angle) * base, cos((double)angle) * base, 0.f);
	}
	glEnd();

	// draw the base of the cone
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (int angle = 0; angle < 360; angle++) {
		// normal is just pointing down
		glNormal3f(0, -1, 0);
		glVertex3f(sin((double)angle) * base, cos((double)angle) * base, 0.f);
	}
	glEnd();
	/*IF GLU IS INCLUDED
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);

	gluCylinder(quadric, base, (GLdouble)0.0, height, slices, stacks);
	const GLdouble innerradius = 0.0;
	gluDisk(quadric, innerradius, base, slices, stacks);

	gluDeleteQuadric(quadric);
	*/
}

//We make a function for OPENGL to draw a cube
void drawCube(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength) {

	GLfloat halfSideLength = edgeLength * 0.5f;

	GLfloat vertices[] = //There needs to be 24 vertices
	{
		// front face
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top right
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom right
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

		// back face
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top left
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom left

		// left face
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

		// right face
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

		// top face
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // bottom right
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // bottom left

		// top face
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // top left
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // top right
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength // bottom left

	};

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor4f(1.0, 0.5, 0.0, 1.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	glDrawArrays(GL_QUADS, 0, 24);

	glDisableClientState(GL_VERTEX_ARRAY);
}