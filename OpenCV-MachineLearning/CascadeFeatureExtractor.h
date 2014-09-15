#ifndef CascadeFeatureExtractor_h
#define CascadeFeatureExtractor_h

#include "opencv/cv.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

class CascadeFeatureExtractor
{
private:
	CascadeClassifier cascade;
public:
	vector<Rect> featureWindows;

	CascadeFeatureExtractor(String cascade_dir)
	{
		  if( !cascade.load( cascade_dir ) ){ printf("--(!)Error loading\n"); };
	}

	void Clear()
	{
		featureWindows.clear();
	}

	void ExtractFaces(const Mat& img , Size dim=Size(30, 30))
	{
		featureWindows.clear();
		Mat img_gray;
		cvtColor(img, img_gray, COLOR_BGR2GRAY );
		 equalizeHist( img_gray, img_gray );
		cascade.detectMultiScale(img_gray, featureWindows, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, dim);
	}
};

#endif