#ifndef GENERAL_H
#define GENERAL_H

#include "AccessUnit.h"
#include "RecSample.h"
#include "HandGesture.h"

#define FPS 60
#define CAMERA 0
#define SAMPLES 7
#define square_len 3

typedef struct {
    Point start;
    Point end;
} R;

extern vector <RecSample> recsamples;
extern int  trackLower[SAMPLES][3];
extern int  trackUpper[SAMPLES][3];
extern bool contourFlag;

extern void getRecPos();
extern void readyForPalm(AccessUnit *m, HandGesture *hg);
extern void initTrackbar();
extern void getAverageColor(AccessUnit *m, int s, HandGesture *hg);
extern void genBinary(AccessUnit *m);
extern void genContours(AccessUnit *m, HandGesture *hg);
extern void drawConvexityDefect(Mat *toD, HandGesture hg);
extern void drawInformation(Mat *toD, HandGesture hg);
extern void drawGrid(Mat *toD);
extern void initMouseArea();
extern void drawMouseAndControl(Mat *toD, HandGesture hg);

#endif // GENERAL_H
