#ifndef EyeIrisDetector_h
#define EyeIrisDetector_h

#include "opencv/cv.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <vector>

#include "CascadeFeatureExtractor.h"

using namespace std;
using namespace cv;

class EyeIrisDetector
{
private:
	vector<Rect>faces;
	
public:

	EyeIrisDetector()
	{
	
	}

	vector< pair<Point2d,Point2d> >iris;

	void ExtractIris(Mat &img)
	{
		getFaces(img); //get face only in the image using viola and jones cascade classifier
		cropUpper();
		iris.clear();
		for(int i = 0 ; i<faces.size() ; i++)
			iris.push_back(getTwoIris(faces[i]));	
	}



private:

	void getFaces(Mat &img)
	{
		CascadeFeatureExtractor faceCascade("D:\\Osama\\Programming\\OpenCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
		faceCascade.ExtractFaces(img);
		for(int i = 0 ; i<faceCascade.featureWindows.size() ;i++)
			faces.push_back(faceCascade.featureWindows[i]);
	}
	
	void cropUpper()
	{
		for(int i = 0 ; i<faces.size() ; i++)
			faces[i].height = faces[i].height/2;
	}



	bool validWindow(const Rect & crop_window)
	{

	}

	pair<Point2d,Point2d> getTwoIris(const Rect &face)
	{
		
		
	}

	double calculateScore(const Rect & crop_window)
	{
	
	}

	double entropyScore(const Rect & crop_window)
	{
	
	}
	double entropy(double x)
	{
	
	}

	double irisDarknessScore(const Rect & crop_window)
	{
	
	
	}

};

#endif