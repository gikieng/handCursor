#ifndef HANDPROCESS_H
#define HANDPROCESS_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "mlibs.h"
using namespace  std;
using namespace cv;

class HandProcess
{
public:
    HandProcess(int w, int h);

    //protected:
    CvMemStorage *storage;//存Contour的内存块
    IplImage *img;//二值化后的图片
    CvSeq *maxContour;//最大的连通块
    double maxContourSize;//最大的连通块的大小
    Point handCenter;//手掌中心坐标
    vector<Point>fingerTips;//手指指尖向量
    int finger;//手指数量
    float dist;//估测掌心圆半径
    int width, height;//图像大小
    bool isHand;//是否是手

    void findMaxContour();//查找最大的连通块
    void otsuThreshold(IplImage *src);//自动阈值
    void getHandCenter();//获取手掌中心
    void detectFingerNumber();//检测手指数量
    ~HandProcess();
};

#endif // HANDPROCESS_H
