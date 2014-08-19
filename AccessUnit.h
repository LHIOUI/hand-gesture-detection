#ifndef ACCESSUNIT_H
#define ACCESSUNIT_H

#include "opencv2\opencv.hpp"
using namespace cv;

class AccessUnit {
public:
    Mat frame;
    Mat binary;
    vector <Mat> binaryList;
    VideoCapture cap;
};

#endif // ACCESSUNIT_H
