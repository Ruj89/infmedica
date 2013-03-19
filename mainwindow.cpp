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
    captureThread = new CaptureThread();
    captureThread->start();
    // Configura gli eventi per il disegno dell'UI
    connect(captureThread,SIGNAL(newFrame(QImage)),this,SLOT(draw(QImage)));
    qRegisterMetaType<Json::Value> ("Json::Value");
    connect(captureThread,SIGNAL(pushData(Json::Value)),this,SLOT(setValues(Json::Value)));
    connect(captureThread,SIGNAL(setImage(QImage)),this,SLOT(setUserImage(QImage)));
    connect(captureThread,SIGNAL(setState(QString)),this,SLOT(setAppState(QString)));
    connect(this,SIGNAL(setState(QString)),this,SLOT(setAppState(QString)));
    connect(this,SIGNAL(stopMainThread()),captureThread,SLOT(endMainLoop()));
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

/*
 * keyReleaseEvent
 * Descrizione: Evento di rilevazione di un pulsante premuto
 * Parametri:
 * - event: evento da tastiera
 * */
void MainWindow::keyReleaseEvent ( QKeyEvent * event ){
    // Chiude il programma quando si preme ESC
    if(event->key() == Qt::Key_Escape){
        emit stopMainThread();
        close();
    }
}

/*
 * setAppState
 * Descrizione: Imposta la stringa di stato
 * Parametri:
 * - string: stringa da impostare
 * */
void MainWindow::setAppState(QString string){
    ui->labelState->setText(string);
}

/*
 * setValues
 * Descrizione: Riempe i campi con i dati del cliente
 * Parametri:
 * - values: array di valori
 * */
void MainWindow::setValues (Json::Value values){
    qDebug() << QString::fromStdString((values["anagrafica"]["Cf"]).asString());
    ui->labelCF->setText(QString::fromStdString(values["anagrafica"]["Cf"].asString()));
    ui->labelNome->setText(QString::fromStdString(values["anagrafica"]["nome"].asString()));
    ui->labelCognome->setText(QString::fromStdString(values["anagrafica"]["cognome"].asString()));
    // Trasforma la stringa con la data di nascita nel formato italiano
    QDate datenew(QDate::fromString(
                              QString::fromStdString(values["anagrafica"]["dataNascita"].asString()),
                              "yyyy-MM-dd"));
    ui->labelData->setText(datenew.toString("dd-MM-yyyy"));
    ui->labelTelefono->setText(QString::fromStdString(values["anagrafica"]["telefono"].asString()));
    // Rimuove tutte le righe della tabella
    while (ui->tableWidget->rowCount() > 0)
        ui->tableWidget->removeRow(0);
    for(unsigned int i=0;i<values["medicine"].size();i++){
        // Inserisce una riga nella tabella
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
        int row = ui->tableWidget->rowCount()-1;
        qDebug() << QString::fromStdString(values["medicine"][i]["nomeMedicina"].asString());
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(values["medicine"][i]["nomeMedicina"].asString())));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(values["medicine"][i]["qnt"].asString())));
        // Trasforma la stringa con l'orario della somministrazione nel formato ora:minuti
        QTime timenew(QTime::fromString(
                        QString::fromStdString(values["medicine"][i]["orarioSomministrazione"].asString()),
                        "hh:mm:ss"));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(timenew.toString("hh:mm")));
    }

    emit setState("Dati caricati con successo!");
}

/*
* draw
* Descrizione: Evento di disegno dell'immagine della fototessera sulla label apposita
* Parametri:
* - img: immagine da visualizzare
* */
void MainWindow::setUserImage (QImage img){
    img = img.scaled(ui->label->width(), ui->label->height());
    ui->label->setPixmap(QPixmap::fromImage(img));
    emit setState("Immagine caricata con successo!");
}
