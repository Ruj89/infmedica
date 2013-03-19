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
    connect(captureThread,SIGNAL(setState(QString)),this,SLOT(setAppState(QString)));
    connect(this,SIGNAL(setState(QString)),this,SLOT(setAppState(QString)));
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

void MainWindow::setAppState(QString string){
    ui->labelState->setText(string);
}

void MainWindow::setValues (Json::Value values){
    qDebug() << QString::fromStdString((values["anagrafica"]["Cf"]).asString());
    ui->labelCF->setText(QString::fromStdString(values["anagrafica"]["Cf"].asString()));
    ui->labelNome->setText(QString::fromStdString(values["anagrafica"]["nome"].asString()));
    ui->labelCognome->setText(QString::fromStdString(values["anagrafica"]["cognome"].asString()));
    QDate datenew(QDate::fromString(
                              QString::fromStdString(values["anagrafica"]["dataNascita"].asString()),
                              "yyyy-MM-dd"));
    ui->labelData->setText(datenew.toString("dd-MM-yyyy"));
    ui->labelTelefono->setText(QString::fromStdString(values["anagrafica"]["telefono"].asString()));
    while (ui->tableWidget->rowCount() != 0)
        ui->tableWidget->removeRow(0);
    for(unsigned int i=0;i<values["medicine"].size();i++){
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
        int row = ui->tableWidget->rowCount()-1;
        qDebug() << QString::fromStdString(values["medicine"][i]["nomeMedicina"].asString());
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(values["medicine"][i]["nomeMedicina"].asString())));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(values["medicine"][i]["qnt"].asString())));
        QTime timenew(QTime::fromString(
                        QString::fromStdString(values["medicine"][i]["orarioSomministrazione"].asString()),
                        "hh:mm:ss"));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(timenew.toString("hh:mm")));
    }

    emit setState("Dati caricati con successo!");
}

void MainWindow::setUserImage (QImage img){
    img = img.scaled(ui->label->width(), ui->label->height());
    ui->label->setPixmap(QPixmap::fromImage(img));
    emit setState("Immagine caricata con successo!");
}
