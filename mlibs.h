#ifndef MLIBS
#define MLIBS
#include <opencv2/opencv.hpp>
using namespace cv;
#define PI 3.1415926
float mDot(const Point &s, const Point &f, const Point &t);
float mDistanceP2P(const Point &a, const Point &b);
float mGetAngle(const Point &s, const Point &f, const Point &t);
#endif // MLIBS
