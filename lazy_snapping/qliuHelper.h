/* This doc cotains some static classes defining some helper functions used for output or debugging*/
#ifndef QLIUHELPER_LAKALLALA
#define QLIUHELPER_LAKALLALA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include <utils/utils.h>
#include <iostream>

#define Max_Depth 4000
#define Min_Depth 0
#define MAX_PATH 256

using namespace cv;


class outputHelper
{

    public:
        static outputHelper output;
        char outputPath[MAX_PATH];
        char fileName[MAX_PATH];
		int frameIdx;
	Mat imageFrame;

    private:
        outputHelper();
        
    public:
        int markOnImage(const vector<vector<Point> > &markPoints, const Scalar &color, int thickness);
	int markOnImage(Mat &img, const vector<vector<Point> > &markPoints, const Scalar &color, int thickness);
	int markByBlendMask(Mat &mask, const Scalar &alpha);
        int markByBlendMask(Mat &img, Mat &mask, const Scalar &alpha);
	int outputImage();
        int outputImage(Mat &img);
	int outputImage(char fileName[]);
        int outputImage(Mat &img, char fileName[]);
        virtual ~outputHelper();
}; 

class imgProcHelper
{
	private:
		imgProcHelper();
	public:
		static void depthToGrey( const Mat & depth, Mat & grey);
		static void rgbToInt( Mat & img, unsigned int* &	ubuff);
		static void intToRgb( const unsigned int* ubuff, Mat & img, int rows, int cols);
		virtual ~imgProcHelper();
};

#endif
