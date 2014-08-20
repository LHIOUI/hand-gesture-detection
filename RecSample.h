#ifndef RECSAMPLES_H
#define RECSAMPLES_H

#include "opencv2\opencv.hpp"
using namespace cv;

class RecSample {
public:
    Mat    img;
    Point  left_up, right_down;

    RecSample();
    RecSample(Point lu, Point rd);
    void draw(Mat src);
    void getSample(Mat src);

private:
    Scalar color;
    int    border;
};

#endif // RECSAMPLES_H
