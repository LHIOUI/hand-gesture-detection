#include "RecSample.h"
#include "opencv2\opencv.hpp"
#include "stdio.h"

RecSample::RecSample()
{
    left_up    = Point(0, 0);
    right_down = Point(0, 0);
}

RecSample::RecSample(Point lu, Point rd)
{
    left_up    = lu;
    right_down = rd;
    color  = Scalar(0, 255, 0);
    border = 2;

//	printf("img.cols=%d img.rows=%d\n", img.cols, img.rows);

}

void RecSample::getSample(Mat src)
{
    img = src(Rect(left_up, right_down));
}

void RecSample::draw(Mat src)
{
    rectangle(src, left_up, right_down, color, border);
}
