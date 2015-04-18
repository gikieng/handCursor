#include "handprocess.h"
HandProcess::HandProcess(int w = 480, int h = 320):width(w), height(h)
{
    storage = cvCreateMemStorage();
    img = cvCreateImage(cvSize(w, h), 8, 1);
    maxContour = NULL;
}
void HandProcess::otsuThreshold(IplImage *src)
{
    assert(src->nChannels == 3);
    cvSmooth(src, src, CV_GAUSSIAN, 11,11, 0, 0);
    cvSmooth(src, src, CV_MEDIAN, 11, 11, 0, 0);
    int width = src->width;
    int height = src->height;
    IplImage *imgYCrCb = cvCreateImage(cvSize(width, height),
                                       src->depth, src->nChannels);
    cvCvtColor(src, imgYCrCb, CV_BGR2YCrCb);
    cvSplit(imgYCrCb, 0, img, 0, 0);
    cvFlip(img, img, 1);
    float p[256] = {0};
    uchar *data = (uchar *)img->imageData;
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
            p[data[i*img->widthStep + j]] ++;
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
    cvThreshold(img, img, threshold, 255, CV_THRESH_BINARY);
    cvReleaseImage(&imgYCrCb);
}
void HandProcess::findMaxContour(){
    maxContourSize = -1;
    CvSeq *contour;
    CvContourScanner scanner = cvStartFindContours(img, storage);
    while((contour = cvFindNextContour(scanner))!=NULL){
        double size = cvContourArea(contour);
        if(size > maxContourSize){
            maxContour = contour;
            maxContourSize = size;
        }
    }
    isHand = (maxContourSize > 10000 && maxContourSize < 90000);
}
void HandProcess::getHandCenter()
{
    handCenter.x = 0;
    handCenter.y = 0;
    Point *tpoint;
    for(int i = 0; i < maxContour->total; i++){
        tpoint = (Point *) cvGetSeqElem(maxContour, i);
        handCenter.x += tpoint->x;
        handCenter.y += tpoint->y;
    }
    handCenter.x /= maxContour->total;
    handCenter.y /= maxContour->total;
}
int cmp(const Point &a,  const Point &b)
{
    return a.x < b.x;
}

void HandProcess::detectFingerNumber(){
    //show anylysis
    IplImage *showImg = cvCreateImage(cvSize(width, height), 8, 3);
    char info[120];
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 1, 1, 0);
    cvDrawContours(showImg, maxContour, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), 0);

    vector<Point>newFingerTips;
    CvSeq *hull = cvConvexHull2(maxContour);
    dist = 0;
    finger = 0;
    int osfinger = 0;
    CvSeq *defect = cvConvexityDefects(maxContour, hull);
    for(int i = 0; i < defect->total; ++i){
        CvConvexityDefect *df = (CvConvexityDefect*)cvGetSeqElem(defect, i);
        dist += mDistanceP2P(*df->depth_point, handCenter);
        if(df->depth > 19){
            float angle = mGetAngle(*df->start, *df->depth_point, *df->end);
            sprintf(info, "AG%.2f", angle);
            cvPutText(showImg, info, *df->depth_point, &font, CV_RGB(0, 0, 255));
            if(angle < 140) osfinger = 1;
            if(angle < 80){
                newFingerTips.push_back(*df->start);
                newFingerTips.push_back(*df->end);
                cvLine(showImg, *df->start, handCenter, CV_RGB(0, 155, 120),1);
                cvLine(showImg, *df->end, handCenter, CV_RGB(0, 155, 120),1);
                finger += 1;
            }
        }
    }
    fingerTips.clear();
    finger += osfinger;
    if(!newFingerTips.empty()){
        sort(newFingerTips.begin(), newFingerTips.end(), cmp);
        fingerTips.push_back(newFingerTips[0]);
        for(int i = 1; i < newFingerTips.size(); i++){
            if(mDistanceP2P(newFingerTips[i-1], newFingerTips[i]) > 20)
                fingerTips.push_back(newFingerTips[i]);
        }
    }
    else if(finger == 1){
        Point one(0, height);
        for(int i = 0; i < defect->total; ++i){
            CvConvexityDefect *df = (CvConvexityDefect*)cvGetSeqElem(defect, i);
            if(df->start->y < one.y)
                one = *df->start;
            if(df->end->y < one.y)
                one = *df->start;
        }
        fingerTips.push_back(one);
        cvCircle(showImg, one, 10, CV_RGB(255, 0, 0));
    }
    for(int i = 0; i < fingerTips.size(); i++){
        cvCircle(showImg, fingerTips[i], 10, CV_RGB(255, 0, 0));
        sprintf(info, "F:%d %f", i+1, mDistanceP2P(fingerTips[i], handCenter));
        cvPutText(showImg, info,fingerTips[i],&font,CV_RGB(255, 255,255));
    }

    if(finger > 6)isHand = false;
    finger = finger > 5?5:finger;
    dist /= defect->total;

    cvCircle(showImg, handCenter, dist/2, CV_RGB(255, 0, 0));
    sprintf(info, "finger: %d", finger);
    cvPutText(showImg, info,Point(10, 20),&font,CV_RGB(255, 255,255));
    sprintf(info, "distance: %f", dist);
    cvPutText(showImg, info,Point(10, 40),&font,CV_RGB(255, 255,255));
    sprintf(info, "ContourSize: %f", maxContourSize);
    cvPutText(showImg, info,Point(10, 60),&font,CV_RGB(255, 255,255));
    cvCircle(showImg, Point(320, 240), 60, CV_RGB(255, 0, 0));
    cvShowImage("debug", showImg);
    cvWaitKey(10);
    cvReleaseImage(&showImg);
}


HandProcess::~HandProcess()
{
    cvReleaseImage(&img);
    cvReleaseMemStorage(&storage);
}

