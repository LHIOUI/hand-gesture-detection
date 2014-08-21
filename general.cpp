#include "general.h"
#include "stdio.h"
#include "stdlib.h"
#include "string"
#include "iostream"
#include "sstream"
#include "windows.h"

vector <RecSample> recsamples;
int  avrColor[SAMPLES][3];
int  trackLower[SAMPLES][3];
int  trackUpper[SAMPLES][3];
int  record;
bool contourFlag = false;
R    area;
R    lineRaw[9], lineCol[9];

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

void readyForPalm(AccessUnit *m, HandGesture *hg)
{
    hg->numToDraw = 0;
    hg->bounRect  = Rect(0, 0, 0, 0);

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

void getAverageColor(AccessUnit *m, int s, HandGesture *hg)
{
    hg->numToDraw = 0;
    hg->bounRect  = Rect(0, 0, 0, 0);
    int j;
    cvtColor(m->frame, m->frame, CV_BGR2HLS);
    for (j = 0; j < SAMPLES; j++) {
        recsamples[j].getSample(m->frame);
        calc(recsamples[j], avrColor[j], s);
        recsamples[j].draw(m->frame);
    }

    cvtColor(m->frame, m->frame, CV_HLS2BGR);
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
    Mat tmp;
    m->frame.copyTo(tmp);
    blur(tmp, tmp, Size(3, 3));
    cvtColor(tmp, tmp, CV_BGR2HLS);
    for (i = 0; i < SAMPLES; i++) {
        initColor(i);
        lower = Scalar(avrColor[i][0]-trackLower[i][0],
            avrColor[i][1]-trackLower[i][1], avrColor[i][2]-trackLower[i][2]);
        upper = Scalar(avrColor[i][0]+trackUpper[i][0],
            avrColor[i][1]+trackUpper[i][1], avrColor[i][2]+trackUpper[i][2]);
        m->binaryList.push_back(Mat(m->frame.rows, m->frame.cols, CV_8U));
        inRange(tmp, lower, upper, m->binaryList[i]);
    }

    Mat roi(m->frame, Rect(10, 18, 30, 30));
    roi = Scalar(avrColor[0][0], avrColor[0][1], avrColor[0][2]);

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
    } else {
        hg->bounRect = Rect(0, 0, 0, 0);
        hg->fingerTips.clear();
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

        circle(*toD, pStart, 3, Scalar(255, 0, 0), 2);
        circle(*toD, pEnd, 3, Scalar(255, 0, 0), 2);
        circle(*toD, pFar, 3, Scalar(0, 255, 0), 2);
        d++;
    }
}

string int2String(int a)
{
    char s[5];
    sprintf(s, "%d", a);
    return s;
}

string bool2String(bool a)
{
    if (a) {
        return "true";
    } else {
        return "false";
    }
}

void drawInformation(Mat *toD, HandGesture hg)
{
    int fontFace = FONT_HERSHEY_PLAIN;
    Scalar color(245,200,200);
//    int xPos = toD->cols/1.5;
//    int yPos = toD->rows/2;
    int xPos = 240;
    int yPos = 6;
    float fontSize = 0.7f;
    int lineChange = 14;
    string info = "Figures info:";
    putText(*toD, info, Point(yPos, xPos), fontFace, fontSize, color);
    xPos += lineChange;
    info  = string("Number of defects: ") + string(int2String(hg.nDefects)) ;
    putText(*toD, info, Point(yPos, xPos), fontFace, fontSize, color);
    xPos += lineChange;
    info = string("Is hand: ") + string(bool2String(hg.isHand()));
    putText(*toD, info, Point(yPos, xPos), fontFace, fontSize, color);
    xPos += lineChange;
    info = string("Command Number is: ") + string(int2String(hg.numToDraw));
    putText(*toD, info, Point(yPos, xPos), fontFace, fontSize, color);
}


void drawGrid(Mat *toD)
{
//    rectangle(*toD, area.start, area.end, Scalar(240, 236, 124), 2);
    int i;
    for (i = 0; i < 6; i++) {
        if (i == 2 || i == 3) {
            line(*toD, lineRaw[i].start, lineRaw[i].end, Scalar(240, 236, 124), 2);
            line(*toD, lineCol[i].start, lineCol[i].end, Scalar(240, 236, 124), 2);
        } else {
            line(*toD, lineRaw[i].start, lineRaw[i].end, Scalar(172, 224, 154));
            line(*toD, lineCol[i].start, lineCol[i].end, Scalar(172, 224, 154));
        }
    }
}


void initMouseArea()
{
    int i;
    for (i = 0; i < 6; i++) {
        int foo = 85 + i * 26;
        lineRaw[i].start = Point(foo, 85);
        lineRaw[i].end = Point(foo, 215);
        lineCol[i].start = Point(85, foo);
        lineCol[i].end = Point(215, foo);
    }
}

int sign(int x)
{
    if (x == 0) {
        return 0;
    } else if (x < 0) {
        return -1;
    }
    return 1;
}

void mouseControl(Point mouse, HandGesture hg)
{
    if (mouse.x > 85 && mouse.x < 215 && mouse.y > 85 && mouse.y < 215) {
        POINT pSrc;
        if (!GetCursorPos(&pSrc)) {
            printf("failed to get position of cursors.\n");
            return;
        }
        printf("cursors position: x=%d\ty=%d\n", pSrc.x, pSrc.y);
        int x   = (mouse.x - 150) / 13;
        if (x == 2 || x == 4) x--;
        pSrc.x += sign(x) * (x * x * x * x + 8);
        int y   = (mouse.y - 150) / 25;
        if (y == 2 || y == 4) y--;
        pSrc.y += sign(y) * (y * y * y * y + 8);
        printf("*****  x=%d y=%d\n", x, y);
        if (!SetCursorPos(pSrc.x, pSrc.y)) {
            printf("failed to set position of cursors.\n");
        }
//        if (x != 0 || y != 0) {
            printf("move cursors to: x=%d\ty=%d\n", pSrc.x, pSrc.y);
//        }

        switch (hg.nDefects) {
            case 0:
                record = 0;
                break;
            case 1:
                if (record == 3) {
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
                record = 1;
                break;
            case 2:
                if (record != 2) {
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
                record = 2;
                break;
            case 3:
                if (record != 3) {
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                }
                record = 3;
                break;
            case 4:
                if (record == 3) {
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
                record = 4;
                break;
            case 5:
                if (record == 3) {
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
                record = 5;
                break;
            default:
                break;
        }
    }

}

void drawMouseAndControl(Mat *toD, HandGesture hg)
{
    int n = hg.defects[hg.cMaxId].size();
    if (n > 0) {
        Point mouse(0, 0);
        vector <Vec4i> :: iterator d = hg.defects[hg.cMaxId].begin();
        while (d != hg.defects[hg.cMaxId].end()) {
            Vec4i &v = *d;
            Point pFar(hg.contours[hg.cMaxId][v[2]]);
            mouse.x += pFar.x;
            mouse.y += pFar.y;
            d++;
        }
        mouse.x /= n;
        mouse.y /= n;
        line(*toD, Point(mouse.x-5, mouse.y-5), Point(mouse.x+5, mouse.y+5),
             Scalar(0, 0, 255), 3);
        line(*toD, Point(mouse.x-5, mouse.y+5), Point(mouse.x+5, mouse.y-5),
             Scalar(0, 0, 255), 3);

        mouseControl(mouse, hg);
    }
}
