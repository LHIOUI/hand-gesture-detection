#ifndef HANDGESTURE_H
#define HANDGESTURE_H

#include "opencv2\opencv.hpp"
#include "AccessUnit.h"
#include "math.h"

#define PI 3.1415926

using namespace cv;

class HandGesture {
public:
    AccessUnit m;
    vector <vector <Point> > contours;
    vector <vector <Point> > hullPoint;
    vector <vector <int> >   hullInt;
    vector <vector <Vec4i> > defects;
    vector <Point> fingerTips;
    vector <int> fingerNumbers;
    int  frameNum;
    int  numToDraw;
    int  nDefects;
    int  cMaxId;
    Rect bounRect;

    HandGesture();
    HandGesture(AccessUnit gm);
    void genContour();
    void initVec();
    void eleminateDefects();
    bool isHand();
    void getFingerTips();
    void getFingerNumber();


private:
    float distance(Point p1, Point p2);
    float getAngle(Point p1, Point p0, Point p2);
    void  removeOtherPoints(vector<Vec4i>defects);
    void  checkForOneFinger();
    void  compute();
    void  removeOtherFingerTips();

    int   numNoFinger;

};

#endif // HANDGESTURE_H
