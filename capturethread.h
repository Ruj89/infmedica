#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QtGui>
#include "opencv/highgui.h"

using namespace cv;

class ImageBuffer;

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    int timesCheckedQRCode;
    QString qrcode;
    CaptureThread();
    std::string temp;
    static int writer(void *ptr, size_t size, size_t nmemb, string stream);
private:
    QLabel* label;
    QImage MatToQImage(const Mat& mat);
    QString getQRCode(cv::Mat );
protected:
    void run();
signals:
    void newFrame(const QImage &frame);
};
#endif // CAPTURETHREAD_H
