#ifndef EyeIrisDetector_h
#define EyeIrisDetector_h

#include "opencv/cv.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <vector>
#include <queue>
#include <stdexcept>

#include "CascadeFeatureExtractor.h"

using namespace std;
using namespace cv;

class EyeIrisDetector
{
private:
	vector<Rect>faces;
	int colorFreq[256 + 5];
	Mat img;
	
	#define WINDOWS_THRESHOLD 100
	#define WINDOWVSFACERATIO (1.0/7.0)
	#define UPPERCROPRATIO (1.0/3.0)
	#define LOWERCROPRATIO (1.0/4.0)
	#define SIDECROPRATIO (1.0/4.0)
	#define LEFT_EYE_THRESHOLD 0.50
	#define STEPX 1
	#define STEPY 1
	#define DELTAX 3
	#define DELTAY 1

	struct Window
	{
		Rect rect;
		double score;
		int intensity_sum;
		Window()
		{
		score=0.0;
		intensity_sum=0;
		}
		Window(Rect rect, double entropy_score, int intensity_sum) : rect(rect), score(entropy_score), intensity_sum(intensity_sum)
		{}
		bool operator < (const Window &w)const
		{
			return score<w.score;
		}
	};


public:

	EyeIrisDetector()
	{}

	vector< pair<Point2i,Point2i> >iris;

	void ExtractIris(Mat & _img, bool debugMode = false)
	{
		img = _img;
		faces.clear();
		cvtColor(img, img, CV_BGR2GRAY);
		getFaces(img); //get face only in the image using viola and jones cascade classifier
		cropEyeRegion();
		iris.clear();
		for(int i = 0 ; i<1 && i<faces.size() ; i++)
		{
			Mat croppedFace = img(faces[i]);
			vector<Window> bestWindows = getBestWindows(faces[i],img);
			double total_entropy=0;
			for(int j = 0 ; j<bestWindows.size() ; j++)
				total_entropy += bestWindows[j].score;

			vector<double> darkness_scores(bestWindows.size(),0);

			int total_darkness=0;
			for(int j = 0 ; j<bestWindows.size() ; j++)
			{
				darkness_scores[j] = calcIrisDarknessScore(bestWindows[j].rect,croppedFace);
				total_darkness += darkness_scores[j];
			}

			Window best_left,best_right;

			for(int j = 0 ; j<bestWindows.size() ; j++)
			{
				double Hscore = bestWindows[j].score/total_entropy;
				double Cscore = 1.0 - ((double)darkness_scores[j]/(double)total_darkness);
				double Tscore = Hscore + Cscore;
				// left eye window
				if(bestWindows[j].rect.x + bestWindows[j].rect.width  <= (int)((double)faces[i].width * LEFT_EYE_THRESHOLD))
				{
					if(best_left.score<Tscore)
					{
						best_left.score=Tscore;
						best_left.rect=bestWindows[j].rect;
					}
				}
				// right eye window
				else if(bestWindows[j].rect.x >= (int)((double)faces[i].width * LEFT_EYE_THRESHOLD))
				{
					if(best_right.score<Tscore)
					{
						best_right.score=Tscore;
						best_right.rect=bestWindows[j].rect;
					}

				}
			}

			Mat A = croppedFace(best_left.rect);
			Mat B = croppedFace(best_right.rect);
			
			Point2i left_iris  = faces[i].tl() + best_left.rect.tl() + Point2i(best_left.rect.width/2.0, best_left.rect.height/2.0);
			Point2i right_iris = faces[i].tl() + best_right.rect.tl() + Point2i(best_right.rect.width/2.0, best_right.rect.height/2.0);
			iris.push_back(make_pair(left_iris,right_iris));

			// For Debugging Purposes
			if(debugMode && faces.size())
			{
				rectangle(_img,faces[0],Scalar(255,0,0)); // eye region
				rectangle(_img,Rect(faces[0].x,faces[0].y,faces[0].width*LEFT_EYE_THRESHOLD , faces[0].height),Scalar(255,0,0)); // LEFT_EYE_THRESHOLD
				double d = best_left.rect.width;
				double r = (double)d *(WINDOWVSFACERATIO); // eye radius calc
				circle(_img, left_iris , r, Scalar(255,255,0)); //left eye
				circle(_img, right_iris , r, Scalar(255,255,0)); // right eye
				rectangle(_img, Rect(left_iris.x-best_left.rect.width/2,left_iris.y-best_left.rect.height/2,best_left.rect.size().width,best_left.rect.size().height), Scalar(255,0,0)); // left window
				rectangle(_img, Rect(right_iris.x-best_right.rect.width/2,right_iris.y-best_right.rect.height/2,best_right.rect.size().width,best_right.rect.size().height), Scalar(255,0,0)); // right window
			}

		}


	}



private:

	void getFaces(Mat &img)
	{
		CascadeFeatureExtractor faceCascade("D:\\Osama\\Programming\\OpenCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
		faceCascade.ExtractFaces(img);
		for(int i = 0 ; i<faceCascade.featureWindows.size() ;i++)
			faces.push_back(faceCascade.featureWindows[i]);
	}

	void cropEyeRegion()
	{
		for(int i = 0 ; i<faces.size() ; i++)
		{
			faces[i].height = faces[i].height/2.0;
			faces[i].y += (int)(UPPERCROPRATIO*(double)faces[i].height);
			faces[i].height -= (int)(LOWERCROPRATIO*(double)faces[i].height);
			faces[i].width -= (int)(SIDECROPRATIO*(double)faces[i].width);
			faces[i].x += (int)(SIDECROPRATIO*0.65*(double)faces[i].width);
		}
	}

	vector<Window> getBestWindows(const Rect &face, const Mat &img)
	{
		Mat croppedFace = img(face);
		double d = face.width;
		double r = toRadius(d);
		const int windowWidth = 2*r + DELTAX;
		const int windowHeight = 2*r + DELTAY; 


		priority_queue<Window> bestLeftWindowsPQ;
		priority_queue<Window> bestRightWindowsPQ;

		for(int i = 0 ; i+windowHeight<croppedFace.rows ; i += STEPY)
			for(int j = 0 ; j+windowWidth<croppedFace.cols ; j += STEPX)
			{
				Rect rect = Rect(j,i,windowWidth,windowHeight);
				int intensity_sum=0;
				int splitCol = (int)((double)croppedFace.size().width*LEFT_EYE_THRESHOLD);
				if(rect.x+rect.width <= splitCol)
				{
								double score = calcEntropyScore(rect,croppedFace);
								bestLeftWindowsPQ.push(Window(rect,score,intensity_sum));
								if(bestLeftWindowsPQ.size()>WINDOWS_THRESHOLD)
								bestLeftWindowsPQ.pop();
				}
				else if(rect.x >= splitCol)
				{
								double score = calcEntropyScore(rect,croppedFace);
								bestRightWindowsPQ.push(Window(rect,score,intensity_sum));
								if(bestRightWindowsPQ.size()>WINDOWS_THRESHOLD)
								bestRightWindowsPQ.pop();
				}


			}
			vector<Window> ret;
			while(!bestLeftWindowsPQ.empty())
				ret.push_back(bestLeftWindowsPQ.top()), bestLeftWindowsPQ.pop();
			while(!bestRightWindowsPQ.empty())
				ret.push_back(bestRightWindowsPQ.top()), bestRightWindowsPQ.pop();
			return ret;
	}

	double calcEntropyScore(const Rect & crop_window, const Mat &img)
	{
		memset(colorFreq,0,sizeof(colorFreq));
		Mat interestImg = img(crop_window);
		for(int i=0 ; i<interestImg.rows ; i++)
			for(int j=0 ; j<interestImg.cols ; j++)
				++colorFreq[interestImg.at<uchar>(i,j)];
		double entropyVal = 0;
		for(int i= 0 ; i<256 ; i++)
			entropyVal += calcEntropy(i,crop_window);
		return entropyVal;
	}

	double calcEntropy(int x, const Rect & crop_window)
	{
		double px = (double)colorFreq[x]/(double)(crop_window.width*crop_window.height);
		if(px==0.0)
			return 0.0;
		double ret =  - px * (log(px)/log(2.0));
		return ret;
	}

	int squaredDistance(Point2i from, Point2i to)
	{
		return ((from.x-to.x)*(from.x-to.x) + (from.y-to.y)*(from.y-to.y));
	}

	__inline double toRadius(double faceWidth)
	{
		 return (double)faceWidth *(WINDOWVSFACERATIO)*1.0;
	}

		__inline double toWindow(double radius)
	{
		 return radius*1.5;
	}

	int calcIrisDarknessScore(const Rect &crop_window, const Mat &img)
	{
		double d = crop_window.width;
		double r = toRadius(d);
		int intensitySum=0;
		Point2i center(crop_window.width/2.0 , crop_window.height/2.0);
		Mat interestWindow = img(crop_window);
		for(int i = 0 ; i<interestWindow.rows ; i++)
			for(int j = 0 ; j<interestWindow.cols ; j++)
				if(squaredDistance(center,Point2i(i,j))<= r*r)
					intensitySum += interestWindow.at<uchar>(i,j);
		return intensitySum;
	}

};

#endif