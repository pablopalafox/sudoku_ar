/**
	main.cpp
	Purpose:    * Starts the video feed (OpenCV) and initializes GLFW
				* Creates an object of the class SudokuAR, uses it
				to detect, solve and reproject the solved sudoku onto 
				the input image
				* Draws 4 balls at every corner of the Sudoku (OpenGL)

	@authors	Yoonhee
				Gonzalo Munilla Garrido
				Jose
				Pablo Rodriguez Palafox
	@version 1.0 02/07/18
*/

#include "stdafx.h"

//#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <iomanip>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#include "DrawPrimitives.h"
#include "PoseEstimation.h"
#include "SudokuAR.h"

using namespace std;
cv::VideoCapture cap;

const double sudokuSize = 0.185; // [m]

cv::Mat image;

const int camera_height = 480;
const int camera_width = 640;
const int final_height = camera_height * 2;
const int final_width = camera_width * 2;
int virtual_camera_angle = 30;
unsigned char bkgnd[camera_width*camera_height * 3];

bool sudokuIsSolved = false; // variable to draw the bouncing balls or not

struct Position { 
	double x, y, z; 
	Position(double x_, double y_, double z_) {
		x = x_;
		y = y_;
		z = z_;
	}
};
Position ballpos = Position(0.0, 0.0, 0.0); 
int ballspeed = 30;

/* rotation angle for the cube. */
float rcube = 0.0f;

/* Texture */
GLuint texid;
GLuint texwidth = 2;
GLuint texheight = 2;
// GLuint texdepth=1;

// change texData with sudoku board
GLubyte texData[16] =
{
	0xFF,0x00,0x00,0xFF, // red
	0x00,0xFF,0x00,0xFF, // rgreen
	0x00,0x00,0xFF,0xFF, // blue
	0xFF,0xFF,0xFF,0xFF  // black
};

void InitTexture()
{
	// Bind the texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);     //Sélectionne ce n°
											 /*
											 glTexImage2D (
											 GL_TEXTURE_2D,     //Type : texture 2D
											 0,     //Mipmap : aucun
											 4,     //Couleurs : 4 (red,green,blue,alpha)
											 texwidth,     //Largeur : 2
											 texheight,     //Hauteur : 2
											 0,     //Largeur du bord : 0
											 GL_RGBA,     //Format : RGBA
											 GL_UNSIGNED_BYTE,     //Type des couleurs
											 texData     //Addresse de l'image
											 );
											 */
											 //cv::transpose(image,image);

	//cv::resize(image, image, cv::Size(200, 200));
	//cv::imshow("feed", image);
	glTexImage2D(
		GL_TEXTURE_2D,     //Type : texture 2D
		0,     //Mipmap : aucun
		GL_RGB,     // Colors : 4 (red,green,blue,alpha)
		image.rows,     // width
		image.cols,     // height
		0,     // border
		GL_BGR_EXT,     //Format : RGBA
		GL_UNSIGNED_BYTE,     //Type des couleurs
		image.data     // address of the image
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void DrawCube()
{
	glColor3f(1.0f, 1.0f, 1.0f);

	// draw a cube (6 quadrilaterals)
	glBegin(GL_QUADS);                // start drawing the cube.

	GLfloat cubeheight = 0.2f;
	// Front Face
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -cubeheight, 1.0f);    // Bottom Left Of The Texture and Quad
	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(1.0f, -cubeheight, 1.0f);    // Bottom Right Of The Texture and Quad
	glTexCoord3f(1.0f, cubeheight, 1.0f); glVertex3f(1.0f, cubeheight, 1.0f);    // Top Right Of The Texture and Quad
	glTexCoord3f(0.0f, cubeheight, 1.0f); glVertex3f(-1.0f, cubeheight, 1.0f);    // Top Left Of The Texture and Quad

																				  // Back Face
	glTexCoord3f(0.0f, 0.0f, 0.0f); glVertex3f(-1.0f, -cubeheight, -1.0f);    // Bottom Right Of The Texture and Quad
	glTexCoord3f(0.0f, cubeheight, 0.0f); glVertex3f(-1.0f, cubeheight, -1.0f);    // Top Right Of The Texture and Quad
	glTexCoord3f(1.0f, cubeheight, 0.0f); glVertex3f(1.0f, cubeheight, -1.0f);    // Top Left Of The Texture and Quad
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, -cubeheight, -1.0f);    // Bottom Left Of The Texture and Quad

																			 // Top Face
	glTexCoord3f(0.0f, cubeheight, 0.0f); glVertex3f(-1.0f, cubeheight, -1.0f);    // Top Left Of The Texture and Quad
	glTexCoord3f(0.0f, cubeheight, 1.0f); glVertex3f(-1.0f, cubeheight, 1.0f);    // Bottom Left Of The Texture and Quad
	glTexCoord3f(1.0f, cubeheight, 1.0f); glVertex3f(1.0f, cubeheight, 1.0f);    // Bottom Right Of The Texture and Quad
	glTexCoord3f(1.0f, cubeheight, 0.0f); glVertex3f(1.0f, cubeheight, -1.0f);    // Top Right Of The Texture and Quad

																				  // Bottom Face
	glTexCoord3f(0.0f, 0.0f, 0.0f); glVertex3f(-1.0f, -cubeheight, -1.0f);    // Top Right Of The Texture and Quad
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, -cubeheight, -1.0f);    // Top Left Of The Texture and Quad
	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(1.0f, -cubeheight, 1.0f);    // Bottom Left Of The Texture and Quad
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -cubeheight, 1.0f);    // Bottom Right Of The Texture and Quad

																			 // Right face
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, -cubeheight, -1.0f);    // Bottom Right Of The Texture and Quad
	glTexCoord3f(1.0f, cubeheight, 0.0f); glVertex3f(1.0f, cubeheight, -1.0f);    // Top Right Of The Texture and Quad
	glTexCoord3f(1.0f, cubeheight, 1.0f); glVertex3f(1.0f, cubeheight, 1.0f);    // Top Left Of The Texture and Quad
	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(1.0f, -cubeheight, 1.0f);    // Bottom Left Of The Texture and Quad

																			// Left Face
	glTexCoord3f(0.0f, 0.0f, 0.0f); glVertex3f(-1.0f, -cubeheight, -1.0f);    // Bottom Left Of The Texture and Quad
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -cubeheight, 1.0f);    // Bottom Right Of The Texture and Quad
	glTexCoord3f(0.0f, cubeheight, 1.0f); glVertex3f(-1.0f, cubeheight, 1.0f);    // Top Right Of The Texture and Quad
	glTexCoord3f(0.0f, cubeheight, 0.0f); glVertex3f(-1.0f, cubeheight, -1.0f);    // Top Left Of The Texture and Quad

	glEnd();
	// Done Drawing The Cube
}

void moveBall(float mat[16])
{
	float vector[3];
	vector[0] = mat[3] - ballpos.x;
	vector[1] = mat[7] - ballpos.y;
	vector[2] = mat[11] - ballpos.z;

	float minDistToCam = 0.1;
	float distToPaper = sqrt(mat[3] * mat[3] + mat[7] * mat[7] + mat[11] * mat[11]);
	std::cout << "Distance to paper: " << distToPaper << std::endl;
	float distToCam = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
	std::cout << "Distance to camera: " << distToCam << std::endl;
	if (distToCam < minDistToCam)
	{

		if (ballspeed > 0) // here, positive ballspeed should change to negative
			ballspeed *= -1;

	}
	else if (distToCam > distToPaper)
	{

		if (ballspeed < 0) // ball will start bouncing up
			ballspeed *= -1;

	}

	// update only z-direction speed
	// need to check for zero length to avoid zero division
	if (distToCam != 0) {
		ballpos.x += vector[0];
		ballpos.y += vector[1];
		ballpos.z += vector[2] / (ballspeed * distToCam);
	}
	else {
		ballpos.x += vector[0];
		ballpos.y += vector[1];
		ballpos.z += vector[2] / (ballspeed);
	}

}

void initVideoStream(cv::VideoCapture &cap) {
	if (cap.isOpened())
		cap.release();

	cap.open(0); // open the default camera
	//cap.open("video/thick_sudoku_clip.mp4"); // changed
	if (cap.isOpened() == false) {
		std::cout << "No webcam found, using a video file" << std::endl;
		cap.open("video/thick_sudoku_clip.mp4"); // changed
		//sudokuAR.initVideoStream("video/thick_sudoku.mov");
		if (cap.isOpened() == false) {
			std::cout << "No video file found. Exiting." << std::endl;
			exit(0);
		}
	}

}

/* program & OpenGL initialization */
void initGL()
{
	// initialize the GL library

	// Added in Exercise 8 - End *****************************************************************
	// pixel storage/packing stuff
	glPixelStorei(GL_PACK_ALIGNMENT, 1); // for glReadPixels​
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // for glTexImage2D​
	glPixelZoom(1.0, -1.0);
	// Added in Exercise 8 - End *****************************************************************

	// enable and set colors
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0, 0, 0, 1.0);

	// enable and set depth parameters
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	// light parameters
	GLfloat light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_amb[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light_dif[] = { 0.7, 0.7, 0.7, 1.0 };

	// enable lighting
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


}

void reshape(GLFWwindow* window, int width, int height) {

	// set a whole-window viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// create a perspective projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Note: Just setting the Perspective is an easy hack. In fact, the camera should be calibrated.
	// With such a calibration we would get the projection matrix. This matrix could then be loaded 
	// to GL_PROJECTION.
	// If you are using another camera (which you'll do in most cases), you'll have to adjust the FOV
	// value. How? Fiddle around: Move Marker to edge of display and check if you have to increase or 
	// decrease.
	//gluPerspective( virtual_camera_angle, ((GLfloat)width/(GLfloat)height), 0.01, 100 );
	float ratio = (GLfloat)width / (GLfloat)height;

	float near = 0.01f, far = 100.f;
	float top = tan((double)(virtual_camera_angle*M_PI / 360.0f)) * near;
	float bottom = -top;
	float left = ratio * bottom;
	float right = ratio * top;
	glFrustum(left, right, bottom, top, near, far);


	// invalidate display
	//glutPostRedisplay();
}

void display(GLFWwindow* window, const cv::Mat &img_bgr, float resultMatrix[16])
{
	// Added in Exercise 8 - Start *****************************************************************
	//cv::resize(img_bgr, img_bgr, cv::Size(final_width, final_height));
	memcpy(bkgnd, img_bgr.data, sizeof(bkgnd));
	// Added in Exercise 8 - End *****************************************************************

	int width0, height0;
	glfwGetFramebufferSize(window, &width0, &height0);
	reshape(window, width0, height0);

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Added in Exercise 8 - Start *****************************************************************
	// draw background image
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, camera_width, 0.0, camera_height, -1, 1);
	glRasterPos2i(0, camera_height - 1);
	glDrawPixels(camera_width, camera_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bkgnd);

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	// Added in Exercise 8 - End *****************************************************************

	// move to marker-position
	glMatrixMode(GL_MODELVIEW);

	float resultTransposedMatrix[16];
	for (int x = 0; x<4; ++x)
	{
		for (int y = 0; y<4; ++y)
		{
			resultTransposedMatrix[x * 4 + y] = resultMatrix[y * 4 + x];
		}
	}

	glLoadMatrixf(resultTransposedMatrix);
	//////////////////////////////////////////
	// Set the axis in the following way:   // 
	//		* x: looking right              //
	//		* y: looking upwards            //
	//		* z: looking out of the screen  //
	//////////////////////////////////////////
	glRotatef(-180, 1, 0, 0); // asdfasdfasdf
	glRotatef(90, 0, 0, 1); // asdfasdfasdf
	//glScalef(0.03, 0.03, 0.03);

	//GLfloat scalefactor = 0.03;
	//glTranslatef(0.1, -0.1, 0);
	////glRotatef(-90, 1, 0, 0);
	//glScalef(scalefactor, scalefactor, scalefactor);
	////drawSphere(10, 10, 10);

	//float x = resultMatrix[3];
	//float y = resultMatrix[7];
	//float z = resultMatrix[11];
	//float m_distanceToSudokuInCm = (int)(sqrt(x * x + y * y + z * z)); // we print it in cm

	////InitTexture();
	////DrawCube();

	if (sudokuIsSolved) {
		moveBall(resultMatrix);

		////drawBall
		//glTranslatef(ballpos.x, ballpos.y + 0.024, ballpos.z);
		//glColor4f(1, 0, 0, 1);
		//drawSphere(0.005, 10, 10);

		double scale = 0.7;

		// Translate
		glTranslatef( - sudokuSize * scale / 2.0 + ballpos.x, sudokuSize * scale / 2.0 + ballpos.y, ballpos.z);

		// Morada
		glColor4f(1.0, 0.0, 1.0, 1.0);
		drawSphere(0.005, 10, 10);

		// Translate
		glTranslatef(0.0, - sudokuSize * scale, 0.0);

		// Blanca
		glColor4f(1.0, 0.0, 1.0, 1.0);
		drawSphere(0.005, 10, 10);

		// Translate
		glTranslatef(sudokuSize * scale, 0.0, 0.0);

		// Blanca
		glColor4f(1.0, 0.0, 1.0, 1.0);
		drawSphere(0.005, 10, 10);

		// Translate
		glTranslatef(0.0, sudokuSize * scale, 0.0);

		// Blanca
		glColor4f(1.0, 0.0, 1.0, 1.0);
		drawSphere(0.005, 10, 10);
	}



	//// draw 3 white spheres
	/*glColor4f(1.0, 1.0, 1.0, 1.0);
	drawSphere(0.8, 10, 10);*/
	//glTranslatef(0.0, 0.8, 0.0);
	//drawSphere(0.6, 10, 10);
	//glTranslatef(0.0, 0.6, 0.0);
	//drawSphere(0.4, 10, 10);

	//// draw the eyes
	//glPushMatrix();
	//glColor4f(0.0, 0.0, 0.0, 1.0);
	//glTranslatef(0.2, 0.2, 0.2);
	//drawSphere(0.066, 10, 10);
	//glTranslatef(0, 0, -0.4);
	//drawSphere(0.066, 10, 10);
	//glPopMatrix();

	//// draw a nose
	//glColor4f(1.0, 0.5, 0.0, 1.0);
	//glTranslatef(0.3, 0.0, 0.0);
	//glRotatef(90, 0, 1, 0);
	//drawCone(0.1, 0.3, 10, 10);
}



int main()
{
	//PyObject* pInt;

	//Py_Initialize();

	//PyRun_SimpleString("print('Hello World from Embedded Python!!!')");

	//Py_Finalize();

	
	
	/*system('python ')
	return -1;*/



	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;
	

	// setup OpenCV
	cv::Mat img_bgr;
	initVideoStream(cap);
	
	SudokuAR sudokuAR(sudokuSize);

	// initialize the window system
	/* Create a windowed mode window and its OpenGL context */
	//window = glfwCreateWindow(camera_width, camera_height, "Exercise 8 - Combine", NULL, NULL);
	
	window = glfwCreateWindow(camera_width, camera_height, "Exercise 8 - Combine", NULL, NULL);

	glfwSetWindowPos(window, 1900, 750);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set callback functions for GLFW
	glfwSetFramebufferSizeCallback(window, reshape);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	int window_width, window_height;
	glfwGetFramebufferSize(window, &window_width, &window_height);
	reshape(window, window_width, window_height);

	glViewport(0, 0, window_width, window_height);

	// initialize the GL library
	initGL();

	float resultMatrix[16];
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Capture here */
		cap >> img_bgr;
		// added by YH
		cap >> image;

		if (img_bgr.empty()) {
			std::cout << "Could not query frame. Trying to reinitialize." << std::endl;
			initVideoStream(cap);
			cv::waitKey(1000); /// Wait for one sec.
			continue;
		}

		/* Track a marker */
		if (!sudokuAR.processNextFrame(img_bgr, resultMatrix, &sudokuIsSolved)) {
			return 0;
		}

		/* Render here */
		display(window, img_bgr, resultMatrix);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();


	return 0;
}