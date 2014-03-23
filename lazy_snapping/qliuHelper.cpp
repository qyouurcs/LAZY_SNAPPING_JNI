#include <stdio.h>
#include "qliuHelper.h"
#include <stdio.h>
using namespace std;
outputHelper::outputHelper()
{
    _snprintf(this->outputPath, MAX_PATH, "./");
    _snprintf(this->fileName, MAX_PATH, "tempOutput.png");
	frameIdx = 0;
}
outputHelper::~outputHelper(){}

outputHelper outputHelper::output;

int outputHelper::markOnImage(const vector<vector<Point> > &markPoints, const Scalar &color, int thickness)
{
    return (!this->imageFrame.empty())?outputHelper::markOnImage(this->imageFrame, markPoints, color, thickness):-1;
}
int outputHelper::markOnImage(Mat &img, const vector<vector<Point> > &markPoints, const Scalar &color, int thickness)
{
    size_t i = 0;
    for (i = 0; i < markPoints.size(); i++) {
        cv::drawContours(img, markPoints, i, color, thickness, 8);
    }
    return 0;
}

int outputHelper::markByBlendMask(Mat &mask, const Scalar &alpha)
{
    return (!this->imageFrame.empty())?outputHelper::markByBlendMask(this->imageFrame, mask, alpha):-1;
}

int outputHelper::markByBlendMask(Mat &img, Mat &mask, const Scalar &alpha)
{
    int channels = img.channels();
    switch(channels)
    {
	case(3):
	{
    	    MatIterator_<Vec3b> it, end;
    	    MatIterator_<uchar> itMask, endMask;
	    for(it = img.begin<Vec3b>(), end=img.end<Vec3b>(), itMask = mask.begin<uchar>(), endMask=mask.end<uchar>();it!=end, itMask!=endMask; it++, itMask++)
	    {
		if(itMask[0]>0)
		{
			if( alpha[0]<=1 && alpha[1]<=1 && alpha[2]<=1 )
			{
				(*it)[0] = (*it)[0]*alpha[0];
				(*it)[1] = (*it)[1]*alpha[1];
				(*it)[2] = (*it)[2]*alpha[2];
			}
			else
			{
				(*it)[0] = alpha[0];
				(*it)[1] = alpha[1];
				(*it)[2] = alpha[2];
			}
		}
		
	    }
	    break;
	}
	case(1):
	{
    	    MatIterator_<uchar> it, end;
    	    MatIterator_<uchar> itMask, endMask;
	    for(it = img.begin<uchar>(), end=img.end<uchar>(), itMask = mask.begin<uchar>(), endMask=mask.end<uchar>();it!=end, itMask!=endMask; it++, itMask++)
	    {
		if(itMask[0]>0)
		{
			if( alpha[0]<=1)
				it[0] = it[0]*alpha[0];
			else
				it[0] = (uchar) (alpha[0]);
		}
		
	    }
	    break;
	}
    }
    return 0;
}

int outputHelper::outputImage()
{
    return (!this->imageFrame.empty())? outputHelper::outputImage(this->imageFrame): -1;
}

int outputHelper::outputImage(Mat &img)
{
    /*path dir (outputPath);
    path file (fileName);
    path fullPath = dir / file;*/
    char fullPath[MAX_PATH];
    _snprintf(fullPath, MAX_PATH, "%s%s", this->outputPath , this->fileName);
    //std::cout<<fullPath<<std::endl;
    imwrite(fullPath, img);
    return 0;
}

int outputHelper::outputImage(char fileName[])
{
    return (!this->imageFrame.empty())? outputHelper::outputImage(this->imageFrame, fileName): -1;
}

int outputHelper::outputImage(Mat &img, char fileName[])
{
    _snprintf(this->fileName, MAX_PATH, this->fileName);
    outputImage(img);
    return 0;
}



void imgProcHelper::depthToGrey( const Mat & depth, Mat & grey)
{
	Mat temp;
	depth.copyTo(temp);
	grey = Mat(temp.rows, temp.cols, CV_8UC1);
	MatIterator_<unsigned short> it, end;
	MatIterator_<uchar> itGrey, endGrey;
	
	it = temp.begin<unsigned short>();
	end = temp.end<unsigned short>();	
	itGrey = grey.begin<uchar>();
	endGrey=grey.end<uchar>();
	
	for(;it!=end, itGrey!=endGrey; it++, itGrey++)
	{
		(*itGrey) = ((*it) >= Min_Depth && (*it) <= Max_Depth) ? (*it)*255/(Max_Depth - Min_Depth) : 0;
		(*itGrey) = (*it) > Max_Depth ? 255 : (*itGrey);
		 //std::cout << (int) (*itGrey) <<std::endl;
	}
}

//note it is actually bgr, not rgb
void imgProcHelper::rgbToInt( Mat & img, unsigned int*&	ubuff)
{
	ubuff = new unsigned int[img.rows*img.cols];
	
	MatIterator_<Vec3b> it, end;
    int itBuff;
	
	for(it = img.begin<Vec3b>(), end=img.end<Vec3b>(), itBuff = 0; it!=end; it++, itBuff++)
	{
		ubuff[itBuff] = (*it)[2]*256*256 + (*it)[1]*256 + (*it)[0];	
	}
}

//note it is actually bgr, not rgb
void imgProcHelper::intToRgb( const unsigned int* ubuff, Mat & img, int rows, int cols)
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


