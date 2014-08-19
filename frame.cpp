#include "frame.h"
#include "ui_frame.h"
#include "stdio.h"
using namespace cv;

enum Status { NORMAL, COLORCOLLECTION, GETAVERCOLOR,
             GESTUREDETECT } STATUS;
bool binaryFlag = false;
int sCount = 0;

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
            ui->radioButton_2->setEnabled(true);
            genContours(&m, &hg);
            break;
        default:
            break;
    }


    QImage img;
    if (binaryFlag) {
        cvtColor(m.binary, m.binary, CV_GRAY2RGB);
        img = QImage((unsigned char *)m.binary.data, m.binary.cols,
                m.binary.rows, QImage::Format_RGB888);
//        imshow("binary", m.binary);
    } else {
        cvtColor(m.frame, m.frame, CV_BGR2RGB);
        img = QImage((unsigned char *)m.frame.data, m.frame.cols,
                m.frame.rows, QImage::Format_RGB888);
    }
    ui->pic->setPixmap(QPixmap::fromImage(img));
}

void Frame::on_pushButton_3_clicked()
{
    getRecPos(&m);
    STATUS = COLORCOLLECTION;
    if (ui->radioButton_2->isChecked()) {
        on_radio_source_clicked();
        ui->radioButton_2->setChecked(false);
        ui->radio_source->setChecked(true);
    }
    ui->radioButton_2->setEnabled(false);
}

void Frame::on_pushButton_2_clicked()
{
    initTrackbar();
    STATUS = GETAVERCOLOR;
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

void Frame::on_radioButton_2_clicked()
{
    binaryFlag = true;
}

void Frame::on_radio_source_clicked()
{
    binaryFlag = false;
}
