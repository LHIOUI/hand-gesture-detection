#ifndef GENERAL_H
#define GENERAL_H

#include "AccessUnit.h"
#include "RecSample.h"
#include "HandGesture.h"

#define FPS 60
#define CAMERA 0
#define SAMPLES 7
#define square_len 3

extern vector <RecSample> recsamples;
extern int  trackLower[SAMPLES][3];
extern int  trackUpper[SAMPLES][3];
extern bool contourFlag;

extern void getRecPos();
extern void readyForPalm(AccessUnit *m);
extern void initTrackbar();
extern void getAverageColor(AccessUnit *m, int s);
extern void genBinary(AccessUnit *m);
extern void genContours(AccessUnit *m, HandGesture *hg);

#endif // GENERAL_H
