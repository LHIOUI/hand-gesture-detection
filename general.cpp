#include "general.h"
#include "stdio.h"

vector <RecSample> recsamples;
int  avrColor[SAMPLES][3];
int  trackLower[SAMPLES][3];
int  trackUpper[SAMPLES][3];
bool contourFlag = false;

void getRecPos()
{
//    printf("int getRecPos().\n");
    recsamples.push_back(RecSample( Point(150, 150),
        Point(150+square_len, 150+square_len)));
    recsamples.push_back(RecSample( Point(160, 150),
        Point(160+square_len, 150+square_len)));
    recsamples.push_back(RecSample( Point(140, 150),
        Point(140+square_len, 150+square_len)));

    recsamples.push_back(RecSample( Point(155, 170),
        Point(155+square_len, 170+square_len)));
    recsamples.push_back(RecSample( Point(145, 170),
        Point(145+square_len, 170+square_len)));

    recsamples.push_back(RecSample( Point(155, 130),
        Point(155+square_len, 130+square_len)));
    recsamples.push_back(RecSample( Point(145, 130),
        Point(145+square_len, 130+square_len)));
}

void readyForPalm(AccessUnit *m)
{
//    printf("in readyForPalm().\n");
//    printf("Cover rectangles with palm.\n");

    int j;
    for(j = 0; j < SAMPLES; j++) {
        recsamples[j].draw(m->frame);
    }
}

int getRe(vector <int> arr)
{
    size_t size = arr.size();
    sort(arr.begin(), arr.end());
    return arr[size/2];
}

void calc(RecSample rs, int ac[3], int flag)
{
    vector <int> h;
    vector <int> l;
    vector <int> s;

    int i, j;
    for (i = 0; i < rs.img.rows; i++) {
        for (j = 0; j < rs.img.cols; j++) {
            h.push_back(rs.img.data[rs.img.channels()*(rs.img.cols*+j) + 0]);
            l.push_back(rs.img.data[rs.img.channels()*(rs.img.cols*+j) + 1]);
            s.push_back(rs.img.data[rs.img.channels()*(rs.img.cols*+j) + 2]);
        }
    }
    if (flag) {
        ac[0] =(ac[0] + getRe(h)) / 2;
        ac[1] =(ac[1] + getRe(l)) / 2;
        ac[2] =(ac[2] + getRe(s)) / 2;
    } else {
        ac[0] = getRe(h);
        ac[1] = getRe(l);
        ac[2] = getRe(s);
    }
}

void getAverageColor(AccessUnit *m, int s)
{
//    printf("in getAverageColor().\n");
    int j;
    cvtColor(m->frame, m->frame, CV_BGR2HLS);
    for (j = 0; j < SAMPLES; j++) {
        recsamples[j].getSample(m->frame);
        calc(recsamples[j], avrColor[j], s);
        recsamples[j].draw(m->frame);
    }

    cvtColor(m->frame, m->frame, CV_HLS2BGR);
//    printf("Finding average color of hand.\n");
}

void initTrackbar()
{
//    printf("in initTrackbar()\n");
    int i;
    for (i = 0; i < SAMPLES; i++) {
        trackLower[i][0] = 12;
        trackUpper[i][0] = 7;
        trackLower[i][1] = 30;
        trackUpper[i][1] = 40;
        trackLower[i][2] = 80;
        trackUpper[i][2] = 80;
    }
}

void initColor(int n)
{
    int i;
    for (i = 0; i < 3; i++) {
        trackLower[n][i] = trackLower[0][i];
        trackUpper[n][i] = trackUpper[0][i];
    }

    if (avrColor[n][0] < trackLower[n][0]) {
        trackLower[n][0] = avrColor[n][0];
    } if (avrColor[n][1] < trackLower[n][1]) {
        trackLower[n][1] = avrColor[n][1];
    } if (avrColor[n][2] < trackLower[n][2]) {
        trackLower[n][2] = avrColor[n][2];
    } if (avrColor[n][0] + trackUpper[n][0] > 255) {
        trackUpper[n][0] = 255-avrColor[n][0];
    } if (avrColor[n][1] + trackUpper[n][1] > 255) {
        trackUpper[n][1] = 255-avrColor[n][1];
    } if (avrColor[n][2] + trackUpper[n][2] > 255) {
        trackUpper[n][2] = 255-avrColor[n][2];
    }
}

void genBinary(AccessUnit *m)
{
    Scalar lower, upper;
    int i;
    blur(m->frame, m->frame, Size(3, 3));
    cvtColor(m->frame, m->frame, CV_BGR2HLS);
    for (i = 0; i < SAMPLES; i++) {
        initColor(i);
        lower = Scalar(avrColor[i][0]-trackLower[i][0],
            avrColor[i][1]-trackLower[i][1], avrColor[i][2]-trackLower[i][2]);
        upper = Scalar(avrColor[i][0]+trackUpper[i][0],
            avrColor[i][1]+trackUpper[i][1], avrColor[i][2]+trackUpper[i][2]);
        m->binaryList.push_back(Mat(m->frame.rows, m->frame.cols, CV_8U));
        inRange(m->frame, lower, upper, m->binaryList[i]);
    }
    cvtColor(m->frame, m->frame, CV_HLS2BGR);

    Mat tmp(m->frame, Rect(10, 18, 30, 30));
    tmp = Scalar(avrColor[0][0], avrColor[0][1], avrColor[0][2]);

    m->binaryList[0].copyTo(m->binary);
    for (i = 1; i < SAMPLES; i++) {
        m->binary += m->binaryList[i];
    }
    medianBlur(m->binary, m->binary, 7);
}

int findBiggestContour(vector < vector <Point> > contours)
{
    int biggest = -1;
    unsigned int size = 0;
    unsigned int i;
    for (i = 0; i < contours.size(); i++) {
        if (contours[i].size() > size) {
            size = contours[i].size();
            biggest = i;
        }
    }
    return biggest;
}

void genContours(AccessUnit *m, HandGesture *hg)
{
    contourFlag = false;
    findContours(m->binary, hg->contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    hg->initVec();
    hg->cMaxId = findBiggestContour(hg->contours);

    if (hg->contours[hg->cMaxId].size() < 200) hg->cMaxId = -1;
    if (hg->cMaxId != -1 ) {
        contourFlag = true;

        hg->bounRect = boundingRect(hg->contours[hg->cMaxId]);
        convexHull(Mat(hg->contours[hg->cMaxId]), hg->hullPoint[hg->cMaxId], false, true);
        convexHull(Mat(hg->contours[hg->cMaxId]), hg->hullInt[hg->cMaxId], false, false);
        approxPolyDP(Mat(hg->hullPoint[hg->cMaxId]), hg->hullPoint[hg->cMaxId], 18, true);

        if (hg->contours[hg->cMaxId].size() > 3) {
            convexityDefects(hg->contours[hg->cMaxId], hg->hullInt[hg->cMaxId], hg->defects[hg->cMaxId]);
            hg->eleminateDefects();
        }

        if (hg->isHand()) {
            hg->getFingerTips();
        }
    }
}

void drawConvexityDefect(Mat *toD, HandGesture hg)
{
    vector <Vec4i> :: iterator d = hg.defects[hg.cMaxId].begin();
    while (d != hg.defects[hg.cMaxId].end()) {
        Vec4i &v = *d;
        Point pStart(hg.contours[hg.cMaxId][v[0]]);
        Point pEnd(hg.contours[hg.cMaxId][v[1]]);
        Point pFar(hg.contours[hg.cMaxId][v[2]]);

        circle(*toD, pStart, 4, Scalar(255, 0, 0), 4);
        circle(*toD, pEnd, 4, Scalar(0, 255, 0), 4);
        circle(*toD, pFar, 4, Scalar(0, 0, 255), 4);
        d++;
    }
}
