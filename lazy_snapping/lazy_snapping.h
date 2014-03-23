#ifndef _LAZY_SNAPPING_H__
#define _LAZY_SNAPPING_H__

#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include "graph.h"
#include <vector>
#include <iostream>
#include <cmath>
using namespace std;
using namespace cv;

typedef Graph<float,float,float> GraphType;
class LasySnapping{
    
public :
    LasySnapping():graph(NULL){
		for(int i = 0; i < 3; i ++)
		{
			avgForeColor[i] = 0;
			avgBackColor[i] = 0;
		}
        //avgForeColor ={0,0,0};
        // avgBackColor ={0,0,0};
    }
    ~LasySnapping(){ 
        if(graph){
            delete graph;
        }
    };
private :
    vector<CvPoint> forePts;
    vector<CvPoint> backPts;
    IplImage* image;
    // average color of foreground points
    unsigned char avgForeColor[3];
    // average color of background points
    unsigned char avgBackColor[3];
public :
    void setImage(IplImage* image){
        this->image = image;
        graph = new GraphType(image->width*image->height,image->width*image->height*2);
    }
    // include-pen locus
    void setForegroundPoints(vector<CvPoint> pts){
        forePts.clear();
        for(int i =0; i< pts.size(); i++){
            if(!isPtInVector(pts[i],forePts)){
                forePts.push_back(pts[i]);
            }
        }
        if(forePts.size() == 0){
            return;
        }
        int sum[3] = {0};
        for(int i =0; i < forePts.size(); i++){
            unsigned char* p = (unsigned char*)image->imageData + forePts[i].x * 3 
                + forePts[i].y*image->widthStep;
            sum[0] += p[0];
            sum[1] += p[1];
            sum[2] += p[2];            
        }
        cout<<sum[0]<<" " <<forePts.size()<<endl;
        avgForeColor[0] = sum[0]/forePts.size();
        avgForeColor[1] = sum[1]/forePts.size();
        avgForeColor[2] = sum[2]/forePts.size();
    }
    // exclude-pen locus
    void setBackgroundPoints(vector<CvPoint> pts){
        backPts.clear();
        for(int i =0; i< pts.size(); i++){
            if(!isPtInVector(pts[i],backPts)){
                backPts.push_back(pts[i]);
            }
        }
        if(backPts.size() == 0){
            return;
        }
        int sum[3] = {0};
        for(int i =0; i < backPts.size(); i++){
            unsigned char* p = (unsigned char*)image->imageData + backPts[i].x * 3 + 
                backPts[i].y*image->widthStep;
            sum[0] += p[0];
            sum[1] += p[1];
            sum[2] += p[2];            
        }
        avgBackColor[0] = sum[0]/backPts.size();
        avgBackColor[1] = sum[1]/backPts.size();
        avgBackColor[2] = sum[2]/backPts.size();
    }
    // return maxflow of graph
    int runMaxflow();
	int RunMaxFlowSuperPixel(int* super_label, int num_of_labels);
    // get result, a grayscale mast image indicating forground by 255 and background by 0
    IplImage* getImageMask();
	IplImage* getImageMask(int* sp_labels);
private :
    float colorDistance(unsigned char* color1, unsigned char* color2);
    float minDistance(unsigned char* color, vector<CvPoint> points);
    bool isPtInVector(CvPoint pt, vector<CvPoint> points);
    void getE1(unsigned char* color,float* energy);
    float getE2(unsigned char* color1,unsigned char* color2);
    
    GraphType *graph;    
};

#endif