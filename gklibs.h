#ifndef GKLIBS
#define GKLIBS
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;
using namespace cv;

void ostuThreshold(IplImage *src, IplImage *dst);
void findMaxContour(IplImage *src);

#endif // GKLIBS

