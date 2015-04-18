#include "objectcontrol.h"
#include <stack>
objectcontrol::objectcontrol()
{
    curPos.x = 240;
    curPos.y = 180;
    prePos = curPos;
    img = cvCreateImage(cvSize(640, 480),8,3);
}

objectcontrol::~objectcontrol()
{
    cvReleaseImage(&img);
}

void objectcontrol::setPos(Point vec, float dist)
{
    if(mDistanceP2P(vec, Point(320, 240))< 60)
        return;
    vec.x -= 320;
    vec.y -= 240;
    float d = mDistanceP2P(Point(0, 0), vec);
    curPos.x += vec.x/d * 20;
    curPos.y += vec.y/d * 20;

    if(curPos.y < 0)curPos.y = 0;
    if(curPos.y > 480)curPos.y = 480;
    if(curPos.x < 0)curPos.x = 0;
    if(curPos.x > 640)curPos.x = 640;
    state.push_back(dist);
    dist = 0;
    for(int i = 0; i < state.size(); i++)
        dist += state[i];
    dist /= state.size();
    if(state.size() > 10) state.erase(state.begin());

    if(dist > 110)
        isPress = true;
    else isPress = false;
    showPos();
}

void objectcontrol::showPos()
{

    //if(isPress)
        cvLine(img, curPos, prePos, CV_RGB(0, 255, 255));
    //else {
      //  cvCircle(img, curPos, 10, CV_RGB(0, 255, 0),1, 8, 1);
    //}
       prePos = curPos;
    cvCircle(img, Point(320, 240), 60, CV_RGB(255, 255, 0),1, 8, 1);
    cvShowImage("testimg", img);
    //cvWaitKey(10);
}
