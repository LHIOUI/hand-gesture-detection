#include "frame.h"
#include "ui_frame.h"
#include "stdio.h"
using namespace cv;

enum Status { NORMAL, COLORCOLLECTION, GETAVERCOLOR,
             GESTUREDETECT } STATUS;
int  sCount = 0;

Frame::Frame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Frame)
{
    ui->setupUi(this);

    connect(&timer, SIGNAL(timeout()), this, SLOT(readFrame()));
    connect(ui->close, SIGNAL(clicked()), this, SLOT(closeCamera()));
    connect(ui->open, SIGNAL(clicked()), this, SLOT(openCamera()));
    openCamera();
    hg = HandGesture(m);
    on_pushButton_3_clicked();
}

Frame::~Frame()
{
    delete ui;
    if (m.cap.isOpened())
        m.cap.release();
}

void Frame::openCamera()
{
    m.cap.open(CAMERA);
    timer.start(FPS);
}

void Frame::closeCamera()
{
    timer.stop();
    m.cap.release();
}

void Frame::readFrame()
{
    m.cap >> m.frame;
    cv::resize(m.frame, m.frame, Size(300, 300));
    flip(m.frame, m.frame, 1);

    switch (STATUS) {
        case COLORCOLLECTION:
            readyForPalm(&m);
            break;
        case GETAVERCOLOR:
            getAverageColor(&m, sCount);
            sCount++;
            if (sCount == 4) {
                sCount = 0;
                STATUS = GESTUREDETECT;
            }
            break;
        case GESTUREDETECT:
            genBinary(&m);
            if (ui->radioButton_2->isChecked())
                cvtColor(m.binary, m.binaryDisplay, CV_GRAY2RGB);
            ui->radioButton_2->setEnabled(true);
            genContours(&m, &hg);
            break;
        default:
            break;
    }

    Mat toD;
    if (ui->radioButton_2->isChecked()) {
        toD = m.binaryDisplay;
    } else if (ui->radio_source->isChecked()){
        cvtColor(m.frame, m.frame, CV_BGR2RGB);
        toD = m.frame;
    }

    if  (hg.isHand() && contourFlag && STATUS == GESTUREDETECT) {
        if (ui->checkBoxContour->isChecked())
            drawContours(toD, hg.contours, hg.cMaxId,
                Scalar(0, 0, 255), 2, 8);
        if (ui->checkBoxPoly->isChecked())
            drawContours(toD, hg.hullPoint, hg.cMaxId,
                Scalar(255, 0, 0), 2, 8);
        if (ui->checkBoxDefect->isChecked()) {
            printf("hg.defects[hg.cMaxId].size()=%d.\n", hg.defects[hg.cMaxId].size());
            if (hg.defects[hg.cMaxId].size() > 2) {
                vector <Vec4i> :: iterator d = hg.defects[hg.cMaxId].begin();
                while (d != hg.defects[hg.cMaxId].end()) {
                    Vec4i &v = *d;
                    int s = hg.contours[hg.cMaxId].size();
printf("v: %d %d %d s:%d\n", v[1], v[2], v[3], s);
//                    if (v[1] < s)
                    Point pStart(hg.contours[hg.cMaxId][v[1]]);
                    printf("got pStart\n");
//                    if (v[2] < s)
                    Point pEnd(hg.contours[hg.cMaxId][v[2]]);
                    printf("got pEnd\n");
//                    if (v[3] < s)
//                    Point pFar(hg.contours[hg.cMaxId][v[3]]);
//                    printf("got pFar\n");

//    printf("pStart:(%d, %d)  pEnd:(%d, %d) pFar:(%d, %d)\n", pStart.x, pStart.y, pEnd.x, pEnd.y, pFar.x, pFar.y);
    //                circle(toD, pFar, 4, Scalar(0, 255, 0), 4);
                    circle(toD, pStart, 4, Scalar(255, 0, 0), 4);
                    circle(toD, pEnd, 4, Scalar(0, 255, 0), 4);
                    d++;
//    printf("will out of while\n") ;
                }
            }
        }
    }

    QImage img((unsigned char *)toD.data, toD.cols,
            toD.rows, QImage::Format_RGB888);


    ui->pic->setPixmap(QPixmap::fromImage(img));
}

void Frame::on_pushButton_3_clicked()
{
    getRecPos();
    STATUS = COLORCOLLECTION;
    if (ui->radioButton_2->isChecked()) {
        ui->radioButton_2->setChecked(false);
        ui->radio_source->setChecked(true);
    }
    ui->radioButton_2->setEnabled(false);
}

void Frame::on_pushButton_2_clicked()
{
    if (STATUS == COLORCOLLECTION) {
        initTrackbar();
        STATUS = GETAVERCOLOR;
    }
}

void Frame::on_pushButton_clicked()
{
    namedWindow("trackbars", WINDOW_AUTOSIZE);
    createTrackbar("lower1", "trackbars", &trackLower[0][0], 255);
    createTrackbar("upper1", "trackbars", &trackUpper[0][0], 255);
    createTrackbar("lower2", "trackbars", &trackLower[0][1], 255);
    createTrackbar("upper2", "trackbars", &trackUpper[0][1], 255);
    createTrackbar("lower3", "trackbars", &trackLower[0][2], 255);
    createTrackbar("upper3", "trackbars", &trackUpper[0][2], 255);
}
