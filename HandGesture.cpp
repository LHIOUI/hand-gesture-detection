#include "HandGesture.h"

HandGesture::HandGesture()
{
}

HandGesture::HandGesture(AccessUnit gm) {
    m = gm;
}

void HandGesture::initVec()
{
    hullInt = vector <vector <int> > (contours.size());
    hullPoint = vector <vector <Point> > (contours.size());
    defects = vector <vector <Vec4i> > (contours.size());
}

float HandGesture::distance(Point p1, Point p2)
{
    return sqrt( (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) );
}

float HandGesture::getAngle(Point p1, Point p0, Point p2)
{
    float l1    = distance(p1, p0);
    float l2    = distance(p2, p0);
    float arc   = (p1.x-p0.x)*(p2.x-p0.x) + (p1.y-p0.y)*(p2.y-p0.y);
    float angle = acos(arc/(l1*l2));
    angle = angle * 180 / PI;
    return angle;
}

void HandGesture::removeOtherPoints(vector<Vec4i>defects)
{
    Vec4i tmp;
    float tolerance = bounRect.width/6;

    int i, j;
    for (i = 0; i < (int)defects.size(); i++) {
        for (j = i; j < (int)defects.size(); j++) {
            Point pStart(contours[cMaxId][defects[i][0]]);
            Point pEnd(contours[cMaxId][defects[i][1]]);
            Point pStart2(contours[cMaxId][defects[j][0]]);
            Point pEnd2(contours[cMaxId][defects[j][1]]);
            if (distance(pStart, pEnd2) < tolerance) {
                contours[cMaxId][defects[i][0]] = pEnd2;
//				break;
            }
            if (distance(pEnd, pStart2) < tolerance) {
                contours[cMaxId][defects[j][0]] = pEnd;
            }
        }
    }
}

void HandGesture::eleminateDefects()
{
    int tolerance = bounRect.height/5;
    float angleTolerance = 95;
    vector < Vec4i > newDefects;

    vector<Vec4i>::iterator i = defects[cMaxId].begin();
    while (i != defects[cMaxId].end()) {
        Vec4i &v = *i;
        Point pStart(contours[cMaxId][v[0]]);
        Point pEnd(contours[cMaxId][v[1]]);
        Point pFar(contours[cMaxId][v[2]]);

        if (distance(pStart, pFar) > tolerance && distance(pEnd, pFar) > tolerance
            && getAngle(pStart, pFar, pEnd) < angleTolerance) {
            if (pStart.y > (bounRect.y + bounRect.height-bounRect.height/4) ||
                pEnd.y > (bounRect.y + bounRect.height-bounRect.height/4)) {
            } else {
                newDefects.push_back(v);
            }
        }
        i++;
    }
    nDefects = newDefects.size();
    defects[cMaxId].swap(newDefects);
    removeOtherPoints(defects[cMaxId]);
}

bool HandGesture::isHand()
{
    float h = bounRect.height;
    float w = bounRect.width;

    bool isHand = true;
    if (fingerTips.size() > 5) {
        isHand = false;
    } else if (h == 0 || w == 0) {
        isHand = false;
    } else if (h/w > 4 || w/h > 4) {
        isHand = false;
    } else if (bounRect.x < 20) {
        isHand = false;
    }

    return isHand;
}
