#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturethread.h"
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

void MainWindow::on_btnProva_clicked()
{
    CaptureThread *captureThread = new CaptureThread(ui->WidgetImageLabel);
    captureThread->start((QThread::Priority)1);
    connect(captureThread,SIGNAL(newFrame(QImage)),this,SLOT(draw(QImage)));

}

void MainWindow::draw(QImage img){
    qDebug() << "lanciato disegno";
    ui->WidgetImageLabel->setPixmap(QPixmap::fromImage(img));
    img.save("prova.jpg");
}
