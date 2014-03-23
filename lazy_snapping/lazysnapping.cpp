/* author: zhijie Lee
 * home page: lzhj.me
 * 2012-02-06
 */
#include "lazy_snapping.h"
#include<set>
using namespace std;
float LasySnapping::colorDistance(unsigned char* color1, unsigned char* color2)
{
    return sqrt((double)(color1[0]-color2[0])*(color1[0]-color2[0])+
        (color1[1]-color2[1])*(color1[1]-color2[1])+
        (color1[2]-color2[2])*(color1[2]-color2[2]));    
}

float LasySnapping::minDistance(unsigned char* color, vector<CvPoint> points)
{
    float distance = -1;
    for(int i =0 ; i < points.size(); i++){
        unsigned char* p = (unsigned char*)image->imageData + points[i].y * image->widthStep + 
            points[i].x * image->nChannels;
        float d = colorDistance(p,color);
        if(distance < 0 ){
            distance = d;
        }else{
            if(distance > d){
                distance = d;
            }
        }
    }
	return distance;
}
bool LasySnapping::isPtInVector(CvPoint pt, vector<CvPoint> points)
{
    for(int i =0 ; i < points.size(); i++){
        if(pt.x == points[i].x && pt.y == points[i].y){
            return true;
        }
    }
    return false;
}
void LasySnapping::getE1(unsigned char* color,float* energy)
{
    // average distance
    float df = colorDistance(color,avgForeColor);
    float db = colorDistance(color,avgBackColor);
    // min distance from background points and forground points
    // float df = minDistance(color,forePts);
    // float db = minDistance(color,backPts);
    energy[0] = df/(db+df);
    energy[1] = db/(db+df);
}
float LasySnapping::getE2(unsigned char* color1,unsigned char* color2)
{
    const float EPSILON = 0.01;
    float lambda = 100;
    return lambda/(EPSILON+
        (color1[0]-color2[0])*(color1[0]-color2[0])+
        (color1[1]-color2[1])*(color1[1]-color2[1])+
        (color1[2]-color2[2])*(color1[2]-color2[2]));
}

int LasySnapping::RunMaxFlowSuperPixel(int* super_label, int num_of_labels)// THis 
{
	// We now have different ways of constructing the graph.

	// Construct the new fg and bg vectors.
	set<int> fg_labels;
	for(int i = 0; i < forePts.size(); i++)
	{
		CvPoint a = forePts[i];
		fg_labels.insert(super_label[a.x + a.y * image->width]);
	}
	set<int> bg_labels;
	for(int i = 0; i < backPts.size(); i++)
	{
		CvPoint a = backPts[i];
		bg_labels.insert(super_label[a.x + a.y * image->width]);
	}

	const float INFINNITE_MAX = 1e10;
    int indexPt = 0;
	//scan the image to find out the connected super pixels and their avg color
	vector<int>* edges = new vector<int>[num_of_labels];// all the edges between different super pixels.
	int* image_data = new int[num_of_labels * 3]; // average color.
	int* sp_num = new int[num_of_labels];
	memset(image_data, 0, sizeof(int) * num_of_labels * 3);
	memset(sp_num, 0, sizeof(int) * num_of_labels);
	int cur_label = 0;
	for(int h = 0; h < image->height; h++)
	{
		unsigned char* p = (unsigned char*)image->imageData + h *image->widthStep;
		for(int w = 0; w < image->width; w++)
		{
			cur_label = super_label[ h * image->width + w];
			sp_num[cur_label] ++;
			image_data[cur_label * 3] += p[0];
			image_data[cur_label * 3 + 1] += p[1];
			image_data[cur_label * 3 + 2] += p[2];
			// check left and the upper position.
			if( h > 0 && w > 0 )
			{
				int left_label = super_label[ h * image->width + w -1];
				int upper_label = super_label[ ( h - 1) * image->width + w];
				if( left_label != cur_label )
				{
					vector<int> cur_vec = edges[cur_label];
					if(std::find(cur_vec.begin(), cur_vec.end(), left_label) ==cur_vec.end()) 
						cur_vec.push_back(left_label);
				}
				if( upper_label != cur_label )
				{
					vector<int> cur_vec = edges[cur_label];
					if(std::find(cur_vec.begin(), cur_vec.end(), left_label) ==cur_vec.end()) 
						cur_vec.push_back(upper_label);
				}
			}
			p += 3;
		}
	}
	unsigned char* avg_image_data = new unsigned char[ 3 * num_of_labels];
	for( int i =0; i < num_of_labels; i++)
	{
		avg_image_data[i * 3] = image_data[i * 3] / sp_num[i];
		avg_image_data[i * 3 + 1] = image_data[i * 3 + 1] / sp_num[i];
		avg_image_data[i * 3 + 2] = image_data[i * 3 + 2] / sp_num[i];
	}
	//Now lets loop over all the data points.
	while(indexPt < num_of_labels)
	{
		//get current e1.
		float e1[2] = {0};
		if( fg_labels.count( indexPt ) != 0) // is fg
		{
			e1[0] = 0;
			e1[1] = INFINNITE_MAX;
		}
		else if( bg_labels.count(indexPt) != 0 )
		{
			e1[0] = INFINNITE_MAX;
			e1[1] = 0;
		}
		else
		{
			getE1(avg_image_data+ 3 * indexPt, e1);
		}
		// add node.
		graph->add_node();
        graph->add_tweights(indexPt, e1[0],e1[1]);
		// add edge, 4-connect
		vector<int> cur_edges = edges[indexPt];
		for(int i = 0; i < cur_edges.size(); i++)
		{
			float e2 = getE2(avg_image_data+ 3 * indexPt,avg_image_data + 3 * cur_edges[i]);
			graph->add_edge(indexPt,cur_edges[i],e2,e2);
		}
		indexPt ++;
	}
    
    return graph->maxflow();
}


int LasySnapping::runMaxflow()
{   
    const float INFINNITE_MAX = 1e10;
    int indexPt = 0;
    for(int h = 0; h < image->height; h ++){
        unsigned char* p = (unsigned char*)image->imageData + h *image->widthStep;
        for(int w = 0; w < image->width; w ++){
            // calculate energe E1
            float e1[2]={0};
            if(isPtInVector(cvPoint(w,h),forePts)){
                e1[0] =0;
                e1[1] = INFINNITE_MAX;
            }else if(isPtInVector(cvPoint(w,h),backPts)){
                e1[0] = INFINNITE_MAX;
                e1[1] = 0;
            }else {
                getE1(p,e1);
            }
            // add node
            graph->add_node();
            graph->add_tweights(indexPt, e1[0],e1[1]);
            // add edge, 4-connect
            if(h > 0 && w > 0){
                float e2 = getE2(p,p-3);
                graph->add_edge(indexPt,indexPt-1,e2,e2);
                e2 = getE2(p,p-image->widthStep);
                graph->add_edge(indexPt,indexPt-image->width,e2,e2);
            }
            
            p+= 3;
            indexPt ++;            
        }
    }
    
    return graph->maxflow();
}
IplImage* LasySnapping::getImageMask()
{
    IplImage* gray = cvCreateImage(cvGetSize(image),8,1); 
    int indexPt =0;
    for(int h =0; h < image->height; h++){
        unsigned char* p = (unsigned char*)gray->imageData + h*gray->widthStep;
        for(int w =0 ;w <image->width; w++){
            if (graph->what_segment(indexPt) == GraphType::SOURCE){
                *p = 0;
            }else{
                *p = 255;
            }
            p++;
            indexPt ++;
        }
    }
    return gray;
}

IplImage* LasySnapping::getImageMask(int* super_label)
{
	IplImage* gray = cvCreateImage(cvGetSize(image),8,1); 
    int indexPt =0;
    for(int h =0; h < image->height; h++){
        unsigned char* p = (unsigned char*)gray->imageData + h*gray->widthStep;
        for(int w =0 ;w <image->width; w++){
			if (graph->what_segment(super_label[h * gray->width + w]) == GraphType::SOURCE){
                *p = 0;
            }else{
                *p = 255;
            }
            p++;
        }
    }
    return gray;
}