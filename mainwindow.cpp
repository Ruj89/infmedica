#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturethread.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CaptureThread *captureThread = new CaptureThread(ui->WidgetImageLabel);
    captureThread->start();
    connect(captureThread,SIGNAL(newFrame(QImage)),this,SLOT(draw(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::draw(QImage img){
    img = img.scaled(ui->WidgetImageLabel->width(), ui->WidgetImageLabel->height());
    ui->WidgetImageLabel->setPixmap(QPixmap::fromImage(img));
}
