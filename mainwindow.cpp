#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPaintEvent>
#include <QPainter>
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    CvCapture* capture = cvCaptureFromCAM(0);
    IplImage *currFrame;
    currFrame = cvQueryFrame(capture);
    QImage currFrameQ(
                (uchar *)currFrame->imageData,
                currFrame->width,
                currFrame->height,
                currFrame->widthStep,
                QImage::Format_RGB888
            );
    currFrameQ = currFrameQ.rgbSwapped();
    QPainter widgetPainter(ui->WidgetImageCaptured);
    widgetPainter.drawImage(rect(), currFrameQ, currFrameQ.rect());
}

void MainWindow::on_btnProva_clicked()
{

}
