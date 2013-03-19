#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QtGui>
#include "opencv/highgui.h"
#include <jsoncpp/json/json.h>

using namespace cv;

class ImageBuffer;

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    QString qrcode;
    CaptureThread();
    std::string jsondata;
    QByteArray userimage;
    static int writer(void *ptr, size_t size, size_t nmemb, string stream);
    static int writerImage(void *ptr, size_t size, size_t nmemb, QByteArray buffer);

private:
    QString qrcode_old;
    QLabel* label;
    QImage MatToQImage(const Mat& mat);
    QString getQRCode(cv::Mat );
    int timesCheckedQRCode;
    int empty_code;
    void getUserImage(QString);
    bool getUserJson(QString id);
    void parseJson();
protected:
    void run();
signals:
    void newFrame(const QImage &frame);
    void pushData(Json::Value);
    void setImage(QImage img);
    void setState(QString state);
};
#endif // CAPTURETHREAD_H
