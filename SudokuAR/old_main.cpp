// Marker_Tracker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "SudokuAR.h"


/*
A. Detect sudoku grid
A.1 Convert the webcam image in gray color
A.2 Apply gaussian blurring
A.3 Apply adaptative thresholding
Now we have a binary image
A.4 In it, we find linear segments using Hough transform
B.
B.1 From the linear segments image, find the contours (cv::findContours)
B.2 Retain the contours with the biggest area (we assume it corresponds to the sudoku grid)
B.3 Because the detected linear segments are sometimes not well connected,
replace the contour by its approximate encompassing polygon (cv::approxPolyDP)
C.
In the binary "thresholded" image (form step 1) we keep the part of the image delimited
by the found polygon in step 2, and apply to it a perspective transformation,
so that we get a nice stabilized square shaped image of the sudoku
(cv::getPerspectiveTransform, cv::warpPerspective)


*/


int main(int argc, char** argv)
{

	SudokuAR sudokuAR;

	// Ask user whether to act on webcam feed or on saved video
	char c;

	while (!sudokuAR.m_feedInitialized) {

		std::cout << "Enter 'v' to display saved video | Enter 'c' to display webcam feed" << std::endl;
		//std::cin >> c;
		c = 'v';

		if (c == 'v' || c == 'V')
		{
			// Video feed
			//sudokuAR.initVideoStream("video/MarkerMovie.mp4");

			//sudokuAR.initVideoStream("video/large_sudoku.mov");
			//sudokuAR.initVideoStream("video/large_sudoku_clip.mp4");

			sudokuAR.initVideoStream("video/thick_sudoku_clip.mp4");
			//sudokuAR.initVideoStream("video/thick_sudoku.mov");
		}
		else if (c == 'c' || c == 'C')
		{
			// Camera feed
			sudokuAR.initVideoStream();
		}
	}

	while (sudokuAR.processNextFrame()) {}

	return 0;
}

