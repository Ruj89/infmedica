#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QtGui>
#include "opencv/highgui.h"
#include <jsoncpp/json/json.h>
#include <QtNetwork/QNetworkAccessManager>

using namespace cv;

class ImageBuffer;

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    int timesCheckedQRCode;
    QString qrcode;
    CaptureThread();
    std::string jsondata;
    std::ostringstream userimage;
    static int writer(void *ptr, size_t size, size_t nmemb, string stream);
private:
    QNetworkReply *rep;
    QLabel* label;
    QImage MatToQImage(const Mat& mat);
    QString getQRCode(cv::Mat );
    void getUserImage(QString);
    bool getUserJson(QString id);
    void parseJson();
private slots:
    void replyFinish ();
protected:
    void run();
signals:
    void newFrame(const QImage &frame);
    void pushData(Json::Value);
    void setImage(QImage img);
};
#endif // CAPTURETHREAD_H
