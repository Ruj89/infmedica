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
    empty_code = 0;
    stopFlag = false;
}



void CaptureThread::run()
{
    cv::VideoCapture capture;
    cv::Mat frame;
    QImage img;
    int cameraIndex = 0;

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
            empty_code=0;
            if (qrcode_decoded == qrcode){
                timesCheckedQRCode++;
                qDebug() << "Stringa individuata" << timesCheckedQRCode << "volte.";
            } else {
                emit setState("Cattura codice");
                timesCheckedQRCode = 1;
                qrcode = QString(qrcode_decoded);
            }
        } else empty_code++;

        // Segnala lo stato di attesa
        if(empty_code>50) emit setState("In attesa");

        // Il codice è prelevato con successo, quindi richiesta al webserver
        if(timesCheckedQRCode == 5 && qrcode_old != qrcode){
            emit setState("Codice riconoscito!");
            qDebug() << "Stringa individuata 5 volte:" << qrcode;
            if(getUserJson(qrcode)){
                qrcode_old = qrcode;
                // Parsing della JSON string ricavata
                parseJson();
            }
        }

        // Converti l'immagine in un formato delle Qt e disegnalo
        img = MatToQImage(frame);
        emit newFrame(img);
    }
    capture.release();
    qDebug() << "Chiusura del thread";
}

/*
 * writer
 * Descrizione: Scrittura della risposta dalla cURL in una stringa
 * Parametri: come CURLOPT_WRITEFUNCTION
 * */
int CaptureThread::writer(void *ptr, size_t size, size_t nmemb, string stream)
{
    stream = string(static_cast<const char*>(ptr), size * nmemb);
    return size*nmemb;
}

/*
 * writerImage
 * Descrizione: Scrittura della risposta dalla cURL in un array di byte
 * Parametri: come CURLOPT_WRITEFUNCTION
 * */
int CaptureThread::writerImage(void *ptr, size_t size, size_t nmemb, QByteArray buffer)
{
    buffer.append(static_cast<const char*>(ptr), size * nmemb);
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

/*
 * getUserJson
 * Descrizione: Ricava i dati in formato JSON dal server
 * Parametri:
 * - id: id del paziente
 * */
bool CaptureThread::getUserJson(QString id){
    CURL *curl;
    CURLcode code;
    emit setState("Caricamento dei dati");

    // Inizializza la libreria
    curl = curl_easy_init();
    if(curl == NULL){
        qDebug() << "Impossibile inizializzare la libreria cURL";
        return false;
    }
    // Imposta l'indirizzo da prelevare
    code = curl_easy_setopt(curl, CURLOPT_URL, ("http://localhost/infmedica/getData.php?id="+id.toUpper().toStdString()).c_str());
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_URL";
        return false;
    }
    // Imposta la funzione di writer
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CaptureThread::writer);
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_WRITEFUNCTION";
        return false;
    }
    // Imposta la variabile sulla quale scrivere
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsondata);
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_WRITEDATA";
        return false;
    }
    // Effettua la richiesta al server
    code = curl_easy_perform(curl);
    // Fa il cleanup della richiesta
    curl_easy_cleanup(curl);
    if (code != CURLE_OK){
        qDebug() << "Connessione all'indirizzo http://localhost/infmedica/getData.php?id=" << id << "fallita";
        return false;
    }
    qDebug() << QString::fromStdString(jsondata);
    return true;
}

/*
 * getUserImage
 * Descrizione: Ricava l'immagine formato fototessera del paziente
 * Parametri:
 * - url: indirizzo web dell'immagine
 * */
void CaptureThread::getUserImage(QString url){
    emit setState("Caricamento della fototessera");
    CURL *curl;
    CURLcode code;

    // Inizializza la libreria
    curl = curl_easy_init();
    if(curl == NULL){
        qDebug() << "Impossibile inizializzare la libreria cURL";
    }
    // Imposta l'indirizzo da prelevare
    code = curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_URL";
    }
    // Imposta la funzione di writer
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CaptureThread::writerImage);
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_WRITEFUNCTION";
    }
    // Imposta la variabile sulla quale scrivere
    userimage = QByteArray();
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &userimage);
    if (code != CURLE_OK){
        qDebug() << "Impossibile impostare CURLOPT_WRITEDATA";
    }
    // Effettua la richiesta al server
    code = curl_easy_perform(curl);
    // Fa il cleanup della richiesta
    curl_easy_cleanup(curl);
    if (code != CURLE_OK){
        qDebug() << "Connessione all'indirizzo" << url << "fallita";
    }
    QImage* img2 = new QImage();
    // Trasforma i raw data nel formato immagine delle Qt
    img2->loadFromData(userimage);
    emit setImage(*img2);
}

/*
 * parseJson
 * Descrizione: Effettua il parsing della stringa JSON da inviare alla view e ricava
 *      l'url della fototessera da inserire
 * */
void CaptureThread::parseJson(){
    Json::Value root;
    Json::Reader reader;
    // Effettua il parsing del JSON
    reader.parse( jsondata, root );
    emit pushData(root);
    // Ottieni l'immagine formato fototessera
    getUserImage(QString::fromStdString(root["anagrafica"]["foto"].asString()));
}


void CaptureThread::endMainLoop(){
    stopFlag = true;
}
