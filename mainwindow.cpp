#include "mainwindow.h"
#include "ui_mainwindow.h"
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

void MainWindow::on_btnDisplay_clicked()
{
    cv::namedWindow("Disp");
    cv::VideoCapture cap(0);
    cv::Mat frame;
    do{
        cap >> frame;
        imshow("Disp",frame);
    }while(cv::waitKey(30)<0);
    cv::destroyAllWindows();
}
