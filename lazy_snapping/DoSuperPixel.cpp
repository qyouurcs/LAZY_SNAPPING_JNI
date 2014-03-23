#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;
#include "slic.h"
#include "DoSuperPixel.h"
#include "qliuHelper.h"

void rgbToInt( Mat & img, unsigned int*&	ubuff)
{
	ubuff = new unsigned int[img.rows*img.cols];
	
	MatIterator_<Vec4b> it, end;
    int itBuff;
	
	for(it = img.begin<Vec4b>(), end=img.end<Vec4b>(), itBuff = 0; it!=end; it++, itBuff++)
	{
		ubuff[itBuff] = (*it)[2]*256*256 + (*it)[1]*256 + (*it)[0];	
	}
}

void intToRgb( const unsigned int* ubuff, Mat & img, int rows, int cols)
{
	img = Mat(rows, cols, CV_8UC3);
	MatIterator_<Vec3b> it, end;
    int itBuff;
	
	for(it = img.begin<Vec3b>(), end=img.end<Vec3b>(), itBuff = 0; it!=end; it++, itBuff++)
	{
		(*it)[2] = (ubuff[itBuff] >> 16) & 0xFF;
		(*it)[1] = (ubuff[itBuff] >>  8) & 0xFF;
		(*it)[0] = (ubuff[itBuff]      ) & 0xFF;
	}
}

int* DoSuperPixel(Mat & image, int & num_of_labels)
{
	if(! image.data )
        return NULL;
	unsigned int* ubuff;
	int* labels;
	rgbToInt( image, ubuff);
	SLIC slic;
	//20 , 1 for last two parameters.
	//slic.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels
	slic.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(ubuff, image.cols, image.rows,labels, num_of_labels,NUM_OF_SP,0,20,1);
	slic.DrawContoursAroundSegments(ubuff,labels, image.cols, image.rows, 0);
	//intToRgb(ubuff, image, image.rows, image.cols);
	Mat tmp;
	intToRgb(ubuff, tmp, image.rows, image.cols);
	
	outputHelper::output.outputImage(tmp,"Test.jpg");
	delete[] ubuff;
	return labels;
}

/*
int main( int argc, char** argv )
{
    if( argc != 2)
    {
		 cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
		 return -1;
    }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

	unsigned int* ubuff;

	int* labels;
	rgbToInt( image, ubuff);

	SLIC slic;
	int num_of_labels;
	//20 , 1 for last two parameters.
	//slic.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels
	slic.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(ubuff, image.cols, image.rows,labels, num_of_labels,400,0,20,1);
	//slic.DrawContoursAroundSegments(ubuff,labels, image.cols, image.rows, 0);
	intToRgb(ubuff, image, image.rows, image.cols);
	
	delete[] ubuff;
	delete[] labels;

    return 0;
}
*/