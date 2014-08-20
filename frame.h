#ifndef FRAME_H
#define FRAME_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "AccessUnit.h"
#include "general.h"
#include "HandGesture.h"
using namespace cv;

namespace Ui {
class Frame;
}

class Frame : public QWidget
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = 0);
    ~Frame();

private:
    Ui::Frame *ui;
    AccessUnit m;
    HandGesture hg;
    QTimer timer;

private slots:
    void openCamera();
    void closeCamera();
    void readFrame();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_checkBoxMouse_clicked();
};


#endif // FRAME_H
