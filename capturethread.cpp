#include "capturethread.h"
#include "mainwindow.h"
#include <opencv2/opencv.hpp>


// Qt header files
#include <QDebug>
CaptureThread::CaptureThread(QLabel *label) : QThread(),
                             label(label)
{
} // CaptureThread constructor

QImage CaptureThread::MatToQImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
} // MatToQImage()

void CaptureThread::run()
{
    cv::VideoCapture capture;
    cv::Mat frame;
    QImage img;
    int cameraIndex = 0;
    bool stopFlag = false;

    qDebug() << "Opening camera" << cameraIndex ;
    capture.open(cameraIndex);

    if(!capture.isOpened()){
        qDebug() << "Could not open camera" << cameraIndex;
        return;
    }

    while(!stopFlag){
        capture >> frame;
        qDebug() << "Frame Width = " << frame.cols << "Frame Height = " << frame.rows;
        if(frame.cols ==0 || frame.rows==0){
            qDebug() << "Invalid frame skipping";
            continue;
        }
        Mat currentFrame=Mat(frame);
        img = MatToQImage(currentFrame); //Custom function

        emit newFrame(img);
    }
    capture.release();
    qDebug() << "Thread returning";
} // run()
