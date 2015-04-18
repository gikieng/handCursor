#include "mlibs.h"

//点积
float mDot(const Point &s, const Point &f, const Point &t){
    return (s.x - f.x) * (t.x - f.x) + (s.y - f.y) * (t.y - f.y);
}
//两点之间距离
float mDistanceP2P(const Point &a, const Point &b){
    return sqrt((a.x - b.x) * (a.x - b.x)+
                (a.y - b.y) * (a.y - b.y));
}
//获取两向量的夹角
float mGetAngle(const Point &s, const Point &f, const Point &t){
    float f1 = mDistanceP2P(s, f);
    float f2 = mDistanceP2P(f, t);
    float angle = acos(mDot(s, f, t)/(f1*f2));
    return angle * 180 /PI;
}
