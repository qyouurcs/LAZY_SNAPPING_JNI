#ifndef __DO_SUPER_PIXEL_H__
#define __DO_SUPER_PIXEL_H__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

static const int NUM_OF_SP = 800;

int* DoSuperPixel(Mat & image, int & num_of_labels);

#endif