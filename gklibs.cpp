#include "gklibs.h"

void drawReferLine(IplImage *src){
    int width = src->width;
    int height = src->height;
    cvCircle(src, Point(width/2, height/2),20, CV_RGB(20, 100, 20));
    cvLine(src, Point(0, 0), Point(width, height),CV_RGB(20, 100, 20));
    cvLine(src, Point(width, 0), Point(0, height),CV_RGB(20, 100, 20));
}
/*
 *自动阈值
 *传入src源三通道图片
 *转成dst二值化图片
 */
void ostuThreshold(IplImage *src, IplImage *dst){
    assert(src->nChannels == 3 && dst->nChannels == 1);
    cvSmooth(src, src, CV_GAUSSIAN, 11,11, 0, 0);
    cvSmooth(src, src, 0, 11, 11, 0, 0);

    //cvMorphologyEx(src, src, NULL, , CV_MOP_OPEN,11);
    int width = src->width;
    int height = src->height;
    IplImage *imgYCrCb = cvCreateImage(cvSize(width, height),
                                       src->depth, src->nChannels);
    cvCvtColor(src, imgYCrCb, CV_BGR2YCrCb);
    cvSplit(imgYCrCb, 0, dst, 0, 0);
    cvFlip(dst, dst, 1);
    float p[256] = {0};
    uchar *data = (uchar *)dst->imageData;
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
            p[data[i*dst->widthStep + j]] ++;
    float w0, w1, u0, u1, deltaMax, deltaTmp;
    float w[256] = {0}, tmp[256] = {0};
    w[0] = p[0];
    for(int i = 1; i < 256; i++)
    {
        w[i] = p[i] + w[i-1];
        tmp[i] = i * p[i] + tmp[i-1];
    }
    int threshold = 0;
    for(int i = 0; i < 255; i++)
    {
        w0 = w[i];
        w1 = w[255] - w[i];
        u0 = tmp[i]/w[i];
        u1 = (tmp[255] - tmp[i])/(w[255] - w[i]);
        deltaTmp = w0 * w1 *(u0 - u1) * (u0 - u1);
        if(deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = i;
        }
    }
    cvThreshold(dst, dst, threshold, 255, CV_THRESH_BINARY);
    cvReleaseImage(&imgYCrCb);
}


int vdot(Point a, Point b, Point c){
    Point v1 = Point (a.x - b.x, a.y - b.y);
    Point v2 = Point (c.x - b.x, c.y - b.y);
    return v1.x * v2.x + v1.y * v2.y;
}

float distance(Point a, Point b){
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
float getAngel(Point a, Point b, Point c){
    float f1 = ::distance(a, b);
    float f2 = ::distance(b, c);
    float dot = vdot(a, b, c);
    if(f1 == 0 || f2 == 0) return 180;
    float angle = acos(dot/(f1*f2));
    return angle * 180 /3.1415926;
}

string int2string(int number){
    stringstream ss;
    ss << number;
    return ss.str();
}
struct Cursor{
    Point point;
    bool pressed;
    Cursor(){
        point.x = 320;
        point.y = 240;
        pressed = false;
    }
};
Cursor cursor;
vector <Point> fingerTV;
float pred = 0;
/**
  *查找最大连通块
  *
 **/
void findMaxContour(IplImage *src){
    CvMemStorage *storage = cvCreateMemStorage();
    CvSeq *contour, *maxContour, *hull;
    double maxSize = 1;
    IplImage *tmp = cvCreateImage(cvGetSize(src), 8, 3);
    CvContourScanner scanner = cvStartFindContours(src, storage);
    while((contour = cvFindNextContour(scanner))!=NULL){
        double size = cvContourArea(contour);
        if(size > maxSize){
            maxContour = contour;
            maxSize = size;
        }
    }
    cvDrawContours(tmp, maxContour, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), 0);
    if(maxSize > 20000 && maxSize < 90000){
    Point *tpoint;
    Point center(0, 0);
    for(int i = 0; i < maxContour->total; i++){
        tpoint = (Point *) cvGetSeqElem(maxContour, i);
        center.x += tpoint->x;
        center.y += tpoint->y;
    }
    center.x /= maxContour->total;
    center.y /= maxContour->total;
    hull = cvConvexHull2(maxContour);
    int finger = 0;
    int offsetf = 0;
    double d = 0;
    vector <Point> newFingerTV;
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0);
    CvSeq *defect = cvConvexityDefects(maxContour, hull);
    for(int i = 0; i < defect->total; ++i){
        CvConvexityDefect *df = (CvConvexityDefect*)cvGetSeqElem(defect, i);
        d += distance(*df->depth_point, center);
        if(df->depth > 19){
            newFingerTV.push_back(*df->start);
            for(int i = 0; i < fingerTV.size(); i++){
                if(distance(fingerTV[i], *df->start) < 4){
                    CvFont font;
                    cvInitFont(&font, 1, 1,1);
                    char info[20];
                    float angle = getAngel(*df->start, *df->depth_point, *df->end);
                    offsetf = 1;
                    if(angle > 80)continue;
                    sprintf(info, "AG%.2f", angle);
                    cvPutText(tmp, info, *df->depth_point, &font, CV_RGB(0, 0, 255));
                    cvLine(tmp, *df->start,*df->depth_point,CV_RGB(0,255,255));
                    cvLine(tmp, *df->end,*df->depth_point,CV_RGB(0,255,255));
                    cvCircle(tmp, *df->depth_point, 5, CV_RGB(120, 255, 10));
                    finger += 1;
                    break;
                }
            }
        }
    }
    fingerTV.swap(newFingerTV);
    if(!fingerTV.empty()){
        char info[120];
        sprintf(info, "finger: %d", finger+offsetf);
    cvPutText(tmp, info,Point(10, 10),&font,CV_RGB(255, 255,255));
    finger = 0;

    d /= defect->total;
    if(fabs(d - pred) > 10) pred = d;
    int thickness = 1;
    if(d > 130 || pred > 130)thickness = -1;

    cout << "distance: " << fabs(d - pred)<<endl;
    sprintf(info, "distance: %f", pred);
    cvCircle(tmp, center, pred, CV_RGB(255, 0, 0));
    cvPutText(tmp, info,Point(10, 20),&font,CV_RGB(255, 255,255));
    drawReferLine(tmp);
    cvCircle(tmp, cursor.point, 5, CV_RGB(255, 0, 0),thickness, 8, 0);
    cursor.point.x += (center.x - src->width/2)/16;
    cursor.point.y += (center.y - src->height/2)/12;
    cursor.point.x = (cursor.point.x + src->width)%src->width;
    cursor.point.y = (cursor.point.y + src->height)%src->height;
    cvLine(tmp, center, Point(tmp->width/2, tmp->height/2), CV_RGB(100, 100,20));
    cvShowImage("tmp", tmp);
    }
    }else destroyAllWindows();
    cvReleaseImage(&tmp);
    cvReleaseMemStorage(&storage);
    return;
}
