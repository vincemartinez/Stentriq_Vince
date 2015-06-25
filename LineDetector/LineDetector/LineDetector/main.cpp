/**
* @file HoughLines_Demo.cpp
* @brief Demo code for Hough Transform
* @author OpenCV team
*/

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

/// Global variables

/** General variables */
Mat src, edges;
Mat src_gray;
Mat standard_hough, probabilistic_hough;
int min_threshold = 50;
int max_trackbar = 150;

const char* standard_name = "Standard Hough Lines Demo";
const char* probabilistic_name = "Probabilistic Hough Lines Demo";

int s_trackbar = max_trackbar;
int p_trackbar = max_trackbar;

/// Function Headers
void help();
void Standard_Hough(int, void*);
void Probabilistic_Hough(int, void*);

/**
* @function main
*/
int main(int argc, char** argv)
{
	const char* filename = argc >= 2 ? argv[1] : "pipeline2.jpg";
	/// Read the image
	src = imread(filename, 1);

	if (src.empty())
	{
		help();
		return -1;
	}

	/// Pass the image to gray
	cvtColor(src, src_gray, COLOR_RGB2GRAY);

	/// Apply Canny edge detector
	Canny(src_gray, edges, 50, 200, 3);

	/// Create Trackbars for Thresholds
	char thresh_label[50];
	sprintf(thresh_label, "Thres: %d + input", min_threshold);

	namedWindow(standard_name, WINDOW_AUTOSIZE);
	createTrackbar(thresh_label, standard_name, &s_trackbar, max_trackbar, Standard_Hough);

	namedWindow(probabilistic_name, WINDOW_AUTOSIZE);
	createTrackbar(thresh_label, probabilistic_name, &p_trackbar, max_trackbar, Probabilistic_Hough);

	/// Initialize
	Standard_Hough(0, 0);
	Probabilistic_Hough(0, 0);
	waitKey(0);
	return 0;
}

/**
* @function help
* @brief Indications of how to run this program and why is it for
*/
void help()
{
	printf("\t Hough Transform to detect lines \n ");
	printf("\t---------------------------------\n ");
	printf(" Usage: ./HoughLines_Demo <image_name> \n");
}

/**
* @function Standard_Hough
*/
void Standard_Hough(int, void*)
{
	vector<Vec2f> s_lines;
	cvtColor(edges, standard_hough, COLOR_GRAY2BGR);

	/// 1. Use Standard Hough Transform
	HoughLines(edges, s_lines, 1, CV_PI / 180, min_threshold + s_trackbar, 0, 0);

	/// Show the result
	for (size_t i = 0; i < s_lines.size(); i++)
	{
		float r = s_lines[i][0], t = s_lines[i][1];
		double cos_t = cos(t), sin_t = sin(t);
		double x0 = r*cos_t, y0 = r*sin_t;
		double alpha = 1000;

		Point pt1(cvRound(x0 + alpha*(-sin_t)), cvRound(y0 + alpha*cos_t));
		Point pt2(cvRound(x0 - alpha*(-sin_t)), cvRound(y0 - alpha*cos_t));
		line(standard_hough, pt1, pt2, Scalar(255, 0, 0), 3, LINE_AA);
	}

	imshow(standard_name, standard_hough);
}

/**
* @function Probabilistic_Hough
*/
void Probabilistic_Hough(int, void*)
{
	vector<Vec4i> p_lines;
	cvtColor(edges, probabilistic_hough, COLOR_GRAY2BGR);

	/// 2. Use Probabilistic Hough Transform
	HoughLinesP(edges, p_lines, 1, CV_PI / 180, min_threshold + p_trackbar, 30, 10);

	/// Show the result
	for (size_t i = 0; i < p_lines.size(); i++)
	{
		Vec4i l = p_lines[i];
		line(probabilistic_hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
	}

	
	//compare all lines to one another for matching slope
	for (size_t j = 0; j < p_lines.size()-1; j++){

		for (size_t k = j+1; k < p_lines.size(); k++){

			Vec4i mj = p_lines[j];    //line 1
			Vec4i mk = p_lines[k];    //line 2
			Point pj1(mj[0], mj[1]);  //point 1 line 1
			Point pj2(mj[2], mj[3]);  //point 2 line 1
			Point pk1(mk[0], mk[1]);  //point 1 line 2
			Point pk2(mk[2], mk[3]);  //point 2 line 2
			
			if (abs((((double)(mj[3] - mj[1]) / (double)(mj[2] - mj[0])) / ((double)(mk[3] - mk[1]) / (double)(mk[2] - mk[0]))) - 1) < .1){
				//if the ratio of the slopes of the two lines differs from 1 by less than .1 (if the lines are of almost equal slope)
				double l1_xcenter = (double)((mj[2] - mj[0]) / 2)+mj[0];                //x value of line 1 midpoint
				double l1_ycenter = (double)((mj[3] - mj[1]) / 2)+mj[1];                //y value of line 1 midpoint
				double l2_xcenter = (double)((mk[2] - mk[0]) / 2) + mk[0];              //x value of line 2 midpoint
				double l2_ycenter = (double)((mk[3] - mk[1]) / 2) + mk[1];              //y value of line 2 midpoint
				double average_xcenter = ((l2_xcenter - l1_xcenter) / 2) + l1_xcenter;  //x value of midpoint of both lines
				double average_ycenter = ((l2_ycenter - l1_ycenter) / 2) + l1_ycenter;  //y value of midpoint of both lines

				Point line_location(average_xcenter,average_ycenter);
				//create a point in between the two lines to indicate the location of their center
				circle(probabilistic_hough, line_location, 1, Scalar(0, 255, 0), 3, 8, 0);
				//draw a green circle centered at the center location
			}

		}

	}
	

	imshow(probabilistic_name, probabilistic_hough);
}