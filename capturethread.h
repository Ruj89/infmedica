#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

// Qt header files
#include <QThread>
#include <QtGui>
// OpenCV header files
#include "opencv/highgui.h"

using namespace cv;

class ImageBuffer;

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread(QLabel *label);
private:
    QLabel* label;
    QImage MatToQImage(const Mat& mat);
protected:
    void run();
signals:
    void newFrame(const QImage &frame);
};
#endif // CAPTURETHREAD_H
