#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturethread.h"
#include <jsoncpp/json/json.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Configura l'ambiente grafico
    ui->setupUi(this);
    QWidget::showFullScreen();
    //QWidget::showMaximized();

    // Inizializza e avvia il thread per il riconoscimento dell'immagine
    CaptureThread *captureThread = new CaptureThread();
    captureThread->start();
    // Configura l'evento per il disegno dell'UI
    connect(captureThread,SIGNAL(newFrame(QImage)),this,SLOT(draw(QImage)));
    qRegisterMetaType<Json::Value> ("Json::Value");
    connect(captureThread,SIGNAL(pushData(Json::Value)),this,SLOT(setValues(Json::Value)));
    connect(captureThread,SIGNAL(setImage(QImage)),this,SLOT(setUserImage(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * draw
 * Descrizione: Evento di disegno dell'immagine della webcam sulla label
 * Parametri:
 * - img: immagine da visualizzare
 * */
void MainWindow::draw(QImage img){
    // Scala l'immagine nella dimensione voluta
    img = img.scaled(ui->WidgetImageLabel->width(), ui->WidgetImageLabel->height());
    // Associa l'immagine alla label
    ui->WidgetImageLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::keyReleaseEvent ( QKeyEvent * event ){
    if(event->key() == Qt::Key_Escape) close();
}

void MainWindow::setValues (Json::Value values){
    qDebug() << QString::fromStdString((values["anagrafica"]["Cf"]).asString());
    ui->labelCF->setText(QString::fromStdString(values["anagrafica"]["Cf"].asString()));
    ui->labelNome->setText(QString::fromStdString(values["anagrafica"]["nome"].asString()));
    ui->labelCognome->setText(QString::fromStdString(values["anagrafica"]["cognome"].asString()));
    ui->labelData->setText(QString::fromStdString(values["anagrafica"]["dataNascita"].asString()));
    ui->labelTelefono->setText(QString::fromStdString(values["anagrafica"]["telefono"].asString()));
}

void MainWindow::setUserImage (QImage img){
    img = img.scaled(ui->label->width(), ui->label->height());
    ui->label->setPixmap(QPixmap::fromImage(img));
}
