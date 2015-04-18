#ifndef OBJECTCONTROL_H
#define OBJECTCONTROL_H
#include <opencv2/opencv.hpp>
#include "mlibs.h"
#include <vector>
using namespace cv;
class objectcontrol
{
private:
    Point curPos;
    Point prePos;
    bool isPress;
    IplImage *img;
    std::vector<int> state;
public:
    void setPos(Point vec, float dist);
    void showPos();
    objectcontrol();
    ~objectcontrol();
};

#endif // OBJECTCONTROL_H
