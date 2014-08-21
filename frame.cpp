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
    initMouseArea();
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

    hg.nDefects  = 0;
    switch (STATUS) {
        case COLORCOLLECTION:
            readyForPalm(&m, &hg);
            break;
        case GETAVERCOLOR:
            getAverageColor(&m, sCount, &hg);
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
            hg.getFingerNumber();
            break;
        default:
            break;
    }

    Mat toD;
    if (ui->radioButton_2->isChecked()) {
        toD = m.binaryDisplay;
    } else if (ui->radio_source->isChecked()){
//        cvtColor(m.frame, m.frame, CV_BGR2YCrCb);
        cvtColor(m.frame, m.frame, CV_BGR2RGB);
        toD = m.frame;
    }

    if (ui->checkBoxGrid->isChecked() && STATUS == GESTUREDETECT) {
        drawGrid(&toD);
    }

    if  (hg.isHand() && contourFlag && STATUS == GESTUREDETECT) {
        if (ui->checkBoxContour->isChecked())
            drawContours(toD, hg.contours, hg.cMaxId,
                Scalar(0, 0, 255), 2, 8);
        if (ui->checkBoxPoly->isChecked())
            drawContours(toD, hg.hullPoint, hg.cMaxId,
                Scalar(255, 0, 0), 2, 8);
        if (ui->checkBoxDefect->isChecked()) {
            drawConvexityDefect(&toD, hg);
        }
        if (ui->checkBoxMouse->isChecked()) {
            drawMouseAndControl(&toD, hg);
        }
    }

    if (ui->information->isChecked()) {
        drawInformation(&toD, hg);
    }
    if (ui->checkBoxRect->isChecked()) {
        rectangle(toD, hg.bounRect, Scalar(234, 234, 234));
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
    createTrackbar("low-h", "trackbars", &trackLower[0][0], 255);
    createTrackbar("up-h", "trackbars", &trackUpper[0][0], 255);
    createTrackbar("low-l", "trackbars", &trackLower[0][1], 255);
    createTrackbar("up-l", "trackbars", &trackUpper[0][1], 255);
    createTrackbar("low-s", "trackbars", &trackLower[0][2], 255);
    createTrackbar("up-s", "trackbars", &trackUpper[0][2], 255);
}

void Frame::on_checkBoxMouse_clicked()
{
    if (ui->checkBoxMouse->isChecked()) {
        ui->checkBoxGrid->setChecked(true);
    } else {
        ui->checkBoxGrid->setChecked(false);
    }
}
