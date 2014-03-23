#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include "graph.h"
#include <vector>
#include <iostream>
#include <cmath>
using namespace std;
using namespace cv;

#include"lazy_snapping.h";
#include"DoSuperPixel.h";


//CvScalar paintColor[2] = {CV_RGB(0,0,255),CV_RGB(255,0,0)};

/*
JNIEXPORT jintArray JNICALL Java_com_example_haveimgfun_LazySnapping_DoLazySnapping
  (JNIEnv *, jclass, jintArray, jintArray, jint, jint);
*/

IplImage * change4channelTo3InIplImage(IplImage * src) {
	if (src->nChannels != 4) {
		return NULL;
	}
	IplImage * destImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	for (int row = 0; row < src->height; row++) {
		for (int col = 0; col < src->width; col++) {
			CvScalar s = cvGet2D(src, row, col);
			cvSet2D(destImg, row, col, s);
		}
	}
	return destImg;
}




int* DoLazySnapping(int* buf, int* bg_mask, int bg_num, int* fg_mask, int fg_num, int w, int h)
{
	vector<CvPoint> local_forePts;
	vector<CvPoint> local_backPts;
	for(int i = 0; i < fg_num; i++)
	{
		local_forePts.push_back(cvPoint(fg_mask[i],fg_mask[i + fg_num]));
	}

	for(int i = 0; i < bg_num; i++)
	{
		local_backPts.push_back(cvPoint(bg_mask[i],bg_mask[i + bg_num]));
	}

	Mat img_mat(h, w, CV_8UC4, (unsigned char*) buf);
	IplImage image= IplImage(img_mat);
	IplImage* image3channel = change4channelTo3InIplImage(&image);
	cvSaveImage("image3channel.bmp", image3channel);


	LasySnapping ls;
	ls.setImage(image3channel);
    ls.setBackgroundPoints(local_backPts);
    ls.setForegroundPoints(local_forePts);
    ls.runMaxflow();
    IplImage* mask = ls.getImageMask();
	cvSaveImage("mask.bmp", mask);
    IplImage* gray = cvCreateImage(cvGetSize(image3channel),8,1);
    cvResize(mask,gray);
  
    cvCanny(gray,gray,50,150,3);
        
	cvSaveImage("gray.bmp", gray);
    IplImage* showImg = cvCloneImage(image3channel);
	cvSaveImage("showImg.bmp", showImg);
    for(int h =0; h < image.height; h ++){
        unsigned char* pgray = (unsigned char*)gray->imageData + gray->widthStep*h;
		unsigned char* pimage = (unsigned char*)showImg->imageData + showImg->widthStep *h;
        for(int width  =0; width < image.width; width++){
            if(*pgray++ != 0 ){
                pimage[0] = 0;
                pimage[1] = 255;
                pimage[2] = 0;
            }
            pimage+=3;                
        }
    }
    cvSaveImage("t.bmp",showImg);
    cvShowImage("Result",showImg);
	cvReleaseImage(&image3channel);
    cvReleaseImage(&mask);
    cvReleaseImage(&showImg);
    cvReleaseImage(&gray);
	return NULL;
}

int* DoLazySnapping(int* buf, int* bg_mask, int bg_num, int* fg_mask, int fg_num, int w, int h, int * sp_labels, int num_of_labels)
{
	
	vector<CvPoint> local_forePts;
	vector<CvPoint> local_backPts;
	for(int i = 0; i < fg_num; i++)
	{
		local_forePts.push_back(cvPoint(fg_mask[i],fg_mask[i + fg_num]));
	}

	for(int i = 0; i < bg_num; i++)
	{
		local_backPts.push_back(cvPoint(bg_mask[i],bg_mask[i + bg_num]));
	}

	Mat img_mat(h, w, CV_8UC4, (unsigned char*) buf);
	if( sp_labels == NULL)
	{
		//first do superpixel, if this is not done yet.
		//sp_labels = DoSuperPixel(img_mat, num_of_labels);
		//sp_labels = DoSuperPixel(img_mat, num_of_labels);
		sp_labels = DoSuperPixel(img_mat, num_of_labels);
	}

	IplImage image= IplImage(img_mat);
	IplImage* image3channel = change4channelTo3InIplImage(&image);
	cvSaveImage("image3channel.bmp", image3channel);


	LasySnapping ls;
	ls.setImage(image3channel);
    ls.setBackgroundPoints(local_backPts);
    ls.setForegroundPoints(local_forePts);
	ls.RunMaxFlowSuperPixel(sp_labels, num_of_labels);
	IplImage* mask = ls.getImageMask(sp_labels);

	cvSaveImage("mask.bmp", mask);
    IplImage* gray = cvCreateImage(cvGetSize(image3channel),8,1);
    cvResize(mask,gray);
  
    cvCanny(gray,gray,50,150,3);
        
	cvSaveImage("gray.bmp", gray);
    IplImage* showImg = cvCloneImage(image3channel);
	cvSaveImage("showImg.bmp", showImg);
    for(int h =0; h < image.height; h ++){
        unsigned char* pgray = (unsigned char*)gray->imageData + gray->widthStep*h;
		unsigned char* pimage = (unsigned char*)showImg->imageData + showImg->widthStep *h;
        for(int width  =0; width < image.width; width++){
            if(*pgray++ != 0 ){
                pimage[0] = 0;
                pimage[1] = 255;
                pimage[2] = 0;
            }
            pimage+=3;                
        }
    }
    cvSaveImage("t.bmp",showImg);
    cvShowImage("Result",showImg);
	cvReleaseImage(&image3channel);
    cvReleaseImage(&mask);
    cvReleaseImage(&showImg);
    cvReleaseImage(&gray);
	return NULL;
}


int main(int argc, char** argv)
{	
    if(argc != 3){
        cout<<"command : lazysnapping inputImage maskImage"<<endl;
        return 0;
    }
	const int SCALE = 1;
	
	IplImage* image_ptr = cvLoadImage(argv[1],CV_LOAD_IMAGE_COLOR);
	
	IplImage* imageLS = cvCreateImage(cvSize(image_ptr->width/SCALE,image_ptr->height/SCALE),
            8,3);

    cvResize(image_ptr,imageLS);
	cvReleaseImage(&image_ptr);

	image_ptr = imageLS;
	int * pixels = new int[image_ptr->width * image_ptr->height];
	

	int i = 0;
	for(int h = 0; h < image_ptr->height; h ++){
        unsigned char* pimage = (unsigned char*)image_ptr->imageData + image_ptr->widthStep*h;
        for(int width  = 0; width < image_ptr->width; width++){
			
            pixels[i] = pimage[0];
			pixels[i] += ((int)pimage[1])<<8;
			pixels[i] += ((int)pimage[2]) << 16;
			i++;
            pimage+=3;                
        }
    }
	vector<CvPoint> fg_vecs;
	vector<CvPoint> bg_vecs;
	IplImage* mask = cvLoadImage(argv[2], CV_LOAD_IMAGE_COLOR);
    imageLS = cvCreateImage(cvSize(mask->width/SCALE,mask->height/SCALE),
            8,3);

    cvResize(mask,imageLS);
	cvReleaseImage(&mask);
	
	mask = imageLS;
	cvSaveImage("mask_resized.bmp",mask);
	for(int h = 0; h < mask->height; h ++){
        unsigned char* pimage = (unsigned char*)mask->imageData + mask->widthStep*h;
        for(int width  = 0; width < mask->width; width++){
			if( pimage[0] < 100) //BGR
				fg_vecs.push_back(cvPoint(width,h));
			else if( pimage[2] < 100)
				bg_vecs.push_back(cvPoint(width,h));
            pimage+=3;                
        }
    }
	int* bg_ptr = new int[bg_vecs.size() * 2];
	for(int i = 0; i < bg_vecs.size(); i++)
	{
		bg_ptr[i] = bg_vecs[i].x;
		bg_ptr[i + bg_vecs.size()] = bg_vecs[i].y;
	}
	
	int *fg_ptr = new int[fg_vecs.size() * 2];
	for(int i = 0; i < fg_vecs.size(); i++)
	{
		fg_ptr[i] = fg_vecs[i].x;
		fg_ptr[i + fg_vecs.size()] = fg_vecs[i].y;
	}

	cvShowImage("Hello", image_ptr);
	
	//DoLazySnapping(pixels,bg_ptr, bg_vecs.size(), fg_ptr, fg_vecs.size(), image_ptr->width, image_ptr->height);
	DoLazySnapping(pixels,bg_ptr, bg_vecs.size(), fg_ptr, fg_vecs.size(), image_ptr->width, image_ptr->height, NULL, 0);
    
	return 0;
}
