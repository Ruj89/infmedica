#include "capturethread.h"
#include "mainwindow.h"
#include <decodeqr.h>
#include <opencv2/opencv.hpp>
#include <curl/curl.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QDebug>

CaptureThread::CaptureThread() : QThread()
{
    timesCheckedQRCode = 0;
    qrcode = "";
}

void CaptureThread::run()
{
    cv::VideoCapture capture;
    cv::Mat frame;
    QImage img;
    int cameraIndex = 0;
    bool stopFlag = false;

    // Apertura della webcam
    qDebug() << "Si apre la webcam" << cameraIndex ;
    capture.open(cameraIndex);
    if(!capture.isOpened()){
        qDebug() << "Non si può aprire la webcam" << cameraIndex;
        return;
    }

    // Ciclo principale di lettura da webcam
    while(!stopFlag){
        // Riversa il contenuto della cattura in un oggetto
        capture >> frame;
        if(frame.cols ==0 || frame.rows==0){
            qDebug() << "Salto di un frame";
            continue;
        }

        // Preleva il codice QR dall'immagine e aggiorna il numero di volte che viene prelevato
        QString qrcode_decoded = getQRCode(frame);
        if(qrcode_decoded != ""){
            if (qrcode_decoded == qrcode){
                timesCheckedQRCode++;
                qDebug() << "Stringa individuata" << timesCheckedQRCode << "volte.";
            } else {
                timesCheckedQRCode = 1;
                qrcode = QString(qrcode_decoded);
            }
        }

        // Il codice è prelevato con successo, quindi richiesta al webserver
        if(timesCheckedQRCode == 5){
            qDebug() << "Stringa individuata 5 volte:" << qrcode;
            if(getUserJson(qrcode)) parseJson();
        }

        // Converti l'immagine in un formato delle Qt e disegnalo
        img = MatToQImage(frame);
        emit newFrame(img);
    }
    capture.release();
    qDebug() << "Chiusura del thread";
}

int CaptureThread::writer(void *ptr, size_t size, size_t nmemb, string stream)
{
    stream = string(static_cast<const char*>(ptr), size * nmemb);
    return size*nmemb;
}

/*
 * getQRCode
 * Descrizione: Riconosce un eventuale codice qr presente in un'immagine
 * Parametri:
 * - frame: frame da controllare
 * */
QString CaptureThread::getQRCode(cv::Mat frame){
    QrDecoderHandle decoder=qr_decoder_open();

    IplImage iplframe = frame;
    qr_decoder_decode_image(decoder,&iplframe);

    //
    // ottiene gli headers del QR code
    //
    QrCodeHeader header;
    QString return_string = "";
    if(qr_decoder_get_header(decoder,&header)){
        // Ottiene il testo del QR code
        char *buf=new char[header.byte_size+1];
        qr_decoder_get_body(decoder,(unsigned char *)buf,header.byte_size+1);
        return_string = buf;
    }

    // finalizza
    qr_decoder_close(decoder);

    return return_string;
}

/*
 * MatToQImage
 * Descrizione: Trasforma un frame di uscita dalla webcam nel formato supportato dalle librerie Qt
 * Parametri:
 * - mat: frame da convertire
 * */
QImage CaptureThread::MatToQImage(const Mat& mat)
{
    // 8-bits unsigned, NO. DI CANALI=1
    if(mat.type()==CV_8UC1)
    {
        // Imposta la palette dei colori (usata per riportare l'indici dei colori nel formato qRgb)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copia il dato dell'immagine
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Crea l'immagine nella classe QImage
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. DI CANALI=3
    if(mat.type()==CV_8UC3)
    {
        // Copia il dato dell'immagine
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Crea l'immagine nella classe QImage
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: il formato del Mat è sconosciuto.";
        return QImage();
    }
}

bool CaptureThread::getUserJson(QString id){
    CURL *curl;
    CURLcode code;

    curl = curl_easy_init();
    if(curl == NULL){
        qDebug() << "Impossibile inizializzare la libreria cURL";
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_URL, ("http://localhost/infmedica/getData.php?id="+id.toUpper().toStdString()).c_str());
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_URL";
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CaptureThread::writer);
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_WRITEFUNCTION";
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsondata);
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_WRITEDATA";
        return false;
    }
    code = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (code != CURLE_OK){
        qDebug() << "Connessione all'indirizzo http://localhost/infmedica/getData.php?id=" << id << "fallita";
        return false;
    }
    qDebug() << QString::fromStdString(jsondata);
    return true;
}

void CaptureThread::getUserImage(QString url){
    QNetworkAccessManager* manager = new QNetworkAccessManager ();
    QNetworkRequest req;
    qDebug() << "Connessione all'immagine" << url;
    req.setUrl(QUrl(url));
    req.setRawHeader("User-Agent", "Prova");
    rep = manager->get(req);

    connect (rep, SIGNAL(finished()), this, SLOT(replyFinish ()));

}

void CaptureThread::replyFinish (){
    qDebug() << "Replay";
    QImage* img2 = new QImage();
    img2->loadFromData(rep->readAll());
    emit setImage(*img2);
}

void CaptureThread::parseJson(){
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    reader.parse( jsondata, root );
    emit pushData(root);
    getUserImage(QString::fromStdString(root["anagrafica"]["foto"].asString()));
}
