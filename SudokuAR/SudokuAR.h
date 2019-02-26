/**
	SudokuAR.h
	Purpose:	* Defines the SudokuAR class to detect a Sodoku grid
				and extract the 81 subimages that define it.

	@author Pablo Rodriguez Palafox
	@version 1.0 02/07/18
*/

#pragma once

#ifndef SudokuAR_H_
#define SudokuAR_H_

#include "stdafx.h"

#include <iomanip>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "PoseEstimation.h"

#define DELIMITERS 6

#define UNASSIGNED 0
#define N 9
#define NN 81

typedef struct
{
	int length;
	int nStop;
	int nStart;
	cv::Point2f vecX; // direction vector of the stripe's width
	cv::Point2f vecY; // direction vector of the stripe's length
} MyStripe;

class SudokuAR
{
public:
	SudokuAR(double markerSize);
	~SudokuAR();

	bool processNextFrame(cv::Mat &img_bgr, float resultMatrix[16], bool* sudokuIsSolved);

private:
	char key;
	bool m_playVideo;
	bool m_isFirstStripe;
	bool m_isFirstMarker;

	double m_sudokuSize;
	bool m_saveSubimages;
	bool m_printSolution;
	bool m_grayFlag;

	int m_differenceRow[NN];
	int m_distanceToSudokuInCm;

	////////////////////////////////////////////////////////////////////////
	
	cv::Mat m_src, m_gray, m_threshold, m_sudoku, m_dst, img_bgr;
	cv::Mat m_subimages[81];

	std::vector<std::vector<cv::Point>> m_contours; // Vector for storing contour
	std::vector<cv::Vec4i> m_hierarchy;
	std::vector<cv::Point> m_approx;
	std::vector<cv::Point> m_approxChild;
	cv::Point* m_square;

	cv::Rect m_boundingBox;
	cv::Point *m_sudokuCorners;
	cv::Point2f m_exactSudokuCorners[4];

	static const cv::Scalar numbersColor;

	static const int numberOfSides;
	static const int nOfIntervals;

	MyStripe m_stripe;
	cv::Mat m_iplStripe;

	// We will have 4 lines, one for each side of the square
	// And each line is defined by 4 floats: 2 contain the direction, 2 a point
	float m_lineParams[4 * 4];
	cv::Mat m_lineParamsMat;
	
	////////////////////////////////////////////////////////////////////////

	static void onBlockSizeSlider(int, void*);
	
	void orderCorners();
	cv::Point* findSudoku();
	bool perspectiveTransform(cv::Point2f* corners, cv::Mat& projMatInv);
	void reprojectSolution(const cv::Mat& projMatInv, cv::Mat& img_bgr);
	void extractSubimagesAndSaveToFolder(bool saveSubimages);
	bool solve();
	void drawSolution();
	void drawNumber(int number, unsigned row, unsigned col);
	cv::Mat fineCropGray(const cv::Mat& img);
	cv::Mat fineCropBinary(const cv::Mat& img);
	void estimateSudokuPose(float resultMatrix[16]); // CHANGED
	void processCorners();
	void computeStripe(double dx, double dy);
	cv::Point2f computeAccurateDelimiter(cv::Point p);
	int subpixSampleSafe(const cv::Mat& gray, const cv::Point2f& p);
	cv::Point2f computeSobel(cv::Point p);
	
	////////////////////////////////////////////////////////////////////////

	static const std::string resultsWindow;
	static const std::string sudokuWindow;
	static const std::string trackbarsWindow;

	static const std::string thresholdTrackbarName;
	static const std::string blockSizeTrackbarName;
	static const std::string constTrackbarName;

	static const int blockSizeSliderMax;
	int m_blockSizeSlider;

	static const int constSliderMax;
	int m_constSlider;

	static const int markerThresholdSliderMax;
	int m_markerThresholdSlider;

	static const std::string minAreaTrackbarName;
	static const std::string maxAreaTrackbarName;
	static const int MAX_AREA;
	int m_minArea;
	int m_maxArea;

	static const int MIN_NUM_OF_BOXES;
	
	int m_maxWidth;
	int m_maxHeight;
};

#endif // !SudokuAR_H_
